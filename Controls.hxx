#ifndef CONTROLS_HXX_INCLUDED
#define CONTROLS_HXX_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <cwchar>
#include <richedit.h>
#include <cstring>
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

class Control
{
    private:
        std::uint32_t ID;
        HWND Handle, Parent;
        std::wstring Title, Class;
        POINT Location;
        int Width, Height;
        bool MouseTracking = false;
        void swap(Control &C);
        static LRESULT __stdcall SubProcedure(HWND Handle, UINT Msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

    public:
        Control();
        virtual ~Control();
        Control(Control&& C);
        Control& operator = (Control&& C);

        bool IsTypeOf(ControlStyle style) const;
        bool IsSameType(const Control &C) const;

        operator HWND() const;
        HWND GetHandle() const;
        HWND GetParent() const;
        std::uint32_t GetID() const;

        const wchar_t* GetText();
        std::uint16_t GetWidth() const;
        std::uint16_t GetHeight() const;
        POINT GetLocation() const;
        bool IsEnabled() const;
        bool IsVisible() const;

        void SetText(const wchar_t* text);
        void SetEnabled(bool Enabled);
        void SetVisibility(bool Visible);
        void SetBounds(RECT Bounds);
        void SetParent(HWND Parent);
        void SetLocation(POINT Location);
        void SetSize(std::uint16_t Width, std::uint16_t Height);
        void SetFont(HFONT hFont);

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

        void RemoveListener(std::uint64_t Notification);

    protected:
        EventManager<LRESULT(UINT, WPARAM, LPARAM)> Events;
        Control(const Control &C) = delete;
        Control& operator = (const Control &C) = delete;

        std::uint32_t CreateID();
        void Create(WindowStyle Style, WindowStyle ExtendedStyle, const wchar_t* Title, const wchar_t* Class, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent);
        void Dispose();

        virtual ControlStyle GetControlStyle() const = 0;

        virtual LRESULT __stdcall HandleMessages(HWND Handle, UINT Msg, WPARAM wParam, LPARAM lParam);
};




class Label : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        Label(Label&& L) : Control(std::move(L)) {}
        Label(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent);
        virtual ~Label() {}
};




class Button : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        Button(Button&& B) : Control(std::move(B)) {}
        Button(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent);
        virtual ~Button() {};
};



class CheckBox : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        CheckBox(CheckBox&& C) : Control(std::move(C)) {}
        CheckBox(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool Checked = false);
        virtual ~CheckBox() {};

        bool IsChecked();
        void SetChecked(bool Checked);
};



class RadioButton : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        RadioButton(RadioButton&& R) : Control(std::move(R)) {}
        RadioButton(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool Selected = false);
        virtual ~RadioButton() {};

        bool IsSelected();
        void SetSelected(bool Selected);
};



class GroupBox : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    private:
        static const std::uint32_t Sunken = 0x00000004;

    public:
        GroupBox(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool Sunken = true);
        GroupBox(GroupBox&& G) : Control(std::move(G)) {}
        virtual ~GroupBox() {}
};



class Panel : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    private:
        static const std::uint32_t Sunken = 0x00000004;

    public:
        Panel(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool Sunken = true);
        Panel(Panel&& P) : Control(std::move(P)) {}
        virtual ~Panel() {}
};



class TextBox : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        TextBox(TextBox&& T) : Control(std::move(T)) {}
        TextBox(const wchar_t* Text, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool MultiLine = false);
        virtual ~TextBox() {}

        void SetReadOnly(bool ReadOnly);
        void SetPassword(bool Enabled, wchar_t PasswordChar = L'*');
        std::uint32_t GetTextLength() const;
        void AppendText(const wchar_t* text) const;
        void ShowScrollBar(bool Show, std::uint32_t BarStyle = SB_VERT);
};



