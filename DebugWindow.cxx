#include "DebugWindow.hxx"

DebugWindow::DebugWindow(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, Form* Parent) : Form(Title, Location, Width, Height, Parent)
{
    this->parseExceptions = false;
    SetExtendedWindowStyle(*this, WindowStyle::EX_TOOLWINDOW | WindowStyle::EX_APPWINDOW);


    #pragma mark MenuBars
    this->menubar = new MenuBar(*this);
    MenuItem* file = new MenuItem(this->menubar, MenuStyle::MS_STRING, L"File");
    MenuItem* edit = new MenuItem(this->menubar, MenuStyle::MS_STRING, L"Edit");
    MenuItem* open = new MenuItem(file, MenuStyle::MS_STRING | MenuStyle::MS_POPUP, L"Open");
    MenuItem* sub = new MenuItem(open, MenuStyle::MS_STRING, L"Sub");
    MenuItem* sub2 = new MenuItem(file, MenuStyle::MS_STRING | MenuStyle::MS_CHECKED, L"SUB2");
    menubar->Show();

    this->controls.push_back(menubar);
    this->controls.push_back(file);
    this->controls.push_back(edit);
    this->controls.push_back(open);
    this->controls.push_back(sub);
    this->controls.push_back(sub2);



    #pragma mark Fonts

    Font* hFont = new Font(14, FW_DONTCARE, false, false, L"Arial");
    this->fonts.push_back(hFont);



    #pragma mark ToolBars, Rebars, StatusBars

    this->topbar = new ToolBar({0, 0}, 0, 0, *this);
    this->rebar = new ReBar({0, 0}, 0, 0, this->topbar->GetHandle(), *this->topbar);
    this->rebar->AddBand();
    this->statusbar = new StatusBar({0, 0}, 0, 0, *this);

    this->controls.push_back(this->topbar);
    this->controls.push_back(this->rebar);
    this->controls.push_back(this->statusbar);



    #pragma mark LeftPanel, BottomPanel

    RECT rect = {0};
    GetClientRect(this->rebar->GetHandle(), &rect);
    this->leftpanel = new Panel(nullptr, {0, rect.bottom}, 300, 438, *this);
    this->bottompanel = new Panel(nullptr, {0, this->leftpanel->GetLocation().y + this->leftpanel->GetHeight()}, Width, 182, *this);
    SetWindowStyle(this->leftpanel->GetHandle(), static_cast<WindowStyle>(BS_GROUPBOX), true);
    SetWindowStyle(this->bottompanel->GetHandle(), static_cast<WindowStyle>(BS_GROUPBOX), true);

    this->controls.push_back(this->leftpanel);
    this->controls.push_back(this->bottompanel);



    #pragma mark LeftPanelControls

    this->registerinfobar = new Label(nullptr, {5, 5}, this->leftpanel->GetWidth() - 10, 95, this->leftpanel->GetHandle());
    this->SetTextBarBackground(this->registerinfobar);
    this->controls.push_back(this->registerinfobar);

    this->registerinfotext = new Label(L"Registers", {5, 5}, 290, 95, this->registerinfobar->GetHandle());
    this->registerinfotext->SetFont(*hFont);
    this->controls.push_back(this->registerinfotext);

    this->registerinfobox = new ListBox({this->registerinfobar->GetLocation().x, this->registerinfobar->GetLocation().y + 25}, this->registerinfobar->GetWidth(), this->leftpanel->GetHeight() - 25, this->leftpanel->GetHandle());
    this->registerinfobox->SetFont(*hFont);
    this->controls.push_back(this->registerinfobox);

    #pragma mark BottomPanelControls

    this->processinfobar = new Label(nullptr, {5, 5}, 290, 95, this->bottompanel->GetHandle());
    this->callhistorybar = new Label(nullptr, {this->processinfobar->GetLocation().x + this->processinfobar->GetWidth() + 5, this->processinfobar->GetLocation().y}, this->processinfobar->GetWidth(), this->processinfobar->GetHeight(), this->bottompanel->GetHandle());
    this->breakpointsbar = new Label(nullptr, {this->callhistorybar->GetLocation().x + this->callhistorybar->GetWidth() + 5, this->callhistorybar->GetLocation().y}, this->callhistorybar->GetWidth(), this->callhistorybar->GetHeight(), this->bottompanel->GetHandle());
    this->debugprocessbar = new Label(nullptr, {this->breakpointsbar->GetLocation().x + this->breakpointsbar->GetWidth() + 5, this->breakpointsbar->GetLocation().y}, this->breakpointsbar->GetWidth(), this->breakpointsbar->GetHeight(), this->bottompanel->GetHandle());

    this->SetTextBarBackground(this->processinfobar);
    this->SetTextBarBackground(this->callhistorybar);
    this->SetTextBarBackground(this->breakpointsbar);
    this->SetTextBarBackground(this->debugprocessbar);

    this->controls.push_back(this->processinfobar);
    this->controls.push_back(this->callhistorybar);
    this->controls.push_back(this->breakpointsbar);
    this->controls.push_back(this->debugprocessbar);

    this->processinfotext = new Label(L"Process Information", {5, 5}, 290, 95, this->processinfobar->GetHandle());
    this->callhistorytext = new Label(L"Function Call History", {5, 5}, 290, 95, this->callhistorybar->GetHandle());
    this->breakpointstext = new Label(L"Breakpoints", {5, 5}, 290, 95, this->breakpointsbar->GetHandle());
    this->debugprocesstext = new Label(L"Debug Process Events", {5, 5}, 290, 95, this->debugprocessbar->GetHandle());

    this->processinfotext->SetFont(*hFont);
    this->callhistorytext->SetFont(*hFont);
    this->breakpointstext->SetFont(*hFont);
    this->debugprocesstext->SetFont(*hFont);

    this->controls.push_back(this->processinfotext);
    this->controls.push_back(this->callhistorytext);
    this->controls.push_back(this->breakpointstext);
    this->controls.push_back(this->debugprocesstext);

    this->processinfopanel = new Panel(nullptr, {5, this->processinfobar->GetLocation().y + 25}, this->processinfobar->GetWidth(), this->bottompanel->GetHeight() - 38, this->bottompanel->GetHandle());

    SetWindowStyle(this->processinfopanel->GetHandle(), static_cast<WindowStyle>(WS_BORDER), false);
    SetWindowStyle(this->processinfopanel->GetHandle(), static_cast<WindowStyle>(BS_GROUPBOX), true);

    this->controls.push_back(this->processinfopanel);


    this->processinfobox = new Label(nullptr, {5, 5}, this->processinfopanel->GetWidth() - 5, this->processinfopanel->GetHeight() - 5, this->processinfopanel->GetHandle());
    this->callhistorybox = new ListBox({this->callhistorybar->GetLocation().x, this->processinfopanel->GetLocation().y}, this->callhistorybar->GetWidth(), this->bottompanel->GetHeight() - 25, this->bottompanel->GetHandle());
    this->breakpointsbox = new ListBox({this->breakpointsbar->GetLocation().x, this->callhistorybox->GetLocation().y}, this->breakpointsbar->GetWidth(), this->callhistorybox->GetHeight(), this->bottompanel->GetHandle());
    this->debugprocessbox = new ListBox({this->debugprocessbar->GetLocation().x, this->breakpointsbox->GetLocation().y}, this->debugprocessbar->GetWidth(), this->breakpointsbox->GetHeight(), this->bottompanel->GetHandle());

    this->processinfobox->SetFont(*hFont);
    this->callhistorybox->SetFont(*hFont);
    this->breakpointsbox->SetFont(*hFont);
    this->debugprocessbox->SetFont(*hFont);

    SetWindowStyle(this->callhistorybox->GetHandle(), static_cast<WindowStyle>(WS_VSCROLL | ES_AUTOVSCROLL), false);
    SetWindowStyle(this->breakpointsbox->GetHandle(), static_cast<WindowStyle>(WS_VSCROLL | ES_AUTOVSCROLL), false);
    SetWindowStyle(this->debugprocessbox->GetHandle(), static_cast<WindowStyle>(WS_VSCROLL | ES_AUTOVSCROLL), false);


    this->controls.push_back(this->processinfobox);
    this->controls.push_back(this->callhistorybox);
    this->controls.push_back(this->breakpointsbox);
    this->controls.push_back(this->debugprocessbox);



    #pragma mark Listeners

    this->processinfopanel->AddListener(WM_NCPAINT, [&](UINT msg, WPARAM wp, LPARAM lp)
    {
        HTHEME themeHandle = OpenThemeData(this->processinfopanel->GetHandle(), L"LISTBOX");

        if(themeHandle)
        {
            RECT rc = {0};
            HDC hdc = GetWindowDC(this->processinfopanel->GetHandle());
            int cxBorder = GetSystemMetrics(SM_CXBORDER);
            int cyBorder = GetSystemMetrics(SM_CYBORDER);
            GetClientRect(this->processinfopanel->GetHandle(), &rc);
            OffsetRect(&rc, cxBorder, cyBorder);
            ExcludeClipRect(hdc, rc.left, rc.top, rc.right, rc.bottom);
            InflateRect(&rc, cxBorder, cyBorder);
            DrawThemeBackground(themeHandle, hdc, 0, 0, &rc, NULL);
            CloseThemeData(themeHandle);

            ReleaseDC(this->processinfopanel->GetHandle(), hdc);
        }
    });

    this->AddListener(WM_CTLCOLORSTATIC, [&](UINT msg, WPARAM wp, LPARAM lp) -> LRESULT
    {
        if(reinterpret_cast<HWND>(lp) == this->leftpanel->GetHandle() || reinterpret_cast<HWND>(lp) == this->bottompanel->GetHandle())
        {
            HDC DC = reinterpret_cast<HDC>(wp);
            SetBkColor(DC, GetSysColor(COLOR_WINDOW));
            return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
        }
        HDC DC = reinterpret_cast<HDC>(wp);
        SetBkMode(DC, TRANSPARENT);
        return (LRESULT)GetStockObject(NULL_BRUSH);
    });

    this->AddListener(WM_SIZE, [&](UINT msg, WPARAM wp, LPARAM lp)
    {
        this->topbar->AdjustSize();
        this->rebar->AdjustSize();
        this->statusbar->AdjustSize();
    });

    this->AddListener(WM_INITMENUPOPUP, [&](UINT msg, WPARAM wp, LPARAM lp)
    {
        UNREFERENCED_PARAMETER(msg);
        if(!!!HIWORD(lp) && wp)
        {
            MenuItem* menu = static_cast<MenuItem*>(GetMenuData(reinterpret_cast<HMENU>(wp)));
            if(menu)
            {
                std::wcout<<menu->GetText()<<L"\n"; //Root Menu Closed..
            }
        }
    });

    this->AddListener(WM_UNINITMENUPOPUP, [&](UINT msg, WPARAM wp, LPARAM lp)
    {
        UNREFERENCED_PARAMETER(msg);
        if(!!!HIWORD(lp) && wp)
        {
            MenuItem* menu = static_cast<MenuItem*>(GetMenuData(reinterpret_cast<HMENU>(wp)));
            if(menu)
            {
                std::wcout<<menu->GetText()<<L"\n"; //Root Menu Closed..
            }
        }
    });

    this->AddListener(WM_MENUSELECT, [&](UINT msg, WPARAM wp, LPARAM lp)
    {
        UNREFERENCED_PARAMETER(msg);
        if(((HIWORD(wp) & MF_HILITE) || (HIWORD(wp) & MF_MOUSESELECT)) && LOWORD(wp))
        {
            MenuItem* menu = static_cast<MenuItem*>(GetMenuItemData(reinterpret_cast<HMENU>(lp), LOWORD(wp)));
            if(menu)
            {
                std::wcout<<menu->GetText();
            }
        }
    });

    this->AddListener(WM_CLOSE, [&](UINT msg, WPARAM wp, LPARAM lp)
    {
        UNREFERENCED_PARAMETER(msg);
        UNREFERENCED_PARAMETER(wp);
        UNREFERENCED_PARAMETER(lp);
        this->Dispose();
    });

    this->bottompanel->AddListener(WM_COMMAND, [&](UINT msg, WPARAM wp, LPARAM lp)
    {
        if(HIWORD(wp) == LBN_SELCHANGE)
        {
            if(reinterpret_cast<HWND>(lp) == this->debugprocessbox->GetHandle())
            {
                std::int32_t index = this->debugprocessbox->GetSelectedIndex();

                DebugEvent &info = std::get<1>(events[index]);

                switch(info.type)
                {
                    case DebugEventType::CREATE_PROCESS_EVENT:
                    {
                        std::wstringstream ss;
                        ss<<L"Created Process:\n\n";
                        ss<<L"Path: "<<info.path<<L"\n";
                        ss<<L"Handle: "<<static_cast<void*>(info.hProcess)<<L"\n";
                        ss<<L"Main: "<<info.lpBaseAddress;
                        this->processinfobox->SetText(ss.str().c_str());
                    }
                    break;

                    case DebugEventType::TERMINATE_PROCESS_EVENT:
                    {
                        std::wstring details = L"Process Terminated:\n\n";
                        details += L"Exit Code: " + std::to_wstring(info.code);
                        this->processinfobox->SetText(details.c_str());
                    }
                    break;

                    case DebugEventType::CREATE_THREAD_EVENT:
                    {
                        std::wstringstream ss;
                        ss<<L"Created Thread:\n\n";
                        ss<<L"Thread ID: "<<info.tid<<L"\n";
                        ss<<L"Handle: "<<static_cast<void*>(info.hThread)<<L"\n";
                        ss<<L"ThreadLocalBase: "<<info.lpLocalAddress<<L"\n";
                        ss<<L"Start Routine Address: "<<info.lpBaseAddress;
                        this->processinfobox->SetText(ss.str().c_str());
                    }
                    break;

                    case DebugEventType::TERMINATE_THREAD_EVENT:
                    {
                        std::wstringstream ss;
                        ss<<L"Thread Destroyed:\n\n";
                        ss<<L"Thread ID: "<<info.tid<<L"\n";
                        ss<<L"ExitCode: "<<std::to_wstring(info.code);
                        this->processinfobox->SetText(ss.str().c_str());
                    }
                    break;

                    case DebugEventType::LOAD_MODULE_EVENT:
                    {
                        std::wstringstream ss;
                        ss<<L"DLL Loaded:\n\n";
                        ss<<L"Path: "<<info.path<<L"\n";
                        ss<<L"Handle: "<<static_cast<void*>(info.hFile)<<L"\n";
                        ss<<L"Base Address: "<<info.lpBaseAddress;
                        this->processinfobox->SetText(ss.str().c_str());
                    }
                    break;

                    case DebugEventType::UNLOAD_MODULE_EVENT:
                    {
                        auto it = std::find_if(events.begin(), events.end(), [&](typename decltype(events)::value_type &it){
                            return (std::get<1>(it).type == DebugEventType::LOAD_MODULE_EVENT) && (std::get<1>(it).lpBaseAddress == info.lpBaseAddress);
                        });

                        std::wstring path = it != events.end() ? (std::get<1>(*it).path.empty() ? L"nullptr" : std::get<1>(*it).path) : L"nullptr";

                        std::wstringstream ss;
                        ss<<L"DLL Unloaded:\n\n";
                        ss<<L"Path: "<<path<<L"\n";
                        ss<<L"Handle: "<<static_cast<void*>(info.hFile)<<L"\n";
                        ss<<L"Base Address: "<<info.lpBaseAddress;
                        this->processinfobox->SetText(ss.str().c_str());
                    }
                    break;

                    case DebugEventType::DEBUG_STRING_EVENT:
                    {
                        std::wstring details = L"Debug String:\n\n";
                        details += L"Text: " + info.path;
                        this->processinfobox->SetText(details.c_str());
                    }
                    break;

                    case DebugEventType::BREAKPOINT_EVENT:
                    {
                        this->processinfobox->SetText(L"Breakpoint");
                        debugger.Step();
                    }
                    break;

                    case DebugEventType::SINGLE_STEP_EVENT:
                    {
                        this->processinfobox->SetText(L"Stepped");
                        debugger.Continue();
                    }
                    break;

                    case DebugEventType::EXCEPTION_EVENT:
                    {
                        std::wstring details = this->GetExceptionDetails(info);
                        this->processinfobox->SetText(details.c_str());
                    }
                    break;

                    default:
                    {
                        this->processinfobox->SetText(info.path.c_str());
                    }
                    break;
                }
            }
        }
    });
}

