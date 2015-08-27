#ifndef DEBUGGER_HXX_INCLUDED
#define DEBUGGER_HXX_INCLUDED

#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <map>

#include "Process.hxx"
#include "Utilities.hxx"

enum class DebugEventType : std::int16_t
{
    UNKNOWN_EVENT,
    CREATE_PROCESS_EVENT,
    TERMINATE_PROCESS_EVENT,
    CREATE_THREAD_EVENT,
    TERMINATE_THREAD_EVENT,
    LOAD_MODULE_EVENT,
    UNLOAD_MODULE_EVENT,
    DEBUG_STRING_EVENT,
    EXCEPTION_EVENT,
    BREAKPOINT_EVENT,
    SINGLE_STEP_EVENT
};

struct DebugEventTypeHash
{
    template<typename T>
    std::size_t operator()(T t) const { return static_cast<std::size_t>(t); }
};

struct DebugEvent
{
    DebugEventType type;
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    const void* lpBaseAddress;
    const void* lpLocalAddress;
    std::wstring path;
    std::uintmax_t code;
    std::uint32_t pid;
    std::uint32_t tid;
    std::uintmax_t flags;
};

class BreakPoint
{
private:
    HANDLE hProcess;
    void* address;
    std::uint8_t opcode;
    std::uint8_t instruction;

public:
    BreakPoint(HANDLE hProcess, void* address);

    bool Enable();
    bool Disable();
    bool IsEnabled() const;
};



class Debugger
{
private:
    bool breakpoint_hit;
    std::thread debug_thread;
    std::unique_ptr<Process> process;

    HANDLE hEvent;
    bool isStepping;
    std::uint32_t activeThread;
    BreakPoint* lastBreakpoint;
    std::unordered_map<const void*, BreakPoint> breakpoints;
    std::unordered_map<DebugEventType, std::function<void(const DebugEvent &event)>, DebugEventTypeHash> listeners;

    void SetContext(std::uint32_t threadId, bool adjustInstruction);

public:
    Debugger();
    virtual ~Debugger();

    void Start(std::string path, std::string directory, std::string arguments, std::ostream &stream);
    void Stop();

    void Step();
    void Continue();

    void AddBreakpoint(const void* address);
    void RemoveBreakpoint(const void* address);
    BreakPoint* FindBreakpoint(const void* address);

    void AddListener(DebugEventType type, std::function<void(const DebugEvent &event)> &&listener);
    void RemoveListener(DebugEventType type);

protected:
    void OnModuleEvent(const DebugEvent &event);
    void OnProcessEvent(const DebugEvent &event);
    void OnThreadEvent(const DebugEvent &event);
    void OnDebugOutputEvent(const DebugEvent &event);
    void OnExceptionEvent(const DebugEvent &event);
    void OnBreakPoint(const DebugEvent &event);
    void OnSingleStep(const DebugEvent &event);
};

#endif // DEBUGGER_HXX_INCLUDED
