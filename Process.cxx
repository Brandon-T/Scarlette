#include "Process.hxx"

Process::Process(std::string path, std::string directory, std::string arguments, std::uint32_t timeout) : path(path), directory(directory), arguments(arguments), timeout(timeout), hRead(INVALID_HANDLE_VALUE), hWrite(INVALID_HANDLE_VALUE), si(), pi(), readthread()
{
    SECURITY_ATTRIBUTES attr;
    ZeroMemory(&attr, sizeof(attr));
    attr.nLength = sizeof(attr);
    attr.bInheritHandle = true;
    CreatePipe(&hRead, &hWrite, &attr, 0);
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);
}

Process::~Process()
{
    this->Stop();
}

bool Process::Start(bool debug, std::ostream* os)
{
    this->os = os;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);
    si.hStdInput = nullptr;
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.dwFlags |= STARTF_USESTDHANDLES;

    std::string commandline = path + " " + arguments;

    if (CreateProcess(nullptr, &commandline[0], nullptr, nullptr, true, debug ? DEBUG_ONLY_THIS_PROCESS : NORMAL_PRIORITY_CLASS, 0, directory.c_str(), &si, &pi))
    {
        CloseHandle(hWrite);
        if (timeout != 0)
        {
            readthread = std::thread(&HandleRead, this);
            WaitForSingleObject(pi.hProcess, timeout);
            this->Stop();
            return false;
        }

        if (pi.hProcess == INVALID_HANDLE_VALUE)
        {
            this->Stop();
            return false;
        }

        readthread = std::thread(&HandleRead, this);
        return true;
    }

    this->Stop();
    return false;
}

void Process::Stop()
{
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hRead);
    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));

    if (readthread.joinable())
    {
        readthread.join();
        readthread = std::thread();
        hRead = INVALID_HANDLE_VALUE;
    }
}

bool Process::ReadPipe(HANDLE hPipe, std::string& Buffer)
{
    DWORD dwRead = 0;
    Buffer.clear();
    Buffer.resize(256);
    bool Result = ReadFile(hPipe, &Buffer[0], Buffer.size(), &dwRead, NULL);
    Buffer.resize(dwRead);
    return Result && dwRead;
}

bool Process::PeakReadPipe(HANDLE hPipe, std::string &Buffer)
{
    DWORD dwRead = 0;
    DWORD dwBytesLeft = 0;
    DWORD dwTotalAvailBytes = 0;
    if (PeekNamedPipe(hPipe, NULL, 256, &dwRead, &dwTotalAvailBytes, &dwBytesLeft) && dwRead)
    {
        return this->ReadPipe(hPipe, Buffer);
    }
    return false;
}

void Process::HandleRead()
{
    std::string Buffer;
    HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    while (true)
    {
        if (!ReadPipe(hRead, Buffer))
        {
            if (GetLastError() == ERROR_BROKEN_PIPE)
                break;
        }

        if (!Buffer.empty())
        {
            DWORD dwWritten = 0;
            if (this->os)
            {
                this->os->write(Buffer.c_str(), Buffer.size());
            }
            else
            {
                WriteFile(ConsoleOutput, Buffer.c_str(), Buffer.size(), &dwWritten, NULL);
            }
            Buffer.clear();
        }
    }
    CloseHandle(ConsoleOutput);
}