DebugWindow::~DebugWindow()
{
    for(Control* control : controls)
    {
        delete control;
    }

    for(Font* font : fonts)
    {
        delete font;
    }

    for(HBRUSH brush : brushes)
    {
        DeleteObject(brush);
    }

    debugger.Stop();
}

void DebugWindow::SetTextBarBackground(Label* textbar)
{
    textbar->AddListener(WM_CTLCOLORSTATIC, [&](UINT msg, WPARAM wp, LPARAM lp) -> LRESULT
    {
        HDC DC = reinterpret_cast<HDC>(wp);
        SetBkMode(DC, TRANSPARENT);
        return (LRESULT)GetStockObject(NULL_BRUSH);
    });

    textbar->AddListener(WM_PAINT, [=](UINT msg, WPARAM wp, LPARAM lp) -> LRESULT
    {
        PAINTSTRUCT psPaint;
        HDC hdc = BeginPaint(textbar->GetHandle(), &psPaint);

        HPEN originalPen = (HPEN)SelectObject(hdc, GetStockObject(DC_PEN));
        HBRUSH originalBrush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(219, 219, 219)));

        SetDCPenColor(hdc, RGB(185, 185, 185));
        Rectangle(hdc, 0, 0, textbar->GetWidth(), 20);

        DeleteObject(SelectObject(hdc, originalBrush));
        DeleteObject(SelectObject(hdc, originalPen));

        EndPaint(textbar->GetHandle(), &psPaint);

        return DefSubclassProc(textbar->GetHandle(), msg, wp, lp);
    });
}

