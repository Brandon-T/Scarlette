#include "Controls.hxx"

void Control::swap(Control &C)
{
    using std::swap;
    swap(Handle, C.Handle);
    swap(Parent, C.Parent);
    swap(Location.x, C.Location.x);
    swap(Location.y, C.Location.y);
    swap(Width, C.Width);
    swap(Height, C.Height);
}

void Control::RemoveListener(std::uint64_t Notification)
{
    Events.UnSubscribe(Notification);
}

Control::Control() : ID(0), Handle(nullptr), Parent(nullptr), Title(), Class(L"CONTROL"), Location(), Width(0), Height(0) {}
Control::~Control() {DestroyWindow(Handle); Handle = nullptr;}

Control::Control(Control&& C) : ID(0), Handle(nullptr), Parent(nullptr), Title(), Class(L"CONTROL"), Location(), Width(0), Height(0) {C.swap(*this);}

Control& Control::operator = (Control&& C) {C.swap(*this); return *this;}

bool Control::IsTypeOf(ControlStyle style) const
{
    return this->GetControlStyle() == style;
}

bool Control::IsSameType(const Control &C) const
{
    return this->GetControlStyle() == C.GetControlStyle();
}

Control::operator HWND() const {return Handle;}

HWND Control::GetHandle() const {return Handle;}

HWND Control::GetParent() const {return Parent;}

std::uint32_t Control::GetID() const {return ID;}

const wchar_t* Control::GetText()
{
    this->Title = std::wstring(GetWindowTextLengthW(this->Handle) + 1, L'\0');
    GetWindowTextW(this->Handle, &Title[0], Title.size());
    return Title.c_str();
}

std::uint16_t Control::GetWidth() const {return Width;}

std::uint16_t Control::GetHeight() const {return Height;}

POINT Control::GetLocation() const {return Location;}

bool Control::IsEnabled() const {return IsWindowEnabled(Handle);}

bool Control::IsVisible() const {return IsWindowVisible(Handle);}

void Control::SetText(const wchar_t* text)
{
    this->Title = text;
    SetWindowTextW(Handle, text);
}

void Control::SetEnabled(bool Enabled) {EnableWindow(Handle, Enabled);}

void Control::SetVisibility(bool Visible) {ShowWindow(Handle, Visible ? SW_SHOW : SW_HIDE);}

void Control::SetBounds(RECT Bounds) {MoveWindow(Handle, Bounds.left, Bounds.top, Bounds.right - Bounds.left, Bounds.bottom - Bounds.top, false);}

void Control::SetParent(HWND Parent) {this->Parent = Parent; ::SetParent(Handle, Parent); ShowWindow(Handle, SW_SHOW);}

void Control::SetLocation(POINT Location) {this->Location = Location; SetWindowPos(Handle, HWND_TOP, Location.x, Location.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);}

void Control::SetSize(std::uint16_t Width, std::uint16_t Height) {this->Width = Width; this->Height = Height; SetWindowPos(Handle, HWND_TOP, 0, 0, Width, Height, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);}

void Control::SetFont(HFONT hFont) {SendMessage(Handle, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), true);}

std::uint32_t Control::CreateID()
{
    static std::uint32_t IDs = 0;
    this->ID = ++IDs;
    return this->ID;
}

void Control::Create(WindowStyle Style, WindowStyle ExtendedStyle, const wchar_t* Title, const wchar_t* Class, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent)
{
    CreateID();
    this->Title = Title ? Title : L"";
    this->Class = Class;
    this->Location = Location;
    this->Width = Width;
    this->Height = Height;
    this->Parent = Parent;

    this->Handle = CreateWindowExW(static_cast<std::uint32_t>(ExtendedStyle), Class, Title, static_cast<std::uint32_t>(Style), Location.x, Location.y, Width, Height, Parent, reinterpret_cast<HMENU>(ID), nullptr, nullptr);
    SetWindowSubclass(this->Handle, SubProcedure, static_cast<UINT_PTR>(this->ID), reinterpret_cast<DWORD_PTR>(this));
}

