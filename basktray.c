//basktray v2.0 tanta_1155
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>

#define LIM 256 //for val:str
#define DEFAULT_DKMD 0

#define TIMER_ID 1
#define TIMER_INTERVAL 1000 //ms

#define IDM_MODE 100
#define IDM_CSTM 101
#define IDM_EXIT 102

#define WM_MYMESSAGE (WM_USER + 1)

int fdkmd, cnt=0;
char sztip[LIM], str[LIM], deb[LIM];


WNDCLASSEX wc;
MSG msg;

NOTIFYICONDATA nid;
POINT pos;

HWND hwnd;
HICON hIcon;
HMENU hMenu;
HMENU OldhMenu;

SYSTEM_POWER_STATUS sps;

typedef struct battery_status{

    int lev;
    int sec;
    int hour;
    int min;
    int AC;

}BS;

void get_sps(BS *bs, SYSTEM_POWER_STATUS sps) {

    GetSystemPowerStatus(&sps);
    bs->lev = sps.BatteryLifePercent;
    bs->sec  = sps.BatteryLifeTime;
    bs->hour = bs->sec/3600;
    bs->min  = (bs->sec%3600)/60;
    bs->AC   = sps.ACLineStatus;
    
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    BS bs;

    switch(msg) {

        case WM_CREATE:
            
            // load icon
            sprintf(str,"./resources/ico-%d/0.ico",fdkmd);
            hIcon = LoadImage(NULL, str, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

            // reset NOTIFYICONDATA
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hwnd;
            nid.uID = 100;
            nid.uVersion = NOTIFYICON_VERSION;
            nid.uCallbackMessage = WM_MYMESSAGE;
            nid.hIcon = hIcon;
            nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
            strcpy(nid.szTip, "Loading...");

            // add tasktray icon
            Shell_NotifyIcon(NIM_ADD, &nid);

            // renew tasktray icon
            Shell_NotifyIcon(NIM_SETVERSION, &nid);

            // Create the right click menu
            hMenu = CreatePopupMenu();// get handle of menu
            if(fdkmd==1){
                AppendMenu(hMenu, MF_STRING | MF_CHECKED, IDM_MODE, TEXT("DarkMode"));
            }else{
                AppendMenu(hMenu, MF_STRING | MF_UNCHECKED, IDM_MODE, TEXT("DarkMode"));
            }
            AppendMenu(hMenu, MF_STRING, IDM_CSTM, TEXT("Custom"));
            AppendMenu(hMenu, MF_STRING, IDM_EXIT, TEXT("Exit"));
    
            SetMenu(hwnd,hMenu);

            SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL, NULL);

            break;
        
        case WM_TIMER:

            get_sps(&bs,sps); //get system power status
            
            sprintf(str, "./resources/ico-%d/%d.ico", fdkmd, bs.lev);

            HICON OldhIcon = nid.hIcon;
            nid.hIcon = LoadImage(NULL, str, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
            if (OldhIcon) {
                DestroyIcon(OldhIcon);
            }
            // load icon

            if (bs.AC) {
                sprintf(sztip,"Left : %d%%\n | AC plugged in |", bs.lev);
            }else{
                if (bs.sec==-1){
                    sprintf(sztip,"Left : %d%%\n| Loading |", bs.lev);
                }else{
                    sprintf(sztip,"Left : %d%%\n%dh %dmin", bs.lev, bs.hour, bs.min, bs.sec);//sprintf:cool!
                }
            }

            strcpy(nid.szTip, sztip);
            // tip text

            Shell_NotifyIcon(NIM_MODIFY, &nid);
            // sendmsg

            break;

        case WM_MYMESSAGE:
            switch(lParam) {

                case WM_LBUTTONDBLCLK:
                    // Left CLK (DouBle)
                    MessageBox(hwnd, "basktray v2.0  THX for using(^^)\n\nCopyright (c) 2024 tanta_1155", "basktray.exe", MB_OK | MB_SYSTEMMODAL);
                    //debug
                    //MessageBox(hwnd, deb, "basktray.exe", MB_OK | MB_SYSTEMMODAL);
                    break;

                case WM_RBUTTONDOWN:
                    // Right CLK
                    // Get the cursor position and show right CLK menu
                    GetCursorPos(&pos);
                    ClientToScreen(hwnd, &pos);
                    SetForegroundWindow(hwnd);//set menu foreground
                    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pos.x, pos.y, 0, hwnd, NULL);
                    break;
            }
            break;
        
        case WM_COMMAND:
            // Handle menu commands
            switch (LOWORD(wParam)) {

                case IDM_MODE:// 100
                    
                    if(fdkmd==1){
                        fdkmd=0;
                        WritePrivateProfileStringA(//Edit ini file
                            "section",
                            "fdkmd",
                            "0\0",
                            "./resources/basktray.ini"
                        );
                        CheckMenuItem(hMenu,IDM_MODE,MF_UNCHECKED);
                    }else{//fdkmd==0 or 2
                        fdkmd=1;
                        WritePrivateProfileStringA(
                            "section",
                            "fdkmd",
                            "1\0",
                            "./resources/basktray.ini"
                        );
                        CheckMenuItem(hMenu,IDM_MODE,MF_CHECKED);//add chk mark
                    }
                    
                    CheckMenuItem(hMenu,IDM_CSTM,MF_UNCHECKED);//unchk chk mark
                    SetMenu(hwnd,hMenu);
                    DrawMenuBar(hwnd);
                    
                    break;

                case IDM_CSTM://101
                
                    if (fdkmd!=2){
                        fdkmd=2;
                        WritePrivateProfileStringA(
                            "section",
                            "fdkmd",
                            "2\0",
                            "./resources/basktray.ini"
                        );
                        CheckMenuItem(hMenu,IDM_CSTM,MF_CHECKED);
                    }else{
                        fdkmd=0;
                        WritePrivateProfileStringA(
                            "section",
                            "fdkmd",
                            "0\0",
                            "./resources/basktray.ini"
                        );
                        CheckMenuItem(hMenu,IDM_CSTM,MF_UNCHECKED);
                    }
                    CheckMenuItem(hMenu,IDM_MODE,MF_UNCHECKED);
                    SetMenu(hwnd,hMenu);
                    DrawMenuBar(hwnd);
                    
                    break;
                
                case IDM_EXIT:// 102
                    SendMessage(hwnd, WM_CLOSE, 0, 0);//gentle closing
                    break;
            
            }
            break;
        
        case WM_DESTROY:
            KillTimer(hwnd,TIMER_ID);
            // kill timer count
            Shell_NotifyIcon(NIM_DELETE, &nid);
            // delete icon from tasktray
            DestroyIcon(hIcon);
            DestroyIcon(OldhIcon);
            DestroyMenu(hMenu);
            DestroyMenu(OldhMenu);
            DestroyWindow(hwnd);
            //free memory
            PostQuitMessage(0);
            //quit msg loop
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    FreeConsole();//undisplay terminal

    fdkmd = 
    GetPrivateProfileInt(
    "section",
    "fdkmd",
    DEFAULT_DKMD,
    "./resources/basktray.ini"
    );// load ini file


    // registar wnd class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "MyWindowClass";
    wc.hIconSm       = NULL;

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,// don't show on Alt+Tab menu && stealth
        "MyWindowClass",
        "title",
        WS_ICONIC,
        0, 0, 0, 0,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    SetParent(hwnd, GetDesktopWindow());

    // show window
    ShowWindow(hwnd, 0);
    UpdateWindow(hwnd);

    // start msg loop
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
