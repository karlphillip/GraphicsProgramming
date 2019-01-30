// https://westechsolutions.net/sites/WindowedBorderlessGaming/
#include <Windows.h>
#include <iostream>


int main(int argc, char* argv[])
{
    HWND win_handle = FindWindow(0, L"Overwatch");
    if (!win_handle)
    {
        std::cout << "Failed FindWindow" << std::endl;
        return 0;
    }

    // Resize
//    unsigned int flags = (SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
//    unsigned int flags = (SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    unsigned int flags = (SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    flags &= ~SWP_NOSIZE; // essential
    unsigned int x = 300;
    unsigned int y = 200;
    unsigned int w = 960;
    unsigned int h = 540;
    SetWindowPos(win_handle, HWND_NOTOPMOST, x, y, w, h, flags);


//    SendMessage(win_handle, WM_SIZE, SIZE_RESTORED, MAKELPARAM(x, y));
//    PostMessage(win_handle, WM_SIZE, SIZE_RESTORED, MAKELPARAM(x, y));

//    InvalidateRect(win_handle, NULL, TRUE);
//    SendMessage(win_handle, WM_SETREDRAW, TRUE, 0);   // Re-enable repaint
//    RedrawWindow(win_handle, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);


//    RedrawWindow(win_handle, &g::rcDesktop , 0 , RDW_INTERNALPAINT | RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_INVALIDATE);

    // Borderless
//    SetWindowLong(win_handle, GWL_STYLE, GetWindowLong(win_handle, GWL_EXSTYLE) | WS_EX_TOPMOST);
//    SetWindowLong(win_handle, GWL_STYLE, WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW);
//    SetWindowLong(win_handle, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);
    SetWindowLong(win_handle, GWL_STYLE, WS_SYSMENU | WS_OVERLAPPEDWINDOW);


//    UpdateWindow(win_handle);

//    RECT rc = { x, y, w, h};
//    RedrawWindow(win_handle, &rc, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW );

    ShowWindow(win_handle, SW_SHOW);

    return 0;
}