class ListBox : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    private:
        std::wstring Title;

    public:
        ListBox(ListBox&& L) : Control(std::move(L)), Title() {using std::swap; swap(L.Title, this->Title);}
        ListBox(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent);
        virtual ~ListBox() {}

        const wchar_t* GetText() = delete;
        std::uint32_t GetItemCount() const;
        std::int32_t GetSelectedIndex() const;
        void SetSelectedIndex(std::int32_t Index);
        void AddItem(const wchar_t* Item, std::int32_t Index = -1);
        void RemoveItem(std::uint32_t Index);
        void Clear();
        std::int32_t GetIndexOf(const wchar_t* Item);
        std::int32_t GetIndexPartial(const wchar_t* Item);
        void SetColumnWidth(std::uint16_t Width);
        const wchar_t* GetItem(std::uint32_t Index);
};



class ComboBox : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    private:
        std::wstring Title;

    public:
        enum class DropDownStyle {STYLE_SIMPLE, STYLE_DROPDOWN, STYLE_DROPDOWN_LIST};

        ComboBox(ComboBox&& C) : Control(std::move(C)), Title() {using std::swap; swap(C.Title, this->Title);}
        ComboBox(DropDownStyle Style, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent);
        virtual ~ComboBox() {}

        const wchar_t* GetText() = delete;
        std::uint32_t GetItemCount() const;
        std::int32_t GetSelectedIndex() const;
        void SetSelectedIndex(std::int32_t Index);
        void AddItem(const wchar_t* Item, std::int32_t Index = -1);
        void RemoveItem(std::uint32_t Index);
        void Clear();
        std::int32_t GetIndexOf(const wchar_t* Item);
        std::int32_t GetIndexPartial(const wchar_t* Item);
        void SetDropDownWidth(std::uint16_t Width);
        void SetDropDownStyle(DropDownStyle Style);
        const wchar_t* GetItem(std::uint32_t Index);
};



class PictureBox: public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        PictureBox(PictureBox&& P) : Control(std::move(P)) {}
        PictureBox(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent);
        virtual ~PictureBox() {}

        const wchar_t* GetText() = delete;
        void SetImage(HBITMAP Img);
};



class MenuItem;
class MenuBar : public Control
{
    private:
        friend MenuItem;
        HWND Parent;
        HMENU Handle;
        std::vector<Control*> Children;

    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        MenuBar(MenuBar&& M);
        MenuBar(HWND Parent) : Control(), Parent(Parent), Handle(CreateMenu()), Children() {SetMenuData(Handle, false, this);}
        virtual ~MenuBar() {if (!Parent) {DestroyMenu(Handle); Handle = nullptr;}}

        void Show() {SetMenu(this->Parent, Handle);};

        operator HWND() = delete;
        operator HMENU() {return this->Handle;}
        HWND GetParent() const {return this->Parent;}
        HMENU GetHandle() const {return this->Handle;}
};



class MenuItem : public Control
{
    private:
        friend MenuBar;
        HMENU Handle;
        Control* Parent;
        std::wstring Text;
        MenuStyle Style;
        std::vector<MenuItem*> Children;

    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        MenuItem(MenuItem&& M);
        MenuItem(MenuStyle Style, const wchar_t* Text);
        MenuItem(MenuStyle Style, const wchar_t* Text, HBITMAP Selected, HBITMAP DeSelected);
        virtual ~MenuItem() {if (!Parent) {DestroyMenu(Handle); Handle = nullptr;}}

        MenuItem(MenuBar* M, MenuStyle Style, const wchar_t* Text);
        MenuItem(MenuItem* M, MenuStyle Style, const wchar_t* Text);
        MenuItem(MenuBar* M, MenuStyle Style, const wchar_t* Text, HBITMAP Selected, HBITMAP DeSelected);
        MenuItem(MenuItem* M, MenuStyle Style, const wchar_t* Text, HBITMAP Selected, HBITMAP DeSelected);

        void AddItem(MenuItem* M);
        MenuItem* FindMenu(std::uint32_t ID);
        MenuItem* FindMenu(const wchar_t* MenuName);

