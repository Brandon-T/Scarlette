#ifndef UTILITIES_HXX_INCLUDED
#define UTILITIES_HXX_INCLUDED

#include <windows.h>
#include <winternl.h>
#include <cstdint>
#include <commctrl.h>
#include <string>

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) {(P) = (P);}
#endif // UNREFERENCED_PARAMETER

enum class ControlStyle : std::uint32_t
{
    CTRL_NONE,
    CTRL_LABEL,
    CTRL_BUTTON,
    CTRL_CHECK_BOX,
    CTRL_RADIO_BUTTON,
    CTRL_GROUP_BOX,
    CTRL_PANEL,
    CTRL_TEXT_BOX,
    CTRL_LIST_BOX,
    CTRL_COMBO_BOX,
    CTRL_PICTURE_BOX,
    CTRL_MENU_BAR,
    CTRL_MENU_ITEM,
    CTRL_TOOL_BAR,
    CTRL_RICH_EDIT,
    CTRL_TAB_PANEL,
    CTRL_REBAR,
    CTRL_STATUS_BAR,
    CTRL_DATE_PICKER,
    CTRL_CALENDAR,
    CTRL_SYS_LINK
};



enum class TabStyle : std::uint32_t
{
    TS_NONE             = 0L,
    TS_BUTTONS          = TCS_BUTTONS,
    TS_FOCUS            = TCS_FOCUSONBUTTONDOWN,
    TS_NOFOCUS          = TCS_FOCUSNEVER,
    TS_MULTILINE        = TCS_MULTILINE,
    TS_RIGHTJUSTIFY     = TCS_RIGHTJUSTIFY,
    TS_FIXEDWIDTH       = TCS_FIXEDWIDTH,
    TS_FORCEICONLEFT    = TCS_FORCEICONLEFT,
    TS_FORCELABELLEFT   = TCS_FORCELABELLEFT,
    TS_TOOLTIPS         = TCS_TOOLTIPS
};

enum class MenuStyle : std::uint32_t
{
    MS_BITMAP       = MF_BITMAP,
    MS_CHECKED      = MF_CHECKED,
    MS_UNCHECKED    = MF_UNCHECKED,
    MS_DISABLED     = MF_DISABLED,
    MS_ENABLED      = MF_ENABLED,
    MS_GRAYED       = MF_GRAYED,
    MS_MENUBARBREAK = MF_MENUBARBREAK,
    MS_MENUBREAK    = MF_MENUBREAK,
    MS_POPUP        = MF_POPUP,
    MS_SEPARATOR    = MF_SEPARATOR,
    MS_STRING       = MF_STRING,
};

enum class WindowStyle : std::uint32_t
{
    NONE                                    = 0L,
    BORDER                                  = WS_BORDER,
    CHILD                                   = WS_CHILD,
    CLIPCHILDREN                            = WS_CLIPCHILDREN,
    CLIPSIBLINGS                            = WS_CLIPSIBLINGS,
    GROUP                                   = WS_GROUP,
    CAPTION                                 = WS_CAPTION,
    DISABLED                                = WS_DISABLED,
    DLGFRAME                                = WS_DLGFRAME,
    HSCROLL                                 = WS_HSCROLL,
    VSCROLL                                 = WS_VSCROLL,
    MINIMIZED                               = WS_MINIMIZE,
    MAXIMIZED                               = WS_MAXIMIZE,
    MAXIMIZE_BOX                            = WS_MAXIMIZEBOX,
    MINIMIZE_BOX                            = WS_MINIMIZEBOX,
    OVERLAPPED                              = WS_OVERLAPPED,
    OVERLAPPED_WINDOW                       = WS_OVERLAPPEDWINDOW,
    POPUP                                   = WS_POPUP,
    POPUP_WINDOW                            = WS_POPUPWINDOW,
    THICK_FRAME                             = WS_THICKFRAME,
    SYSMENU                                 = WS_SYSMENU,
    TABSTOP                                 = WS_TABSTOP,

