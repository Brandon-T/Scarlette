#ifndef DEBUGWINDOW_HXX_INCLUDED
#define DEBUGWINDOW_HXX_INCLUDED

#include <windows.h>
#include "Form.hxx"
#include "Controls.hxx"
#include "Utilities.hxx"
#include "Graphics.hxx"
#include <iostream>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <tuple>
#include <d3d9.h>
#include <d3dx9.h>

#include "Process.hxx"
#include "Debugger.hxx"
#include <Uxtheme.h>


#define DEBUG_PROCESS_EVENT    WM_APP + 10000

class DebugWindow : public Form
{
private:
    std::vector<std::tuple<std::uint32_t, DebugEvent>> events;

    CONTEXT ctx;
    bool parseExceptions;
    Debugger debugger;
    std::vector<Control*> controls;
    std::vector<HBRUSH> brushes;
    std::vector<Font*> fonts;

    IDirect3D9* d3d9;
    IDirect3DDevice9* d3ddev9;

    MenuBar* menubar;
    ToolBar* topbar;
    ReBar* rebar;
    StatusBar *statusbar;

    Panel* leftpanel;
    Panel* bottompanel;

    Label* registerinfobar;
    Label* registerinfotext;
    ListBox* registerinfobox;



    Label* processinfobar;
    Label* callhistorybar;
    Label* breakpointsbar;
    Label* debugprocessbar;

    Label* processinfotext;
    Label* callhistorytext;
    Label* breakpointstext;
    Label* debugprocesstext;

    Panel* processinfopanel;
    Label* processinfobox;
    ListBox* callhistorybox;
    ListBox* breakpointsbox;
    ListBox* debugprocessbox;

    void SetTextBarBackground(Label* textbar);
    void InitD3D9(std::uint32_t width, std::uint32_t height);
    void DestroyD3D9();
    void OnD3D9Draw();
    void GetWindowCenter(int &centerX, int &centerY, int width, int height);

    void ProcessRegisters(const DebugEvent &event);
    void ProcessExceptions(const DebugEvent &event);
    std::wstring GetExceptionDetails(const DebugEvent &event);


public:
    DebugWindow(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, Form* Parent = nullptr);
    ~DebugWindow();

    int MessageLoop();

    void StartProcessDebugging();
};

#endif // DEBUGWINDOW_HXX_INCLUDED