LRESULT __stdcall Control::HandleMessages(HWND Handle, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
        case WM_MOUSEMOVE:
        {
            if (!MouseTracking)
            {
                TrackMouse(Handle);
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

        default:
        {
            if (Events.IsSubscribed(Msg) || Events.IsSubscribed(0))
            {
                return Events.NotifyAll(Msg, Msg, wParam, lParam);
            }
            return DefSubclassProc(Handle, Msg, wParam, lParam);
        }
    }

    return true;
}

LRESULT __stdcall Control::SubProcedure(HWND Handle, UINT Msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    Control* ctrl = nullptr;

    switch(Msg)
    {
        case WM_NCCREATE:
        {
            ctrl = reinterpret_cast<Control*>(dwRefData);
            ctrl->Handle = Handle;
            break;
        }

        case WM_NCDESTROY:
        {
            RemoveWindowSubclass(Handle, SubProcedure, uIdSubclass);
            return DefWindowProcW(Handle, Msg, wParam, lParam);
        }
        break;

        default:
        {
            ctrl = reinterpret_cast<Control*>(dwRefData);
            break;
        }
    }

    return ctrl ? ctrl->HandleMessages(Handle, Msg, wParam, lParam) : DefSubclassProc(Handle, Msg, wParam, lParam);
}



ControlStyle Label::GetControlStyle() const
{
    return ControlStyle::CTRL_LABEL;
}

Label::Label(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent) : Control()
{
    WindowStyle Style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE);
    Create(Style, WindowStyle::NONE, Title, L"STATIC", Location, Width, Height, Parent);
}



ControlStyle Button::GetControlStyle() const
{
    return ControlStyle::CTRL_BUTTON;
}

Button::Button(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent) : Control()
{
    WindowStyle Style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON);
    Create(Style, WindowStyle::NONE, Title, L"BUTTON", Location, Width, Height, Parent);
}


ControlStyle CheckBox::GetControlStyle() const
{
    return ControlStyle::CTRL_CHECK_BOX;
}

CheckBox::CheckBox(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool Checked) : Control()
{
    WindowStyle Style = static_cast<WindowStyle>(WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE);
    Create(Style, WindowStyle::NONE, Title, L"BUTTON", Location, Width, Height, Parent);
    if (Checked)
    {
        CheckDlgButton(this->GetParent(), this->GetID(), BST_CHECKED);
    }
}

bool CheckBox::IsChecked()
{
    return IsDlgButtonChecked(this->GetParent(), this->GetID());
}

void CheckBox::SetChecked(bool Checked)
{
    CheckDlgButton(this->GetParent(), this->GetID(), Checked ? BST_CHECKED : BST_UNCHECKED);
}



ControlStyle GroupBox::GetControlStyle() const
{
    return ControlStyle::CTRL_GROUP_BOX;
}

GroupBox::GroupBox(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool Sunken) : Control()
{
    WindowStyle Style = static_cast<WindowStyle>(WS_CHILD | BS_GROUPBOX | WS_GROUP | WS_VISIBLE | (Sunken ? this->Sunken : 0));
    Create(Style, WindowStyle::NONE, Title, L"BUTTON", Location, Width, Height, Parent);
}



ControlStyle Panel::GetControlStyle() const
{
    return ControlStyle::CTRL_PANEL;
}

Panel::Panel(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool Sunken) : Control()
{
    WindowStyle Style = static_cast<WindowStyle>(WS_CHILD | BS_GROUPBOX | WS_GROUP | WS_VISIBLE | (Sunken ? this->Sunken : 0));
    Create(Style, WindowStyle::NONE, Title, L"STATIC", Location, Width, Height, Parent);
}




ControlStyle RadioButton::GetControlStyle() const
{
    return ControlStyle::CTRL_RADIO_BUTTON;
}

RadioButton::RadioButton(const wchar_t* Title, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool Selected) : Control()
{
    WindowStyle Style = static_cast<WindowStyle>(WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE);
    Create(Style, WindowStyle::NONE, Title, L"BUTTON", Location, Width, Height, Parent);
    if (Selected)
    {
        CheckDlgButton(this->GetParent(), this->GetID(), BST_CHECKED);
    }
}

