//
//  This project was used in the following screencast:
//
//      Part 3 - Reverse Engineering & Binary Augmentation / Modification (Patching) - Snipping Tool
//
//      https://www.youtube.com/watch?v=aNZjeP3GYKo
//
//
//  Created by dennisbabkin.com
//
//  This project is also a part of the blog post.
//
//  For more details, check:
//
//      https://dennisbabkin.com/blog/?i=AAA11C00
//


#include "framework.h"
#include "TestToolbar01.h"

#include <CommCtrl.h>
#include <assert.h>
#include <shellapi.h>




#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HWND ghToolbar;     //Toolbar handle


void createToolbar(HWND hWnd);
void destroyToolbar(HWND hWnd);
void calc_offsets();




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    //calc_offsets();

    //int ss = sizeof(int) + sizeof(TBBUTTON);
    //int dd = offsetof(TBBUTTON, idCommand);


    //ULONGLONG ii = 0x7FF651E30000 - (0x7FF651B34000 + 0x4000 - 8);



    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TESTTOOLBAR01, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTTOOLBAR01));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTTOOLBAR01));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TESTTOOLBAR01);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        createToolbar(hWnd);
    }
    return 0;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                {
                    if((HWND)lParam == ghToolbar)
                    {
                        if(wmId == 100)
                        {
                            ShellAbout(hWnd, L"My app", L"My info", NULL);
                        }
                    }
                }
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        destroyToolbar(hWnd);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }





    //ActivateActCtx(0, 0);       //marker: before

    static int gnRanOnce = 0;
    if(!gnRanOnce)
    {
        if(message == WM_PAINT)
        {
            gnRanOnce = 1;

            //List of APIs that we need to work with
            //  CommandLineToArgvW;  - no
            //  GetCommandLineW;     - no
            //  LocalFree;		 - yes
            //  GetDlgItem;		 - yes
            //  SendMessageW;	 - yes
            //  GetProcAddress;  - yes
            //  GetModuleHandleW; - yes

            LPWSTR* (WINAPI *pfnCommandLineToArgvW)(LPCWSTR lpCmdLine, int* pNumArgs);
            LPWSTR (WINAPI *pfnGetCommandLineW)(VOID);

            (FARPROC&)pfnCommandLineToArgvW = 
                GetProcAddress(GetModuleHandleW(L"Shell32.dll"), "CommandLineToArgvW");
            assert(pfnCommandLineToArgvW);

            (FARPROC&)pfnGetCommandLineW = 
                GetProcAddress(GetModuleHandleW(L"Kernel32.dll"), "GetCommandLineW");
            assert(pfnGetCommandLineW);




            int nArgs;
            LPWSTR* ppArgs = pfnCommandLineToArgvW(pfnGetCommandLineW(), &nArgs);     // main(argc, argv)
            if(ppArgs)
            {
                for(int c = 0; c < nArgs; c++)
                {
                    WCHAR* pCmd = ppArgs[c];

                    // command line argument is "1"
                    if(pCmd[0] == '1' &&
                        pCmd[1] == 0)
                    {
                        HWND hWndToolbar = GetDlgItem(hWnd, 1);
                        assert(hWndToolbar);

                        //Simulate a button click (on the 1st button)
                        TBBUTTON tb_Data;
                        if(SendMessage(hWndToolbar, TB_GETBUTTON, 
                            0,  //index of the button
                            (LPARAM)&tb_Data))
                        {
                            SendMessageW(hWndToolbar, WM_COMMAND, tb_Data.idCommand, (LPARAM)hWndToolbar);
                        }
                        else
                            assert(false);

                        break;
                    }
                }

                LocalFree(ppArgs);
            }
        }
    }

    //ActivateActCtx(0, 0);       //marker: after



    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}