        bool ToggleCheck();
        bool IsChecked() const;
        const wchar_t* GetText();
        void SetIcons(HBITMAP Selected, HBITMAP DeSelected);

        operator HWND() const = delete;
        operator HMENU() const {return this->Handle;}
};



class ToolBar: public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        ToolBar(ToolBar&& T) : Control(std::move(T)) {}
        ToolBar(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent);
        virtual ~ToolBar() {}

        const wchar_t* GetText() = delete;
        void AdjustSize();
};



class RichTextBox: public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    private:
        HMODULE module;
        static DWORD __stdcall EditStreamCallback(DWORD_PTR dwCookie, std::uint8_t* Buffer, long cb, long* pcb);

    public:
        RichTextBox(RichTextBox&& R) : Control(std::move(R)), module(nullptr) {using std::swap; swap(R.module, this->module);}
        RichTextBox(const wchar_t* Text, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool MultiLine = false);
        virtual ~RichTextBox();

        void SetReadOnly(bool ReadOnly);
        void SetPassword(bool Enabled, wchar_t PasswordChar = L'*');
        std::uint32_t GetTextLength() const;
        void AppendText(const wchar_t* Text) const;
        void ShowScrollBar(bool Show, std::uint32_t BarStyle = SB_VERT);

        bool FindTextInSelection(const wchar_t* Text);
        bool LoadFile(const wchar_t* FilePath);
};



class TabControl: public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    private:
        HIMAGELIST Icons;

    public:
        TabControl(TabStyle Style, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent);
        TabControl(TabControl&& T) : Control(std::move(T)), Icons(nullptr) {using std::swap; swap(T.Icons, this->Icons);}
        virtual ~TabControl() {ImageList_Destroy(Icons);}

        const wchar_t* GetText() = delete;

        void AddTab(const wchar_t* Text, HBITMAP Image = nullptr);
        void AddTab(const wchar_t* Text, HICON Image = nullptr);
        void RemoveTab(std::int32_t Index = -1);
};



class ReBar : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    private:
        REBARBANDINFOW band;

    public:
        ReBar(ReBar&& R) : Control(std::move(R)) {using std::swap; swap(R.band, this->band);}
        ReBar(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, ToolBar& Toolbar);
        virtual ~ReBar() {}

        const wchar_t* GetText() = delete;
        void AdjustSize();
        void AddBand();
        void AddBand(std::uint32_t Index);
        void RemoveBand(std::uint32_t Index = 0);
};




class StatusBar: public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    private:
        std::wstring Text;

    public:
        StatusBar(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool GripBar = false);
        StatusBar(StatusBar&& S) : Control(std::move(S)), Text() {using std::swap; swap(S.Text, this->Text);}
        virtual ~StatusBar() {}

        const wchar_t* GetText() = delete;

        const wchar_t* GetText(std::size_t Index = 0);
        void SetText(const wchar_t* Text, std::size_t Index = 0);
        void AdjustSize();

        template<std::size_t size>
        void SetParts(int (&Parts)[size]) {SendMessageW(*this, SB_SETPARTS, sizeof(Parts)/sizeof(decltype(Parts[0])), reinterpret_cast<LPARAM>(Parts));}
};



class DatePicker: public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        DatePicker(DatePicker&& D) : Control(std::move(D)) {}
        DatePicker(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent);
        virtual ~DatePicker() {}

        const wchar_t* GetText() = delete;
};



class Calendar: public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    public:
        Calendar(Calendar&& C) : Control(std::move(C)) {}
        Calendar(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent);
        virtual ~Calendar() {}

        const wchar_t* GetText() = delete;
};



class SysLink : public Control
{
    protected:
        virtual ControlStyle GetControlStyle() const;

    //Meh..
};

#ifdef _UNICODE_SET
#undef _UNICODE
#endif

#ifdef UNICODE_SET
#undef UNICODE
#endif

#endif // CONTROLS_HXX_INCLUDED
