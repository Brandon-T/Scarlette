#ifndef FORM_HXX_INCLUDED
#define FORM_HXX_INCLUDED

#include <windows.h>
#include <functional>
#include <vector>
#include "EventManager.hxx"
#include "Utilities.hxx"

#ifndef _UNICODE
#define _UNICODE
#define _UNICODE_SET
#endif

#ifndef UNICODE
#define UNICODE
#define UNICODE_SET
#endif

class Form
{
    public:
        Form(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, Form* Parent = nullptr);
        ~Form() {}

        Form(Form&& form) = delete;
        Form(const Form& form) = delete;
        Form& operator = (const Form& form) = delete;

        template<typename T, typename = typename std::enable_if<std::is_void<typename std::result_of<T(UINT, WPARAM, LPARAM)>::type>::value>::type>
        std::uint64_t AddListener(std::uint64_t Notification, T&& listener)
        {
            return Events.Subscribe(Notification, [=](UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
                listener(msg, wp, lp);
                return true;
            });
        }

        template<typename T, typename = typename std::enable_if<!std::is_void<typename std::result_of<T(UINT, WPARAM, LPARAM)>::type>::value>::type, typename = void>
        std::uint64_t AddListener(std::uint64_t Notification, T&& listener)
        {
            return Events.Subscribe(Notification, std::forward<std::function<LRESULT(UINT, WPARAM, LPARAM)>>(listener));
        }

        void RemoveListener(std::uint64_t ID);

        int MessageLoop();
        void Close() {PostMessage(WindowHandle, WM_CLOSE, 0, 0);}
        void Dispose() {DestroyWindow(WindowHandle);}
        operator HWND() { return WindowHandle; }

    private:
        bool MouseTracking = false;
        HWND WindowHandle = nullptr;
        EventManager<LRESULT(UINT, WPARAM, LPARAM)> Events;

        LRESULT __stdcall HandleMessages(HWND Hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
        static LRESULT __stdcall WindowProcedure(HWND Hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
};

#ifdef _UNICODE_SET
#undef _UNICODE
#endif

#ifdef UNICODE_SET
#undef UNICODE
#endif

#endif // FORM_HXX_INCLUDED