bool RadioButton::IsSelected()
{
    return IsDlgButtonChecked(this->GetParent(), this->GetID());
}

void RadioButton::SetSelected(bool Selected)
{
    CheckDlgButton(this->GetParent(), this->GetID(), Selected ? BST_CHECKED : BST_UNCHECKED);
}


ControlStyle TextBox::GetControlStyle() const
{
    return ControlStyle::CTRL_TEXT_BOX;
}

TextBox::TextBox(const wchar_t* Text, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool MultiLine) : Control()
{
    WindowStyle Style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE | (MultiLine ? ES_MULTILINE | ES_AUTOVSCROLL : 0));
    Create(Style, WindowStyle::EX_STATICEDGE, Text, L"EDIT", Location, Width, Height, Parent);
}


void TextBox::SetReadOnly(bool ReadOnly)
{
    SendMessageW(*this, EM_SETREADONLY, ReadOnly, 0);
}

void TextBox::SetPassword(bool Enabled, wchar_t PasswordChar)
{
    SendMessageW(*this, EM_SETPASSWORDCHAR, Enabled ? PasswordChar : 0, 0);
}

std::uint32_t TextBox::GetTextLength() const
{
    return GetWindowTextLength(*this);
}

void TextBox::AppendText(const wchar_t* text) const
{
    SendMessageW(*this, EM_SETSEL, -1, -1);
    SendMessageW(*this, EM_REPLACESEL, 0, reinterpret_cast<LPARAM>(text));
}

void TextBox::ShowScrollBar(bool Show, std::uint32_t BarStyle)
{
    ::ShowScrollBar(*this, BarStyle, Show);
}



ControlStyle ListBox::GetControlStyle() const
{
    return ControlStyle::CTRL_LIST_BOX;
}

ListBox::ListBox(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent) : Control(), Title()
{
    WindowStyle Style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE | LBS_NOTIFY);
    Create(Style, WindowStyle::EX_CLIENTEDGE, nullptr, L"LISTBOX", Location, Width, Height, Parent);
}

std::uint32_t ListBox::GetItemCount() const
{
    return SendMessageW(*this, LB_GETCOUNT, 0, 0);
}

std::int32_t ListBox::GetSelectedIndex() const
{
    return SendMessageW(*this, LB_GETCURSEL, 0, 0);
}

void ListBox::SetSelectedIndex(std::int32_t Index)
{
    SendMessageW(*this, LB_SETCURSEL, Index, 0);
}

void ListBox::AddItem(const wchar_t* Item, std::int32_t Index)
{
    SendMessageW(*this, Index == -1 ? LB_ADDSTRING : LB_INSERTSTRING, Index, reinterpret_cast<LPARAM>(Item));
}

void ListBox::RemoveItem(std::uint32_t Index)
{
    SendMessageW(*this, LB_DELETESTRING, Index, 0);
}

void ListBox::Clear()
{
    SendMessageW(*this, LB_RESETCONTENT, 0, 0);
}

std::int32_t ListBox::GetIndexOf(const wchar_t* Item)
{
    return SendMessageW(*this, LB_FINDSTRINGEXACT, -1, reinterpret_cast<LPARAM>(Item));
}

std::int32_t ListBox::GetIndexPartial(const wchar_t* Item)
{
    return SendMessageW(*this, LB_FINDSTRING, -1, reinterpret_cast<LPARAM>(Item));
}

void ListBox::SetColumnWidth(std::uint16_t Width)
{
    SendMessageW(*this, LB_SETCOLUMNWIDTH, Width, 0);
}

const wchar_t* ListBox::GetItem(std::uint32_t Index)
{
    this->Title = std::wstring(SendMessage(*this, LB_GETTEXTLEN, Index, 0) + 1, L'\0');
    SendMessageW(*this, LB_GETTEXT, Index, reinterpret_cast<LPARAM>(&Title[0]));
    return this->Title.c_str();
}




