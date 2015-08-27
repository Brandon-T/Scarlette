#include "Debugger.hxx"

BreakPoint::BreakPoint(HANDLE hProcess, void* address) : hProcess(hProcess), address(address), opcode(0xCC), instruction(0x00) {}

bool BreakPoint::Enable()
{
    SIZE_T amount = 0;
    if (ReadProcessMemory(hProcess, address, &instruction, sizeof(std::uint8_t), &amount) && amount == sizeof(std::uint8_t))
    {
        if (WriteProcessMemory(hProcess, address, &opcode, sizeof(std::uint8_t), &amount) && amount == sizeof(std::uint8_t))
        {
            FlushInstructionCache(hProcess, address, sizeof(std::uint8_t));
            return true;
        }
    }

    instruction = 0x00;
    return false;
}

bool BreakPoint::Disable()
{
    SIZE_T amount = 0;
    if (instruction != 0x00)
    {
        if (WriteProcessMemory(hProcess, address, &instruction, sizeof(std::uint8_t), &amount) && amount == sizeof(std::uint8_t))
        {
            FlushInstructionCache(hProcess, address, sizeof(std::uint8_t));
            instruction = 0x00;
            return true;
        }
    }
    return false;
}

bool BreakPoint::IsEnabled() const
{
    return instruction != 0x00;
}


Debugger::Debugger() : hEvent(CreateEventW(nullptr, false, false, L"ADBGEvent")) {}

Debugger::~Debugger() {CloseHandle(hEvent);}

