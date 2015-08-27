#ifndef PROCESS_HXX_INCLUDED
#define PROCESS_HXX_INCLUDED

#include <windows.h>
#include <string>
#include <thread>
#include <iostream>
#include <atomic>

class Process
{
private:
    std::string path;
    std::string directory;
    std::string arguments;
    std::uint32_t timeout;

    HANDLE hRead;
    HANDLE hWrite;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    std::thread readthread;
    std::ostream* os;

    bool ReadPipe(HANDLE hPipe, std::string& Buffer);
    bool PeakReadPipe(HANDLE hPipe, std::string &Buffer);
    void HandleRead();

public:
    Process(std::string path, std::string directory, std::string arguments, std::uint32_t timeout);
    ~Process();

    bool Start(bool debug = false, std::ostream* os = nullptr);
    void Stop();

    HANDLE GetHandle() const {return pi.hProcess;}
};

#endif // PROCESS_HXX_INCLUDED