ControlStyle ComboBox::GetControlStyle() const
{
    return ControlStyle::CTRL_COMBO_BOX;
}

ComboBox::ComboBox(DropDownStyle Style, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent) : Control(), Title()
{
    WindowStyle style = static_cast<WindowStyle>((Style == DropDownStyle::STYLE_SIMPLE ? CBS_SIMPLE : Style == DropDownStyle::STYLE_DROPDOWN ? CBS_DROPDOWN : CBS_DROPDOWNLIST) | WS_CHILD | WS_VISIBLE);
    Create(style, WindowStyle::EX_STATICEDGE, nullptr, L"COMBOBOX", Location, Width, Height, Parent);
}

std::uint32_t ComboBox::GetItemCount() const
{
    return SendMessageW(*this, CB_GETCOUNT, 0, 0);
}

std::int32_t ComboBox::GetSelectedIndex() const
{
    return SendMessageW(*this, CB_GETCURSEL, 0, 0);
}

void ComboBox::SetSelectedIndex(std::int32_t Index)
{
    SendMessageW(*this, CB_SETCURSEL, Index, 0);
}

void ComboBox::AddItem(const wchar_t* Item, std::int32_t Index)
{
    SendMessageW(*this, Index == -1 ? CB_ADDSTRING : CB_INSERTSTRING, Index, reinterpret_cast<LPARAM>(Item));
}

void ComboBox::RemoveItem(std::uint32_t Index)
{
    SendMessageW(*this, CB_DELETESTRING, Index, 0);
}

void ComboBox::Clear()
{
    SendMessageW(*this, CB_RESETCONTENT, 0, 0);
}

std::int32_t ComboBox::GetIndexOf(const wchar_t* Item)
{
    return SendMessageW(*this, CB_FINDSTRINGEXACT, -1, reinterpret_cast<LPARAM>(Item));
}

std::int32_t ComboBox::GetIndexPartial(const wchar_t* Item)
{
    return SendMessageW(*this, CB_FINDSTRING, -1, reinterpret_cast<LPARAM>(Item));
}

void ComboBox::SetDropDownWidth(std::uint16_t Width)
{
    SendMessageW(*this, CB_SETDROPPEDWIDTH, Width, 0);
}

void ComboBox::SetDropDownStyle(DropDownStyle Style)
{
    SetWindowStyle(*this, static_cast<WindowStyle>(CBS_SIMPLE | CBS_DROPDOWN | CBS_DROPDOWNLIST), true);
    WindowStyle style = static_cast<WindowStyle>(Style == DropDownStyle::STYLE_SIMPLE ? CBS_SIMPLE : Style == DropDownStyle::STYLE_DROPDOWN ? CBS_DROPDOWN : CBS_DROPDOWNLIST);
    SetWindowStyle(*this, style, false);
}

const wchar_t* ComboBox::GetItem(std::uint32_t Index)
{
    this->Title = std::wstring(SendMessageW(*this, CB_GETLBTEXTLEN, Index, 0) + 1, L'\0');
    SendMessageW(*this, CB_GETLBTEXT, Index, reinterpret_cast<LPARAM>(&Title[0]));
    return Title.c_str();
}



ControlStyle PictureBox::GetControlStyle() const
{
    return ControlStyle::CTRL_PICTURE_BOX;
}

PictureBox::PictureBox(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent) : Control()
{
    WindowStyle style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_BITMAP);
    Create(style, WindowStyle::EX_STATICEDGE, nullptr, L"STATIC", Location, Width, Height, Parent);
}

void PictureBox::SetImage(HBITMAP Img)
{
    SendMessageW(*this, STM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(Img));
}



ControlStyle MenuBar::GetControlStyle() const
{
    return ControlStyle::CTRL_MENU_BAR;
}

MenuBar::MenuBar(MenuBar&& M) : Control(std::move(M)), Parent(nullptr), Handle(nullptr), Children()
{
    using std::swap;
    swap(M.Parent, this->Parent);
    swap(M.Handle, this->Handle);
    swap(M.Children, this->Children);

    for (auto &it : this->Children)
    {
        static_cast<MenuItem*>(it)->Parent = this;
    }
}



