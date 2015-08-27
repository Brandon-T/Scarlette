#include "DebugWindow.hxx"



int __stdcall WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hThisInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpszArgument);
    UNREFERENCED_PARAMETER(nCmdShow);

    EnableVisualStyles();
    DebugWindow Win(L"Client", {CW_USEDEFAULT, CW_USEDEFAULT}, 1200, 700);

    Win.StartProcessDebugging();

    return Win.MessageLoop();
}