    EX_ACCEPTFILES                          = WS_EX_ACCEPTFILES,
    EX_APPWINDOW                            = WS_EX_APPWINDOW,
    EX_CLIENTEDGE                           = WS_EX_CLIENTEDGE,
    EX_COMPOSITED                           = WS_EX_COMPOSITED,
    EX_CONTEXTHELP                          = WS_EX_CONTEXTHELP,
    EX_CONTROLPARENT                        = WS_EX_CONTROLPARENT,
    EX_DLGMODALFRAME                        = WS_EX_DLGMODALFRAME,
    EX_LAYERED                              = WS_EX_LAYERED,
    EX_LAYOUTRTL                            = WS_EX_LAYOUTRTL,
    EX_LEFT                                 = WS_EX_LEFT,
    EX_LEFTSCROLLBAR                        = WS_EX_LEFTSCROLLBAR,
    EX_LTRREADING                           = WS_EX_LTRREADING,
    EX_MDICHILD                             = WS_EX_MDICHILD,
    EX_NOACTIVATE                           = WS_EX_NOACTIVATE,
    EX_NOINHERITLAYOUT                      = WS_EX_NOINHERITLAYOUT,
    EX_NOPARENTNOTIFY                       = WS_EX_NOPARENTNOTIFY,
    EX_OVERLAPPEDWINDOW                     = WS_EX_OVERLAPPEDWINDOW,
    EX_PALETTEWINDOW                        = WS_EX_PALETTEWINDOW,
    EX_RIGHT                                = WS_EX_RIGHT,
    EX_RIGHTSCROLLBAR                       = WS_EX_RIGHTSCROLLBAR,
    EX_RTLREADING                           = WS_EX_RTLREADING,
    EX_STATICEDGE                           = WS_EX_STATICEDGE,
    EX_TOOLWINDOW                           = WS_EX_TOOLWINDOW,
    EX_TOPMOST                              = WS_EX_TOPMOST,
    EX_TRANSPARENT                          = WS_EX_TRANSPARENT,
    EX_WINDOWEDGE                           = WS_EX_WINDOWEDGE
};

void TrackMouse(HWND hwnd);
void LockSystemMenu(HWND hwnd, bool lock);
void ClearConsoleRow(int Row);
void SetWindowTransparency(HWND hwnd, std::uint8_t Transperancy);
void EnableVisualStyles();
void SetWindowStyle(HWND hwnd, WindowStyle style, bool remove_style = false);
void SetExtendedWindowStyle(HWND hwnd, WindowStyle style, bool remove_style = false);
std::wstring GetErrorMessage(std::uint32_t Error);

void* GetMenuData(HMENU menu);
void* GetMenuItemData(HMENU menu, UINT id);
void SetMenuData(HMENU menu, bool notify_by_command, void* self_ptr = nullptr);
void SetMenuItemData(HMENU menu, HMENU sub_menu, int id, void* self_ptr = nullptr);
bool GetFileName(HANDLE handle, wchar_t* path);


inline TabStyle operator ~ (TabStyle a){return static_cast<TabStyle>(~static_cast<std::uint32_t>(a));}
inline TabStyle operator | (TabStyle a, TabStyle b) {return static_cast<TabStyle>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));}
inline TabStyle operator & (TabStyle a, TabStyle b) {return static_cast<TabStyle>(static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));}

inline MenuStyle operator ~ (MenuStyle a){return static_cast<MenuStyle>(~static_cast<std::uint32_t>(a));}
inline MenuStyle operator | (MenuStyle a, MenuStyle b) {return static_cast<MenuStyle>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));}
inline MenuStyle operator & (MenuStyle a, MenuStyle b) {return static_cast<MenuStyle>(static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));}

inline WindowStyle operator ~ (WindowStyle a){return static_cast<WindowStyle>(~static_cast<std::uint32_t>(a));}
inline WindowStyle operator | (WindowStyle a, WindowStyle b) {return static_cast<WindowStyle>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));}
inline WindowStyle operator & (WindowStyle a, WindowStyle b) {return static_cast<WindowStyle>(static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));}

#endif // UTILITIES_HXX_INCLUDED
