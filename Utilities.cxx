#include "Utilities.hxx"


void TrackMouse(HWND hwnd)
{
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_HOVER | TME_LEAVE;
    tme.dwHoverTime = 1;
    tme.hwndTrack = hwnd;
    TrackMouseEvent(&tme);
}

void LockSystemMenu(HWND hwnd, bool lock)
{
    HMENU hMnu = GetSystemMenu(hwnd, lock);
    RemoveMenu(hMnu, SC_CLOSE, MF_BYCOMMAND);
    RemoveMenu(hMnu, SC_SIZE, MF_BYCOMMAND);
    RemoveMenu(hMnu, SC_SEPARATOR, MF_BYCOMMAND);
    RemoveMenu(hMnu, SC_STATUS_PROCESS_INFO, MF_BYCOMMAND);
    RemoveMenu(hMnu, SC_MOVE, MF_BYCOMMAND);
    RemoveMenu(hMnu, SC_MAXIMIZE, MF_BYCOMMAND);
    RemoveMenu(hMnu, SC_MINIMIZE, MF_BYCOMMAND);
}

void ClearConsoleRow(int Row)
{
    COORD coord;
    coord.X = 0;
    coord.Y = (Row - 1);
    DWORD dwBytes = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO Info;
    GetConsoleScreenBufferInfo(hStdOut, &Info);
    FillConsoleOutputCharacter(hStdOut, ' ', Info.dwMaximumWindowSize.Y, coord, &dwBytes);
    SetConsoleCursorPosition(hStdOut, Info.dwCursorPosition);
}

void SetWindowTransparency(HWND hwnd, std::uint8_t Transperancy)
{
    long wAttr = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, wAttr | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, Transperancy, 2);
}

void EnableVisualStyles()
{
    wchar_t sys_dir[MAX_PATH] = {0};
    std::uint32_t len = GetSystemDirectoryW(sys_dir, sizeof(sys_dir) / sizeof(sys_dir[0]));
    if (len < sizeof(sys_dir) / sizeof(sys_dir[0]))
    {
        ACTCTXW actCtx =
        {
            sizeof(ACTCTX), ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_SET_PROCESS_DEFAULT |
            ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID, L"shell32.dll", 0, 0, sys_dir, reinterpret_cast<wchar_t*>(0x7C)
        };

        ULONG_PTR ulpActivationCookie = false;
        ActivateActCtx(CreateActCtxW(&actCtx), &ulpActivationCookie);
    }
}

void SetWindowStyle(HWND hwnd, WindowStyle style, bool remove_style)
{
    LONG_PTR current_style = GetWindowLongPtr(hwnd, GWL_STYLE);
    SetWindowLongPtrW(hwnd, GWL_STYLE, remove_style ? (current_style & ~static_cast<std::uint32_t>(style)) : (current_style | static_cast<std::uint32_t>(style)));
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void SetExtendedWindowStyle(HWND hwnd, WindowStyle style, bool remove_style)
{
    LONG_PTR current_style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, remove_style ? (current_style & ~static_cast<std::uint32_t>(style)) : (current_style | static_cast<std::uint32_t>(style)));
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

std::wstring GetErrorMessage(std::uint32_t Error)
{
    LPWSTR lpMsgBuf = nullptr;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, Error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&lpMsgBuf), 0, nullptr);
    std::wstring res(lpMsgBuf);
    LocalFree(lpMsgBuf);
    return res;
}

void* GetMenuData(HMENU menu)
{
    MENUINFO MenuInfo = {0};
    MenuInfo.cbSize = sizeof(MenuInfo);
    MenuInfo.fMask = MIM_MENUDATA;
    GetMenuInfo(menu, &MenuInfo);
    return reinterpret_cast<void*>(MenuInfo.dwMenuData);
}

void* GetMenuItemData(HMENU menu, UINT id)
{
    MENUITEMINFOW ItemInfo = {0};
    ItemInfo.cbSize = sizeof(ItemInfo);
    ItemInfo.fMask = MIIM_DATA;
    GetMenuItemInfoW(menu, id, false, &ItemInfo);
    return reinterpret_cast<void*>(ItemInfo.dwItemData);
}