void createToolbar(HWND hWnd)
{
    //Wnd Styles: 0x50009941 = WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT |
    //    TBSTYLE_LIST | TBSTYLE_TRANSPARENT | CCS_TOP | CCS_NODIVIDER

    //    Wnd ExStyles: 0x0
    //    Toolbar ExStyles: 0x11 = TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_HIDECLIPPEDBUTTONS


    ghToolbar = CreateWindowExW(0, TOOLBARCLASSNAME, L"", 
        WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT |
        TBSTYLE_LIST | TBSTYLE_TRANSPARENT | CCS_TOP | CCS_NODIVIDER,
        0, 0, 0, 0, hWnd, 
        (HMENU)1,            //Control ID for toolbar
        GetModuleHandle(NULL),
        0);
    assert(ghToolbar);

    SendMessageW(ghToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    static const TBBUTTON btns[] = {
        {I_IMAGENONE, 100, TBSTATE_ENABLED, BTNS_SHOWTEXT, {}, 0, (INT_PTR)L"One", },
        {I_IMAGENONE, 101, TBSTATE_ENABLED, BTNS_SHOWTEXT, {}, 0, (INT_PTR)L"Two", },
        {I_IMAGENONE, 102, TBSTATE_ENABLED, BTNS_SHOWTEXT, {}, 0, (INT_PTR)L"Three", },
    };

    SendMessageW(ghToolbar, TB_ADDBUTTONS, _countof(btns), (LPARAM)btns);


}


void destroyToolbar(HWND hWnd)
{
    if(ghToolbar)
    {
        DestroyWindow(ghToolbar);
        ghToolbar = NULL;
    }
}



void calc_offsets()
{

//                      Offset               offset			addr of next instruction

//  GetDlgItem;		    0x309168        User32.dll		    0x2ae44			00007FF651AFC054
//  SendMessageW;	 	0x309178        User32.dll		0x1ca90			00007FF651B0A3F8
//  LocalFree;		    0x3095C8        Kernel32.dll		0x3764			00007FF651B232D4
//  GetProcAddress;  	0x3094E8        Kernel32.dll	0x2e5c			00007FF651B23CBC
//  GetModuleHandleW; 	0x3094DE        Kernel32.dll	0x2e9f			00007FF651B23C83


    
    
    
    
    


    static struct JJJ
    {
        UINT offset;
        ULONGLONG pPtr;
    }
    vals[] = {
        {0x2ae44	,		0x00007FF651AFC054,},
        {0x1ca90	,		0x00007FF651B0A3F8,},
        {0x3764		,	    0x00007FF651B232D4,},
        {0x2e5c		,	    0x00007FF651B23CBC,},
        {0x2e9f		,	    0x00007FF651B23C83,},
    };

    WCHAR buff[256];

    for(int i =0 ; i < _countof(vals); i++)
    {
        ULONGLONG addrOfApiInIAT = vals[i].pPtr + vals[i].offset;

        ULONGLONG needed_offset = 0x7FF651E30000 - addrOfApiInIAT;

        swprintf_s(buff, _countof(buff), L"0x%IX\n", needed_offset);

        OutputDebugStringW(buff);
    }




    return ;
}


//                      Offset               offset			addr of next instruction

//  GetDlgItem;		    0x309168        User32.dll		    0x2ae44			00007FF651AFC054
//  SendMessageW;	 	0x309178        User32.dll		0x1ca90			00007FF651B0A3F8
//  LocalFree;		    0x3095C8        Kernel32.dll		0x3764			00007FF651B232D4
//  GetProcAddress;  	0x3094E8        Kernel32.dll	0x2e5c			00007FF651B23CBC
//  GetModuleHandleW; 	0x3094DE        Kernel32.dll	0x2e9f			00007FF651B23C83



// .text2

/*
lbl_begin:

    ; edi = assumed to be 0
    ; r14d = message
    ; rsi = hWnd

mov         r14d, esi
mov         rsi, rdi
xor         edi, edi

push        rax
sub         rsp, 0x28



lea         rax, [rip + lbl_begin - 0x2f8008]    ; rax = address of gnRanOnce
cmp         dword ptr [rax],edi
jne         lbl_end  

cmp         r14d,0Fh  
jne         lbl_end  

lea         rcx,[rip + str_Shell32]  
mov         dword ptr [rax],1               ; gnRanOnce
call        qword ptr [rip + lbl_begin + 0x3094DE]        ; GetModuleHandleW
mov         rcx,rax  
lea         rdx,[rip + str_CommandLineToArgvW]  
call        qword ptr [rip + lbl_begin + 0x3094E8]          ; GetProcAddress

lea         rcx,[rip + str_Kernel32]  
mov         rbx,rax  
call        qword ptr [rip + lbl_begin + 0x3094DE]      ; GetModuleHandleW
mov         rcx,rax  
lea         rdx,[rip + str_GetCommandLineW]  
call        qword ptr [rip + lbl_begin + 0x3094E8]     ; GetProcAddress

call        rax  
mov         rcx,rax  
lea         rdx,[rsp]                ; nArgs 
call        rbx  
mov         rbp,rax  

test        rax,rax  
je          lbl_end  

mov         r8d,dword ptr [rsp]     ; nArgs
test        r8d,r8d  
jle         lbl_free  
mov         rcx,rax  
nop         word ptr [rax+rax]  

lbl_00:

mov         rdx,qword ptr [rcx]  

cmp         word ptr [rdx],0x31  
jne         lbl_01  
cmp         word ptr [rdx+2],0  
je          lbl_02  

lbl_01:

inc         edi  
add         rcx,8  
cmp         edi,r8d  
jl          lbl_00  
jmp         lbl_free  

lbl_02:

mov         edx,1  
mov         rcx,rsi  
call        qword ptr [rip + lbl_begin + 0x309168]     ; GetDlgItem

lea         r9,[rsp + 8]       ; TBBUTTON tb_Data
xor         r8d,r8d  
mov         rcx,rax  
mov         edx,417h  
mov         rbx,rax  
call        qword ptr [rip + lbl_begin + 0x309178]   ; SendMessageW 
test        rax,rax  
je          lbl_free  

movsxd      r8,dword ptr [rsp + 8 + 4]     ; tb_Data.idCommand
mov         r9,rbx  
mov         edx,111h  
mov         rcx,rbx  
call        qword ptr [rip + lbl_begin + 0x309178]   ; SendMessageW 

lbl_free:

mov         rcx,rbp  
call        qword ptr [rip + lbl_begin + 0x3095C8]      ; LocalFree

lbl_end:


add         rsp, 0x28
pop         rax

mov    rbx,QWORD PTR [rsp+0x40]
mov    rbp,QWORD PTR [rsp+0x48]
mov    rsi,QWORD PTR [rsp+0x50]
mov    rdi,QWORD PTR [rsp+0x58]
add    rsp,0x30
pop    r14
ret


str_Shell32:
.ascii "S\000h\000e\000l\000l\0003\0002\000\056\000d\000l\000l\000\000\000"

str_CommandLineToArgvW:
.ascii "CommandLineToArgvW\000"

str_Kernel32:
.ascii "K\000e\000r\000n\000e\000l\0003\0002\000\056\000d\000l\000l\000\000\000"

str_GetCommandLineW:
.ascii "GetCommandLineW\000"







*/


