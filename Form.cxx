#include "Form.hxx"

void Form::RemoveListener(std::uint64_t Notification)
{
    Events.UnSubscribe(Notification);
}

int Form::MessageLoop()
{
    MSG Messages = {0};
    ShowWindow(WindowHandle, SW_SHOW);

    while(GetMessageW(&Messages, nullptr, 0, 0))
    {
        TranslateMessage(&Messages);
        DispatchMessageW(&Messages);
    }
    return Messages.wParam;
}

Form::Form(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, Form* Parent) : WindowHandle(nullptr)
{
    WNDCLASSEXW WndClass =
    {
        sizeof(WNDCLASSEXW), CS_DBLCLKS, WindowProcedure,
        0, 0, GetModuleHandleW(nullptr), LoadIconW(nullptr, reinterpret_cast<wchar_t*>(IDI_APPLICATION)),
        LoadCursorW(nullptr, reinterpret_cast<wchar_t*>(IDC_ARROW)), HBRUSH(COLOR_GRAYTEXT),
        nullptr, L"PRO_WIN_FORM", LoadIconW(nullptr, reinterpret_cast<wchar_t*>(IDI_APPLICATION))
    };

    RegisterClassExW(&WndClass);
    WindowHandle = CreateWindowExW(0, L"PRO_WIN_FORM", Title, WS_OVERLAPPEDWINDOW, Location.x, Location.y, Width, Height, Parent ? Parent->WindowHandle : nullptr, NULL, GetModuleHandleW(nullptr), this);
}

LRESULT __stdcall Form::HandleMessages(HWND Hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
        case WM_MOUSEMOVE:
        {
            if (!MouseTracking)
            {
                TrackMouse(Hwnd);
                MouseTracking = true;
            }
            Events.Notify(WM_MOUSEMOVE, Msg, wParam, lParam);
        }
        break;

        case WM_MOUSELEAVE:
        {
            MouseTracking = false;
            Events.Notify(WM_MOUSELEAVE, Msg, wParam, lParam);
        }
        break;

        case WM_MOUSEHOVER:
        {
            Events.Notify(WM_MOUSEHOVER, Msg, wParam, lParam);
        }
        break;

        case WM_CLOSE:
		{
		    if (!Events.Notify(WM_CLOSE, Msg, wParam, lParam))
            {
                return DefWindowProcW(Hwnd, Msg, wParam, lParam);
            }

            Events.NotifyAll(Msg, Msg, wParam, lParam);
		}
		break;

        case WM_DESTROY:
        {
            Events.Notify(WM_DESTROY, Msg, wParam, lParam);
            PostQuitMessage(0);
        }
        return 0;

        default:
        {
            if (Events.IsSubscribed(Msg) || Events.IsSubscribed(0))
            {
                return Events.NotifyAll(Msg, Msg, wParam, lParam);
            }
        }
        return DefWindowProcW(Hwnd, Msg, wParam, lParam);
    }
    return true;
}

LRESULT __stdcall Form::WindowProcedure(HWND Hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    Form* frm = nullptr;

    switch(Msg)
    {
        case WM_NCCREATE:
        {
            CREATESTRUCTW* ptr = reinterpret_cast<CREATESTRUCTW*>(lParam);
            frm = reinterpret_cast<Form*>(ptr->lpCreateParams);
            SetWindowLongPtrW(Hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(frm));
            frm->WindowHandle = Hwnd;
            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        return 0;

        default:
        {
            frm = reinterpret_cast<Form*>(GetWindowLongPtrW(Hwnd, GWLP_USERDATA));
            break;
        }
    }

    return frm ? frm->HandleMessages(Hwnd, Msg, wParam, lParam) : DefWindowProcW(Hwnd, Msg, wParam, lParam);
}