ControlStyle MenuItem::GetControlStyle() const
{
    return ControlStyle::CTRL_MENU_ITEM;
}

MenuItem::MenuItem(MenuItem&& M) : Control(std::move(M)), Handle(nullptr), Parent(nullptr), Children()
{
    using std::swap;
    swap(M.Handle, this->Handle);
    swap(M.Parent, this->Parent);
    swap(M.Text, this->Text);
    swap(M.Style, this->Style);
    swap(M.Children, this->Children);

    for (auto &it : Children)
    {
        it->Parent = this;
    }
}

MenuItem::MenuItem(MenuStyle Style, const wchar_t* Text) : Handle(CreateMenu()), Parent(nullptr), Text(Text), Style(Style), Children()
{
    CreateID();
}

MenuItem::MenuItem(MenuStyle Style, const wchar_t* Text, HBITMAP Selected, HBITMAP DeSelected) : MenuItem(Style, Text)
{
    SetIcons(Selected, DeSelected);
}

MenuItem::MenuItem(MenuBar* M, MenuStyle Style, const wchar_t* Text) : MenuItem(Style | MenuStyle::MS_POPUP, Text)
{
    this->Parent = M;
    M->Children.emplace_back(this);
    SetMenuData(Handle, false, this);
    AppendMenuW(M->GetHandle(), static_cast<UINT>(this->Style), reinterpret_cast<UINT_PTR>(Handle), &Text[0]);
    SetMenuItemData(M->GetHandle(), this->Handle, this->GetID(), this);
}

MenuItem::MenuItem(MenuItem* M, MenuStyle Style, const wchar_t* Text) : MenuItem(Style, Text)
{
    M->AddItem(this);
}

MenuItem::MenuItem(MenuBar* M, MenuStyle Style, const wchar_t* Text, HBITMAP Selected, HBITMAP DeSelected) : MenuItem(M, Style, Text)
{
    SetIcons(Selected, DeSelected);
}

MenuItem::MenuItem(MenuItem* M, MenuStyle Style, const wchar_t* Text, HBITMAP Selected, HBITMAP DeSelected) : MenuItem(M, Style, Text)
{
    SetIcons(Selected, DeSelected);
}

void MenuItem::AddItem(MenuItem* child)
{
    child->Parent = this;
    Children.emplace_back(child);
    SetMenuData(child->Handle, false, child);
    AppendMenuW(Handle, static_cast<UINT>(child->Style), reinterpret_cast<UINT_PTR>(child->Handle), child->Text.c_str());
    SetMenuItemData(Handle, child->Handle, child->GetID(), child);
}

MenuItem* MenuItem::FindMenu(std::uint32_t ID)
{
    for (auto &it : Children)
    {
        if (it->GetID() == ID)
        {
            return it;
        }
    }
    return nullptr;
}

MenuItem* MenuItem::FindMenu(const wchar_t* MenuName)
{
    for (auto &it : Children)
    {
        if (it->Parent == this)
        {
            const wchar_t* text = it->GetText();
            if (text && !wcscmp(MenuName, text))
            {
                return it;
            }
        }
    }
    return nullptr;
}

void MenuItem::SetIcons(HBITMAP Selected, HBITMAP DeSelected)
{
    HMENU parent = this->Parent->IsTypeOf(ControlStyle::CTRL_MENU_ITEM) ? static_cast<MenuItem*>(this->Parent)->Handle : static_cast<MenuBar*>(this->Parent)->GetHandle();
    SetMenuItemBitmaps(parent, reinterpret_cast<UINT_PTR>(this->Handle), MF_BYCOMMAND, DeSelected, Selected);
}