void SetMenuData(HMENU menu, bool notify_by_command, void* self_ptr)
{
    MENUINFO MenuInfo = {0};
    MenuInfo.cbSize = sizeof(MenuInfo);
    MenuInfo.fMask = MIM_STYLE | MIM_MENUDATA;
    GetMenuInfo(menu, &MenuInfo);
    MenuInfo.dwStyle = (notify_by_command ? (MenuInfo.dwStyle & ~MNS_NOTIFYBYPOS) : (MenuInfo.dwStyle | MNS_NOTIFYBYPOS));
    MenuInfo.dwMenuData = reinterpret_cast<ULONG_PTR>(self_ptr);
    SetMenuInfo(menu, &MenuInfo);
}

void SetMenuItemData(HMENU menu, HMENU sub_menu, int id, void* self_ptr)
{
    MENUITEMINFOW ItemInfo = {0};
    ItemInfo.cbSize = sizeof(ItemInfo);
    ItemInfo.fMask = MIIM_DATA;
    if (id > 0)
    {
        ItemInfo.fMask |= MIIM_ID;
        ItemInfo.wID = id;
    }
    ItemInfo.dwItemData = reinterpret_cast<ULONG_PTR>(self_ptr);
    SetMenuItemInfoW(menu, reinterpret_cast<UINT_PTR>(sub_menu), false, &ItemInfo);
}

#ifdef PSAPI
bool GetFileName(HANDLE handle)
{
    bool result = false;
	HANDLE hFileMap = CreateFileMapping(handle, NULL, PAGE_READONLY, 0, 1, NULL);

	if (hFileMap)
	{
		void* pData = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

		if (pData)
		{
		    char buffer[MAX_PATH + 1];
			if (GetMappedFileName(GetCurrentProcess(), pData, buffer, MAX_PATH))
			{
			    std::cout<<buffer<<"\n";
			    result = true;
			}
			UnmapViewOfFile(pData);
		}
		CloseHandle(hFileMap);
	}
	Sleep(5000);
	return false;
}
#else
bool GetFileName(HANDLE handle, wchar_t* path)
{
    DWORD (__stdcall *GetFinalPathNameByHandleW)(HANDLE hFile, LPCWSTR lpszFilePath, DWORD cchFilePath, DWORD dwFlags);
    GetFinalPathNameByHandleW = reinterpret_cast<decltype(GetFinalPathNameByHandleW)>(GetProcAddress(GetModuleHandle("kernel32.dll"), "GetFinalPathNameByHandleW"));
    DWORD res = GetFinalPathNameByHandleW(handle, path, MAX_PATH, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);

    if (res >= MAX_PATH)
    {
        return false;
    }
    return true;

    /*NTSTATUS (__stdcall *NtQueryInformationFile)(HANDLE hFile, IO_STATUS_BLOCK* io, void* ptr, unsigned long len, FILE_INFORMATION_CLASS FileInformationClass);
    NTSTATUS (__stdcall *NtQueryObject)(HANDLE Handle, OBJECT_INFORMATION_CLASS ObjectInformationClass, void* ObjectInformation, unsigned long ObjectInformationLength, unsigned long* ReturnLength);
    NtQueryInformationFile = reinterpret_cast<decltype(NtQueryInformationFile)>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryInformationFile"));
    NtQueryObject = reinterpret_cast<decltype(NtQueryObject)>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryObject"));

    IO_STATUS_BLOCK StatusIOBlock = {0};
    unsigned char buffer[MAX_PATH * sizeof(wchar_t) + sizeof(ULONG)];

    NTSTATUS result = NtQueryInformationFile(handle, &StatusIOBlock, &buffer[0], MAX_PATH * sizeof(wchar_t), FileNameInformation);

    if (NT_SUCCESS(result))
    {
        FILE_NAME_INFORMATION* FileNameInfo = reinterpret_cast<FILE_NAME_INFORMATION*>(&buffer[0]);
        if (FileNameInfo->FileNameLength > 0)
        {
            memcpy(&path[0], FileNameInfo->FileName, FileNameInfo->FileNameLength);
            return true;
        }
    }*/
    return false;
}
#endif