void DebugWindow::InitD3D9(std::uint32_t width, std::uint32_t height)
{
    D3DPRESENT_PARAMETERS Parameters = {0};
    this->d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    D3DDISPLAYMODE dispMode = D3DDISPLAYMODE();
    d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispMode);

    Parameters.Windowed = true;
    Parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    Parameters.hDeviceWindow = *this;
    Parameters.BackBufferFormat = dispMode.Format; //D3DFMT_A8R8G8B8;
    Parameters.BackBufferWidth = width;
    Parameters.BackBufferHeight = height;
    Parameters.EnableAutoDepthStencil = TRUE;
    Parameters.AutoDepthStencilFormat = D3DFMT_D16;

    d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, *this, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &Parameters, &this->d3ddev9);
}

void DebugWindow::DestroyD3D9()
{
    if(this->d3ddev9) this->d3ddev9->Release();
    if(this->d3d9) this->d3d9->Release();
}

void DebugWindow::OnD3D9Draw()
{
    RECT vRect = {0};
    GetClientRect(*this, &vRect);
    vRect.left = this->leftpanel->GetLocation().x + this->leftpanel->GetWidth();
    vRect.right = vRect.right;
    vRect.top = this->leftpanel->GetLocation().y;
    vRect.bottom = this->bottompanel->GetLocation().y;

    d3ddev9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0xFF, 0, 0, 0), 1.0f, 0);
    d3ddev9->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xFF, 0, 0, 0), 1.0f, 0);


    /*d3ddev9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    d3ddev9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

    d3ddev9->BeginScene();
    Texture tex{d3ddev9, "C:/Users/Kira/Desktop/TeaPot.png"};

    LPD3DXSPRITE sprite;
    D3DXCreateSprite(d3ddev9, &sprite);

    sprite->Begin(D3DXSPRITE_ALPHABLEND);
    sprite->Draw(tex, NULL, NULL, NULL, 0xFFFFFFFF);
    sprite->End();
    sprite->Release();*/

    d3ddev9->EndScene();
    d3ddev9->Present(NULL, &vRect, NULL, NULL);
}