bool MenuItem::ToggleCheck()
{
    MENUITEMINFOW MenuInfo = {0};
    MenuInfo.cbSize = sizeof(MenuInfo);
    MenuInfo.fMask = MIIM_STATE;
    HMENU parent = this->Parent->IsTypeOf(ControlStyle::CTRL_MENU_ITEM) ? static_cast<MenuItem*>(this->Parent)->Handle : static_cast<MenuBar*>(this->Parent)->GetHandle();
    GetMenuItemInfoW(parent, this->GetID(), false, &MenuInfo);
    MenuInfo.fState = (MenuInfo.fState & MF_CHECKED ? MenuInfo.fState & ~MF_CHECKED : MenuInfo.fState | MF_CHECKED);
    SetMenuItemInfoW(parent, this->GetID(), false, &MenuInfo);
    return MenuInfo.fState & MF_CHECKED;
}

bool MenuItem::IsChecked() const
{
    MENUITEMINFOW MenuInfo = {0};
    MenuInfo.cbSize = sizeof(MenuInfo);
    MenuInfo.fMask = MIIM_STATE;
    HMENU parent = this->Parent->IsTypeOf(ControlStyle::CTRL_MENU_ITEM) ? static_cast<MenuItem*>(this->Parent)->Handle : static_cast<MenuBar*>(this->Parent)->GetHandle();
    GetMenuItemInfoW(parent, this->GetID(), false, &MenuInfo);
    return MenuInfo.fState & MF_CHECKED;
}

const wchar_t* MenuItem::GetText()
{
    if (IsTypeOf(ControlStyle::CTRL_MENU_ITEM))
    {
        HMENU parent = this->Parent->IsTypeOf(ControlStyle::CTRL_MENU_ITEM) ? static_cast<MenuItem*>(this->Parent)->Handle : static_cast<MenuBar*>(this->Parent)->GetHandle();

        MENUITEMINFOW ItemInfo = {0};
        ItemInfo.cbSize = sizeof(ItemInfo);
        ItemInfo.fMask = MIIM_TYPE;
        ItemInfo.dwTypeData = nullptr;
        GetMenuItemInfoW(parent, this->GetID(), false, &ItemInfo);

        if (ItemInfo.dwTypeData == MFT_STRING)
        {
            Text.resize(++ItemInfo.cch);
            ItemInfo.dwTypeData = &Text[0];
            if (GetMenuItemInfoW(parent, this->GetID(), false, &ItemInfo))
            {
                return Text.c_str();
            }
        }
    }
    return nullptr;
}



ControlStyle ToolBar::GetControlStyle() const
{
    return ControlStyle::CTRL_TOOL_BAR;
}

ToolBar::ToolBar(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent) : Control()
{
    WindowStyle style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | CCS_ADJUSTABLE | CCS_NODIVIDER);
    Create(style, WindowStyle::NONE, nullptr, TOOLBARCLASSNAMEW, Location, Width, Height, Parent);
}

void ToolBar::AdjustSize()
{
    SendMessage(*this, TB_AUTOSIZE, 0, 0);
}



ControlStyle RichTextBox::GetControlStyle() const
{
    return ControlStyle::CTRL_RICH_EDIT;
}

RichTextBox::RichTextBox(const wchar_t* Text, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool MultiLine) : Control(), module(nullptr)
{
    this->module = LoadLibraryW(L"Msftedit.dll");
    WindowStyle style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE | (MultiLine ? ES_MULTILINE | ES_AUTOVSCROLL : 0));
    Create(style, WindowStyle::EX_STATICEDGE, Text, MSFTEDIT_CLASS, Location, Width, Height, Parent);
}

RichTextBox::~RichTextBox()
{
    FreeLibrary(this->module);
}

void RichTextBox::SetReadOnly(bool ReadOnly)
{
    SendMessageW(*this, EM_SETREADONLY, ReadOnly, 0);
}

void RichTextBox::SetPassword(bool Enabled, wchar_t PasswordChar)
{
    SendMessageW(*this, EM_SETPASSWORDCHAR, Enabled ? PasswordChar : 0, 0);
}

std::uint32_t RichTextBox::GetTextLength() const
{
    return GetWindowTextLengthW(*this);
}

void RichTextBox::ShowScrollBar(bool Show, std::uint32_t BarStyle)
{
    ::ShowScrollBar(*this, BarStyle, Show);
}