void Debugger::Start(std::string path, std::string directory, std::string arguments, std::ostream &stream)
{
    breakpoint_hit = false;
    debug_thread = std::thread([this, path, directory, arguments, &stream] {
        DEBUG_EVENT event = {0};
        process.reset(new Process{path, directory, arguments, 0});
        process->Start(true, &stream);

        while(true)
        {
            if(!WaitForDebugEvent(&event, INFINITE))
                break;

            switch(event.dwDebugEventCode)
            {
                case CREATE_PROCESS_DEBUG_EVENT:
                {
                    wchar_t path[MAX_PATH] = {0};
                    GetFileName(event.u.CreateProcessInfo.hFile, &path[0]);
                    OnProcessEvent(DebugEvent{DebugEventType::CREATE_PROCESS_EVENT, event.u.CreateProcessInfo.hFile, event.u.CreateProcessInfo.hProcess, event.u.CreateProcessInfo.hThread, reinterpret_cast<void*>(event.u.CreateProcessInfo.lpStartAddress), nullptr, path, 0, event.dwProcessId, event.dwThreadId, 0});
                    ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                    CloseHandle(event.u.CreateProcessInfo.hFile);
                }
                break;

                case EXIT_PROCESS_DEBUG_EVENT:
                {
                    OnProcessEvent(DebugEvent{DebugEventType::TERMINATE_PROCESS_EVENT, event.u.CreateProcessInfo.hFile, event.u.CreateProcessInfo.hProcess, event.u.CreateProcessInfo.hThread, reinterpret_cast<void*>(event.u.CreateProcessInfo.lpStartAddress), nullptr, L"", event.u.ExitProcess.dwExitCode, event.dwProcessId, event.dwThreadId, 0});
                    ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                    return;
                }
                break;

                case CREATE_THREAD_DEBUG_EVENT:
                {
                    OnThreadEvent(DebugEvent{DebugEventType::CREATE_THREAD_EVENT, nullptr, nullptr, event.u.CreateThread.hThread, reinterpret_cast<void*>(event.u.CreateThread.lpStartAddress), event.u.CreateThread.lpThreadLocalBase, L"", 0, event.dwProcessId, event.dwThreadId, 0});
                    ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                }
                break;

                case EXIT_THREAD_DEBUG_EVENT:
                {
                    OnThreadEvent(DebugEvent{DebugEventType::TERMINATE_THREAD_EVENT, nullptr, nullptr, event.u.CreateThread.hThread, reinterpret_cast<void*>(event.u.CreateThread.lpStartAddress), event.u.CreateThread.lpThreadLocalBase, L"", event.u.ExitThread.dwExitCode, event.dwProcessId, event.dwThreadId, 0});
                    ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                }
                break;

                case LOAD_DLL_DEBUG_EVENT:
                {
                    wchar_t path[MAX_PATH] = {0};
                    //GetFileName(event.u.LoadDll.hFile, &path[0]);

                    SIZE_T size = 0;
                    void *ptr = nullptr;
                    ReadProcessMemory(process->GetHandle(), event.u.LoadDll.lpImageName, &ptr, sizeof(ptr), &size);
                    ReadProcessMemory(process->GetHandle(), ptr, &path, sizeof(path), &size);



                    OnModuleEvent(DebugEvent{DebugEventType::LOAD_MODULE_EVENT, event.u.LoadDll.hFile, nullptr, nullptr, event.u.LoadDll.lpBaseOfDll, nullptr, path, 0, event.dwProcessId, event.dwThreadId, 0});
                    ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                    CloseHandle(event.u.LoadDll.hFile);
                }
                break;

                case UNLOAD_DLL_DEBUG_EVENT:
                {
                    OnModuleEvent(DebugEvent{DebugEventType::UNLOAD_MODULE_EVENT, nullptr, nullptr, nullptr, event.u.UnloadDll.lpBaseOfDll, nullptr, L"", 0, event.dwProcessId, event.dwThreadId, 0});
                    ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                }
                break;

                case OUTPUT_DEBUG_STRING_EVENT:
                {
                    OUTPUT_DEBUG_STRING_INFO &DebugString = event.u.DebugString;

                    if (DebugString.fUnicode)
                    {
                        std::wstring details;
                        details.resize(DebugString.nDebugStringLength);
                        ReadProcessMemory(process->GetHandle(), DebugString.lpDebugStringData, &details[0], DebugString.nDebugStringLength, nullptr);
                        OnDebugOutputEvent(DebugEvent{DebugEventType::DEBUG_STRING_EVENT, nullptr, nullptr, nullptr, nullptr, nullptr, details, 0, event.dwProcessId, event.dwThreadId, 0});
                    }
                    else
                    {
                        std::string details;
                        details.resize(DebugString.nDebugStringLength);
                        ReadProcessMemory(process->GetHandle(), DebugString.lpDebugStringData, &details[0], DebugString.nDebugStringLength, nullptr);
                        OnDebugOutputEvent(DebugEvent{DebugEventType::DEBUG_STRING_EVENT, nullptr, nullptr, nullptr, nullptr, nullptr, std::wstring(details.begin(), details.end()), 0, event.dwProcessId, event.dwThreadId, 0});
                    }

                    ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                }
                break;

                case EXCEPTION_DEBUG_EVENT:
                {
                    if (event.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT || event.u.Exception.ExceptionRecord.ExceptionCode == 0x4000001f)
                    {
                        if (breakpoint_hit)
                        {
                            OnBreakPoint(DebugEvent{DebugEventType::BREAKPOINT_EVENT, nullptr, nullptr, nullptr, event.u.Exception.ExceptionRecord.ExceptionAddress, nullptr, L"", event.u.Exception.ExceptionRecord.ExceptionCode, event.dwProcessId, event.dwThreadId, 0});
                            ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                        }
                        else
                        {
                            breakpoint_hit = true;
                            ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                        }
                    }
                    else if (event.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP)
                    {
                        OnSingleStep(DebugEvent{DebugEventType::SINGLE_STEP_EVENT, nullptr, nullptr, nullptr, event.u.Exception.ExceptionRecord.ExceptionAddress, nullptr, L"", event.u.Exception.ExceptionRecord.ExceptionCode, event.dwProcessId, event.dwThreadId, 0});
                        ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                    }
                    else
                    {
                        std::uintmax_t flags = event.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_ACCESS_VIOLATION ? event.u.Exception.ExceptionRecord.ExceptionInformation[0] : 0;
                        void* address = event.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_ACCESS_VIOLATION ? reinterpret_cast<void*>(event.u.Exception.ExceptionRecord.ExceptionInformation[1]) : nullptr;
                        OnExceptionEvent(DebugEvent{DebugEventType::EXCEPTION_EVENT, nullptr, nullptr, nullptr, address, nullptr, L"", event.u.Exception.ExceptionRecord.ExceptionCode, event.dwProcessId, event.dwThreadId, flags});
                        ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_EXCEPTION_NOT_HANDLED);
                    }
                }
                break;

                default:
                {
                    ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                }
                break;
            }
        }
    });
}