int DebugWindow::MessageLoop()
{
    RECT rect = {0};
    GetClientRect(*this, &rect);

    this->InitD3D9(900, 600);

    MSG Messages = {0};
    ShowWindow(*this, SW_SHOW);

    while(true)
    {
        while(PeekMessage(&Messages, nullptr, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&Messages);
            DispatchMessage(&Messages);
        }

        if(Messages.message == WM_QUIT) break;

        this->OnD3D9Draw();
    }

    this->DestroyD3D9();
    return Messages.wParam;
}

void DebugWindow::GetWindowCenter(int &centerX, int &centerY, int width, int height)
{
    RECT rect = {0};
    GetClientRect(GetDesktopWindow(), &rect);
    centerX = (rect.right / 2) - (width / 2);
    centerY = (rect.bottom /2) - (height / 2);
}

void DebugWindow::StartProcessDebugging()
{
    return;
    debugger.Start("C:/Users/Kira/jagexcache/jagexlauncher/bin/JagexLauncher.exe", "C:/Users/Kira/jagexcache/jagexlauncher/bin", "runescape", std::cout);

    debugger.AddListener(DebugEventType::CREATE_PROCESS_EVENT, [this](const DebugEvent &event){
        events.emplace_back(events.size(), event);

        std::wstring item = L"Process Created: ";
        item += event.path;
        this->debugprocessbox->AddItem(item.c_str());
        SendMessage(this->debugprocessbox->GetHandle(), LB_SETTOPINDEX, static_cast<WPARAM>(this->debugprocessbox->GetItemCount() - 1), 0);
    });

    debugger.AddListener(DebugEventType::TERMINATE_PROCESS_EVENT, [this](const DebugEvent &event){
        events.emplace_back(events.size(), event);

        std::wstring item = L"Process Terminated: ";
        item += std::to_wstring(event.code);
        this->debugprocessbox->AddItem(item.c_str());
        SendMessage(this->debugprocessbox->GetHandle(), LB_SETTOPINDEX, static_cast<WPARAM>(this->debugprocessbox->GetItemCount() - 1), 0);
    });

    debugger.AddListener(DebugEventType::CREATE_THREAD_EVENT, [this](const DebugEvent &event){
        events.emplace_back(events.size(), event);

        std::wstring item = L"Created Thread: ";
        item += std::to_wstring(event.tid);
        this->debugprocessbox->AddItem(item.c_str());
        SendMessage(this->debugprocessbox->GetHandle(), LB_SETTOPINDEX, static_cast<WPARAM>(this->debugprocessbox->GetItemCount() - 1), 0);
    });

    debugger.AddListener(DebugEventType::TERMINATE_THREAD_EVENT, [this](const DebugEvent &event){
        events.emplace_back(events.size(), event);

        std::wstring item = L"Terminated Thread: ";
        item += std::to_wstring(event.tid);
        this->debugprocessbox->AddItem(item.c_str());
        SendMessage(this->debugprocessbox->GetHandle(), LB_SETTOPINDEX, static_cast<WPARAM>(this->debugprocessbox->GetItemCount() - 1), 0);
    });

    debugger.AddListener(DebugEventType::LOAD_MODULE_EVENT, [this](const DebugEvent &event){
        events.emplace_back(events.size(), event);

        std::wstring item = L"DLL Loaded: ";
        item += event.path;
        this->debugprocessbox->AddItem(item.c_str());
        SendMessage(this->debugprocessbox->GetHandle(), LB_SETTOPINDEX, static_cast<WPARAM>(this->debugprocessbox->GetItemCount() - 1), 0);
    });

    debugger.AddListener(DebugEventType::UNLOAD_MODULE_EVENT, [this](const DebugEvent &event){
        events.emplace_back(events.size(), event);

        auto it = std::find_if(events.begin(), events.end(), [&](typename decltype(events)::value_type &it){
            return (std::get<1>(it).type == DebugEventType::LOAD_MODULE_EVENT) && (std::get<1>(it).lpBaseAddress == event.lpBaseAddress);
        });

        std::wstring item = L"DLL Unloaded: ";
        item += it != events.end() ? (std::get<1>(*it).path.empty() ? L"nullptr" : std::get<1>(*it).path) : L"nullptr";
        this->debugprocessbox->AddItem(item.c_str());
        SendMessage(this->debugprocessbox->GetHandle(), LB_SETTOPINDEX, static_cast<WPARAM>(this->debugprocessbox->GetItemCount() - 1), 0);
    });

    debugger.AddListener(DebugEventType::DEBUG_STRING_EVENT, [this](const DebugEvent &event){
        events.emplace_back(events.size(), event);

        std::wstring item = L"Debug String: ";
        item += event.path;
        this->debugprocessbox->AddItem(item.c_str());
        SendMessage(this->debugprocessbox->GetHandle(), LB_SETTOPINDEX, static_cast<WPARAM>(this->debugprocessbox->GetItemCount() - 1), 0);
    });

    debugger.AddListener(DebugEventType::EXCEPTION_EVENT, [this](const DebugEvent &event){
        if (this->parseExceptions)
        {
            events.emplace_back(events.size(), event);
            this->ProcessExceptions(event);
            SendMessage(this->debugprocessbox->GetHandle(), LB_SETTOPINDEX, static_cast<WPARAM>(this->debugprocessbox->GetItemCount() - 1), 0);
        }
    });

    debugger.AddListener(DebugEventType::BREAKPOINT_EVENT, [this](const DebugEvent &event){
        events.emplace_back(events.size(), event);
        this->ProcessRegisters(event);
        this->debugprocessbox->AddItem(L"Breakpoint Hit");
    });

    debugger.AddListener(DebugEventType::SINGLE_STEP_EVENT, [this](const DebugEvent &event){
        events.emplace_back(events.size(), event);
        this->debugprocessbox->AddItem(L"Single Step");

        this->registerinfobox->Clear();
        wchar_t buffer[512] = {0};
        swprintf(buffer, L"RAX = %08X", ctx.Rax);
        this->registerinfobox->AddItem(buffer);
        swprintf(buffer, L"RBX = %08X", ctx.Rbx);
        this->registerinfobox->AddItem(buffer);
        swprintf(buffer, L"RCX = %08X", ctx.Rcx);
        this->registerinfobox->AddItem(buffer);
        swprintf(buffer, L"RDX = %08X", ctx.Rdx);
        this->registerinfobox->AddItem(buffer);
        swprintf(buffer, L"RSI = %08X", ctx.Rsi);
        this->registerinfobox->AddItem(buffer);
        swprintf(buffer, L"RDI = %08X", ctx.Rdi);
        this->registerinfobox->AddItem(buffer);
        swprintf(buffer, L"RIP = %08X", ctx.Rip);
        this->registerinfobox->AddItem(buffer);
        swprintf(buffer, L"RSP = %08X", ctx.Rsp);
        this->registerinfobox->AddItem(buffer);
        swprintf(buffer, L"RBP = %08X", ctx.Rbp);
        this->registerinfobox->AddItem(buffer);
        swprintf(buffer, L"EFL = %08X", ctx.EFlags);
        this->registerinfobox->AddItem(buffer);
    });
}