void RichTextBox::AppendText(const wchar_t* Text) const
{
    SendMessageW(*this, EM_SETSEL, -1, -1);
    SendMessageW(*this, EM_REPLACESEL, 0, reinterpret_cast<LPARAM>(Text));
}

bool RichTextBox::FindTextInSelection(const wchar_t* Text)
{
    CHARRANGE SelectedRange;
    FINDTEXTEXW FindTextEx;
    SendMessageW(*this, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&SelectedRange));

    FindTextEx.lpstrText = Text;
    FindTextEx.chrg.cpMin = SelectedRange.cpMin;
    FindTextEx.chrg.cpMax = SelectedRange.cpMax;
    if (SendMessageW(*this, EM_FINDTEXTEXW, static_cast<WPARAM>(FR_DOWN), reinterpret_cast<LPARAM>(&FindTextEx)) > 0)
    {
        SendMessageW(*this, EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(&FindTextEx.chrgText));
        SendMessageW(*this, EM_HIDESELECTION, false, 0);
        return true;
    }
    return false;
}

bool RichTextBox::LoadFile(const wchar_t* FilePath)
{
    HANDLE hFile = CreateFileW(FilePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        EDITSTREAM Stream = {0};
        Stream.dwCookie = reinterpret_cast<DWORD_PTR>(hFile);
        Stream.pfnCallback = RichTextBox::EditStreamCallback;
        if (SendMessageW(*this, EM_STREAMIN, SF_RTF, reinterpret_cast<LPARAM>(&Stream)) && !Stream.dwError)
        {
            CloseHandle(hFile);
            return true;
        }
    }
    CloseHandle(hFile);
    return false;
}

DWORD __stdcall RichTextBox::EditStreamCallback(DWORD_PTR dwCookie, std::uint8_t* Buffer, long cb, long* pcb)
{
    HANDLE hFile = reinterpret_cast<HANDLE>(dwCookie);
    if (ReadFile(hFile, Buffer, cb, reinterpret_cast<DWORD*>(pcb), nullptr))
    {
        return 0;
    }
    return -1;
}



ControlStyle TabControl::GetControlStyle() const
{
    return ControlStyle::CTRL_TAB_PANEL;
}

TabControl::TabControl(TabStyle Style, POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent) : Control(), Icons(ImageList_Create(16, 16, ILC_COLOR32, 0, 1))
{
    WindowStyle style = static_cast<WindowStyle>(WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | static_cast<std::uint32_t>(Style));
    Create(style, WindowStyle::NONE, nullptr, WC_TABCONTROLW, Location, Width, Height, Parent);
}

void TabControl::AddTab(const wchar_t* Text, HBITMAP Image)
{
    std::uint32_t count = SendMessageW(*this, TCM_GETITEMCOUNT, 0, 0);

    TCITEMW Item = {0};
    Item.iImage = Image ? count : -1;
    Item.mask = Text ? TCIF_TEXT : 0;
    Item.mask |= Image ? TCIF_IMAGE : 0;
    Item.pszText = const_cast<wchar_t*>(Text);

    if (Image)
    {
        ImageList_Add(Icons, Image, NULL);
        SendMessageW(*this, TCM_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(Icons));
        ImageList_SetBkColor(Icons, CLR_NONE);
    }

    SendMessageW(*this, TCM_INSERTITEMW, static_cast<WPARAM>(count), reinterpret_cast<LPARAM>(&Item));
}

void TabControl::AddTab(const wchar_t* Text, HICON Image)
{
    std::uint32_t count = SendMessageW(*this, TCM_GETITEMCOUNT, 0, 0);

    TCITEMW Item = {0};
    Item.iImage = Image ? count : -1;
    Item.mask = Text ? TCIF_TEXT : 0;
    Item.mask |= Image ? TCIF_IMAGE : 0;
    Item.pszText = const_cast<wchar_t*>(Text);

    if (Image)
    {
        ImageList_AddIcon(Icons, Image);
        SendMessageW(*this, TCM_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(Icons));
        ImageList_SetBkColor(Icons, CLR_NONE);
    }

    SendMessageW(*this, TCM_INSERTITEMW, static_cast<WPARAM>(count), reinterpret_cast<LPARAM>(&Item));
}