void Debugger::Stop()
{
    if (process && process->GetHandle())
    {
        DebugActiveProcessStop(GetProcessId(process->GetHandle()));
    }

    if (debug_thread.joinable())
    {
        debug_thread.join();
    }
}


void Debugger::Step()
{
    if (lastBreakpoint)
    {
        isStepping = true;
        SetContext(activeThread, false);
        SetEvent(hEvent);
    }
}

void Debugger::Continue()
{
    isStepping = false;
    SetEvent(hEvent);
}

void Debugger::AddBreakpoint(const void* address)
{
    breakpoints.emplace(address, BreakPoint{process->GetHandle(), const_cast<void*>(address)});
}

void Debugger::RemoveBreakpoint(const void* address)
{
    auto it = breakpoints.find(address);
    if (it != breakpoints.end())
    {
        breakpoints.erase(it);
    }
}

BreakPoint* Debugger::FindBreakpoint(const void* address)
{
    auto it = breakpoints.find(address);
    if (it != breakpoints.end())
    {
        return &it->second;
    }
    return nullptr;
}

void Debugger::AddListener(DebugEventType type, std::function<void(const DebugEvent &event)> &&listener)
{
    listeners.emplace(type, std::forward<std::function<void(const DebugEvent &event)>>(listener));
}

void Debugger::RemoveListener(DebugEventType type)
{
    auto it = listeners.find(type);
    if (it != listeners.end())
    {
        listeners.erase(it);
    }
}

void Debugger::OnModuleEvent(const DebugEvent &event)
{
    auto it = listeners.find(event.type);
    if (it != listeners.end())
    {
        it->second(event);
    }
}

void Debugger::OnProcessEvent(const DebugEvent &event)
{
    auto it = listeners.find(event.type);
    if (it != listeners.end())
    {
        it->second(event);
    }
}

void Debugger::OnThreadEvent(const DebugEvent &event)
{
    auto it = listeners.find(event.type);
    if (it != listeners.end())
    {
        it->second(event);
    }
}

void Debugger::OnDebugOutputEvent(const DebugEvent &event)
{
    auto it = listeners.find(event.type);
    if (it != listeners.end())
    {
        it->second(event);
    }
}

void Debugger::OnExceptionEvent(const DebugEvent &event)
{
    auto it = listeners.find(event.type);
    if (it != listeners.end())
    {
        it->second(event);
    }
}

void Debugger::OnBreakPoint(const DebugEvent &event)
{
    auto it = listeners.find(event.type);
    if (it != listeners.end())
    {
        it->second(event);
    }

    BreakPoint* breakpoint = FindBreakpoint(event.lpBaseAddress);
    if (breakpoint != nullptr)
    {
        if (breakpoint->Disable())
        {
            activeThread = event.tid;
            lastBreakpoint = breakpoint;

            SetContext(event.tid, true);
            WaitForSingleObject(hEvent, INFINITE);
        }
    }
}

void Debugger::OnSingleStep(const DebugEvent &event)
{
    auto it = listeners.find(event.type);
    if (it != listeners.end())
    {
        it->second(event);
    }

    if (isStepping)
    {
        activeThread = event.tid;
        WaitForSingleObject(hEvent, INFINITE);
    }

    if (lastBreakpoint && !lastBreakpoint->IsEnabled())
    {
        lastBreakpoint->Enable();
    }
}

void Debugger::SetContext(std::uint32_t threadId, bool adjustInstruction)
{
    CONTEXT ctx = {0};
    ctx.ContextFlags = CONTEXT_ALL;
    HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, false, threadId);
    if (hThread)
    {
        GetThreadContext(hThread, &ctx);

        if (adjustInstruction)
        {
            #if defined(__i386__)
            --ctx.Eip; //ctx.Eip = reinterpret_cast<std::ptrdiff_t>(event.lpBaseAddress);
            #elif defined(__x86_64__)
            --ctx.Rip; //ctx.Rip = reinterpret_cast<std::ptrdiff_t>(event.lpBaseAddress);
            #else
            #error "Unsupported architecture"
            #endif
        }

        ctx.EFlags |= 0x100;
        SetThreadContext(hThread, &ctx);
        CloseHandle(hThread);
    }
}