void DebugWindow::ProcessRegisters(const DebugEvent &event)
{
    CONTEXT ctx = {0};
    ctx.ContextFlags = CONTEXT_ALL;
    HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, false, event.tid);
    if (hThread)
    {
        GetThreadContext(hThread, &ctx);
        CloseHandle(hThread);
    }
}

void DebugWindow::ProcessExceptions(const DebugEvent &event)
{
    switch(event.code)
    {
        case EXCEPTION_ACCESS_VIOLATION:
        {
            std::wstring item = L"Exception: ";
            item += L"Access Violation";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        {
            std::wstring item = L"Exception: ";
            item += L"Index Out Of Bounds";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_DATATYPE_MISALIGNMENT:
        {
            std::wstring item = L"Exception: ";
            item += L"Misaligned data type";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_FLT_DENORMAL_OPERAND:
        {
            std::wstring item = L"Exception: ";
            item += L"Denormal floating point operation";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        {
            std::wstring item = L"Exception: ";
            item += L"Floating Point divide by zero";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_FLT_INEXACT_RESULT:
        {
            std::wstring item = L"Exception: ";
            item += L"Floating Point Inexact result";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_FLT_INVALID_OPERATION:
        {
            std::wstring item = L"Exception: ";
            item += L"Invalid floating point operation";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_FLT_OVERFLOW:
        {
            std::wstring item = L"Exception: ";
            item += L"Floating point overflow";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_FLT_STACK_CHECK:
        {
            std::wstring item = L"Exception: ";
            item += L"Floating point stack overflow/underflow";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_FLT_UNDERFLOW:
        {
            std::wstring item = L"Exception: ";
            item += L"Floating point underflow";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_GUARD_PAGE:
        {
            std::wstring item = L"Exception: ";
            item += L"Access violation - Guarded page";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_ILLEGAL_INSTRUCTION:
        {
            std::wstring item = L"Exception: ";
            item += L"Illegal instruction";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_IN_PAGE_ERROR:
        {
            std::wstring item = L"Exception: ";
            item += L"Invalid page accessed";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        {
            std::wstring item = L"Exception: ";
            item += L"Integer divide by zero";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_INT_OVERFLOW:
        {
            std::wstring item = L"Exception: ";
            item += L"Integer overflow";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_INVALID_DISPOSITION:
        {
            std::wstring item = L"Exception: ";
            item += L"Invalid disposition";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_INVALID_HANDLE:
        {
            std::wstring item = L"Exception: ";
            item += L"Invalid handle";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        {
            std::wstring item = L"Exception: ";
            item += L"Fatal exception";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_PRIV_INSTRUCTION:
        {
            std::wstring item = L"Exception: ";
            item += L"Priv instruction";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case EXCEPTION_STACK_OVERFLOW:
        {
            std::wstring item = L"Exception: ";
            item += L"Stack overflow";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case STATUS_UNWIND_CONSOLIDATE:
        {
            std::wstring item = L"Exception: ";
            item += L"Stack unwind";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        case DBG_CONTROL_C:
        {
            std::wstring item = L"Exception: ";
            item += L"First Chance Exception";
            this->debugprocessbox->AddItem(item.c_str());
        }
        break;

        default:
        {
            std::wstringstream item;
            item<<L"Exception: ";
            item<<L"Unknown Exception - Code: ";
            item<<reinterpret_cast<void*>(event.code);
            this->debugprocessbox->AddItem(item.str().c_str());
        }
        break;
    }
}

std::wstring DebugWindow::GetExceptionDetails(const DebugEvent &event)
{
    std::wstringstream details;
    details<<L"An Exception Occurred:\n\n";

    switch(event.code)
    {
        case EXCEPTION_ACCESS_VIOLATION:
        {
            details<<L"Type: Access Violation\n";
            details<<L"Thread: "<<std::to_wstring(event.tid)<<L"\n";

            switch(event.flags)
            {
                case 0:
                {
                    details<<L"Reason: Attempted to read inaccessible data at: ";
                    details<<event.lpBaseAddress;
                }
                break;

                case 1:
                {
                    details<<L"Reason: Attempted to write to an inaccessible address at: ";
                    details<<event.lpBaseAddress;
                }
                break;

                case 8:
                {
                    details<<L"Reason: Thread caused a user-mode data execution prevention (DEP) violation at: ";
                    details<<event.lpBaseAddress;
                }
                break;
            }
        }
        break;

        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        {
            details<<L"Type: Index Out Of Bounds";
        }
        break;

        case EXCEPTION_DATATYPE_MISALIGNMENT:
        {
            details<<L"Type: Misaligned data type";
        }
        break;

        case EXCEPTION_FLT_DENORMAL_OPERAND:
        {
            details<<L"Type: Denormal floating point operation";
        }
        break;

        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        {
            details<<L"Type: Floating Point divide by zero";
        }
        break;

        case EXCEPTION_FLT_INEXACT_RESULT:
        {
            details<<L"Type: Floating Point Inexact result";
        }
        break;

        case EXCEPTION_FLT_INVALID_OPERATION:
        {
            details<<L"Type: Invalid floating point operation";
        }
        break;

        case EXCEPTION_FLT_OVERFLOW:
        {
            details<<L"Type: Floating point overflow";
        }
        break;

        case EXCEPTION_FLT_STACK_CHECK:
        {
            details<<L"Type: Floating point stack overflow/underflow";
        }
        break;

        case EXCEPTION_FLT_UNDERFLOW:
        {
            details<<L"Type: Floating point underflow";
        }
        break;

        case EXCEPTION_GUARD_PAGE:
        {
            details<<L"Type: Access violation - Guarded page";
        }
        break;

        case EXCEPTION_ILLEGAL_INSTRUCTION:
        {
            details<<L"Type: Illegal instruction";
        }
        break;

        case EXCEPTION_IN_PAGE_ERROR:
        {
            details<<L"Type: Invalid page accessed";
        }
        break;

        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        {
            details<<L"Type: Integer divide by zero";
        }
        break;

        case EXCEPTION_INT_OVERFLOW:
        {
            details<<L"Type: Integer overflow";
        }
        break;

        case EXCEPTION_INVALID_DISPOSITION:
        {
            details<<L"Type: Invalid disposition";
        }
        break;

        case EXCEPTION_INVALID_HANDLE:
        {
            details<<L"Type: Invalid handle";
        }
        break;

        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        {
            details<<L"Type: Fatal exception";
        }
        break;

        case EXCEPTION_PRIV_INSTRUCTION:
        {
            details<<L"Type: Priv instruction";
        }
        break;

        case EXCEPTION_STACK_OVERFLOW:
        {
            details<<L"Type: Stack overflow";
        }
        break;

        case STATUS_UNWIND_CONSOLIDATE:
        {
            details<<L"Type: Stack unwind";
        }
        break;

        case DBG_CONTROL_C:
        {
            details<<L"Type: First Chance Exception";
        }
        break;

        default:
        {
            details<<L"Type: Unknown Exception\n";
            details<<L"Code: "<<reinterpret_cast<void*>(event.code);
        }
        break;
    }

    return details.str();
}