void TabControl::RemoveTab(std::int32_t Index)
{
    ImageList_Remove(Icons, Index);
    SendMessageW(*this, TCM_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(Icons));
    SendMessageW(*this, Index < 0 ? TCM_DELETEALLITEMS : TCM_DELETEITEM, 0, 0);
}



ControlStyle ReBar::GetControlStyle() const
{
    return ControlStyle::CTRL_REBAR;
}

ReBar::ReBar(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, ToolBar& Toolbar) : Control(), band()
{
    WindowStyle style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | RBS_VARHEIGHT | CCS_NODIVIDER | RBS_BANDBORDERS);
    Create(style, WindowStyle::EX_TOOLWINDOW, nullptr, REBARCLASSNAMEW, Location, Width, Height, Parent);

    band.cbSize = sizeof(REBARBANDINFOW);
    band.fMask = RBBIM_STYLE | RBBIM_TEXT | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;
    band.fStyle = RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS;

    DWORD dwSize = static_cast<DWORD>(SendMessageW(Toolbar, TB_GETBUTTONSIZE, 0, 0));

    band.lpText = nullptr;
    band.hwndChild = Toolbar;
    band.cyChild = HIWORD(dwSize);
    band.cxMinChild = 0 * LOWORD(dwSize);
    band.cyMinChild = HIWORD(dwSize);
    band.cx = 0;
}

void ReBar::AdjustSize()
{
    SendMessage(*this, WM_SIZE, 0, 0);
}

void ReBar::AddBand()
{
    std::uint32_t count = SendMessageW(*this, RB_GETBANDCOUNT, 0, 0);
    SendMessageW(*this, RB_INSERTBANDW, static_cast<WPARAM>(count), reinterpret_cast<LPARAM>(&band));
}

void ReBar::AddBand(std::uint32_t Index)
{
    SendMessageW(*this, RB_INSERTBANDW, static_cast<WPARAM>(Index), reinterpret_cast<LPARAM>(&band));
}

void ReBar::RemoveBand(std::uint32_t Index)
{
    SendMessageW(*this, RB_DELETEBAND, static_cast<WPARAM>(Index), 0);
}



ControlStyle StatusBar::GetControlStyle() const
{
    return ControlStyle::CTRL_STATUS_BAR;
}

StatusBar::StatusBar(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent, bool GripBar) : Control()
{
    WindowStyle style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE | (GripBar ? SBARS_SIZEGRIP : 0));
    Create(style, WindowStyle::NONE, nullptr, STATUSCLASSNAMEW, Location, Width, Height, Parent);
}

const wchar_t* StatusBar::GetText(std::size_t Index)
{
    return 0;
}

void StatusBar::SetText(const wchar_t* Text, std::size_t Index)
{
    SendMessageW(*this, SB_SETTEXTW, Index, reinterpret_cast<LPARAM>(Text));
}

void StatusBar::AdjustSize()
{
    SendMessage(*this, WM_SIZE, 0, 0);
}



ControlStyle DatePicker::GetControlStyle() const
{
    return ControlStyle::CTRL_DATE_PICKER;
}

DatePicker::DatePicker(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent) : Control()
{
    WindowStyle style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE);
    Create(style, WindowStyle::NONE, nullptr, DATETIMEPICK_CLASSW, Location, Width, Height, Parent);
}



ControlStyle Calendar::GetControlStyle() const
{
    return ControlStyle::CTRL_CALENDAR;
}

Calendar::Calendar(POINT Location, std::uint16_t Width, std::uint16_t Height, HWND Parent)
{
    WindowStyle style = static_cast<WindowStyle>(WS_CHILD | WS_VISIBLE);
    Create(style, WindowStyle::NONE, nullptr, MONTHCAL_CLASSW, Location, Width, Height, Parent);
}
