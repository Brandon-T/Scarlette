#ifndef EVENTLISTENER_HPP_INCLUDED
#define EVENTLISTENER_HPP_INCLUDED

#include <functional>

class IEventListener
{
    public:
        virtual void notify(int64_t ID, HWND Hwnd, UINT Msg, WPARAM wParam, LPARAM lParam) = 0;
};

class MouseListener : IEventListener
{
    private:
        virtual void notify(int64_t ID, HWND Hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);

    public:
};

void MouseListener::notify(int64_t ID, HWND Hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(ID)
    {
    case WM_MOUSEMOVE:
        break;

        case WM_MOUSEFIRST
    }
}

#endif // EVENTLISTENER_HPP_INCLUDED
