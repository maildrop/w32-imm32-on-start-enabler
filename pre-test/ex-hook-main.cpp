#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#include <process.h>

#include <iostream>
#include <locale>
#include <cstdio>
#include <cstdint>
#include <cassert>

#if defined( _MSC_VER )
# pragma comment(lib,"Ole32.lib")
# pragma comment(lib,"User32.lib")
# pragma comment(lib,"Gdi32.lib")
# pragma comment(lib,"Comctl32.lib")
# pragma comment(lib,"Imm32.lib")
#endif /* defined( _MSC_VER ) */

#if !defined( VERIFY )
# if defined( NDEBUG )
#  define VERIFY( exp ) do{ exp; }while( 0 )
# else /* defined( NDEBUG ) */
#  define VERIFY( exp ) assert( exp )
# endif  /* defined( NDEBUG ) */
#endif 

enum{
  WM_APP_NULL = (WM_APP+1),
  WM_APP_QUIT,
  WM_APP_END
};

struct uithread_arg{
};

static LRESULT WINAPI wnd_proc( HWND hwnd , UINT msg, WPARAM wParam, LPARAM lParam );
static unsigned __stdcall uithread_entry( HINSTANCE instance , uithread_arg* arg );
static unsigned __stdcall uithread_entry0( void * arg );

/* 
static LRESULT WINAPI callwndproc( int code , WPARAM wParam , LPARAM lParam );
*/
  
static LRESULT WINAPI wnd_proc( HWND hwnd , UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch( msg ){
  case WM_DESTROY:
    PostQuitMessage(0);
    return DefWindowProc( hwnd , msg, wParam , lParam );
  case WM_CREATE:
    return DefWindowProc( hwnd , msg, wParam , lParam );
  default:
    return DefWindowProc( hwnd , msg, wParam , lParam );
  }
}

static LRESULT CALLBACK hook_GetMsgProc( int code , WPARAM wParam , LPARAM lParam )
{
  if( code < 0 ){
    return CallNextHookEx( NULL , code , wParam , lParam );
  }else if( HC_ACTION == code){
    if( PM_REMOVE == wParam ){
      MSG* const msg = (MSG*)(lParam);
      if( (msg) && ( NULL == msg->hwnd) ){
        if( WM_QUIT == msg->message ){
          
        }else{
          /* 
             Do not pass message pump 

             The Emacs message pump is broken.
             When msg.hwnd is null, it is not sent to DispatchMessage().
             So here DispatchMessage () 
             and change messages other than those 
             belonging to user definition to harmless WM_NULL.

             Actually, it is desirable to be WM_APP or higher, 
             but Emacs uses WM_USER or later, so it is like this.             
          */
          
          DispatchMessage( msg ); 
          
          if( WM_USER <= msg->message &&
              msg->message < 0x10000 ){
            msg->message = WM_NULL;
            msg->wParam = 0;
            msg->lParam = 0;
          }
          return CallNextHookEx( NULL , code , wParam , lParam );
        }
      }
    }
  }
  return CallNextHookEx( NULL, code , wParam , lParam );
}

/*
static LRESULT CALLBACK callwndproc( int code , WPARAM wParam , LPARAM lParam )
{
  if( code < 0 ){
    return CallNextHookEx( NULL , code , wParam , lParam );
  }
  if( code == HC_ACTION ){
    const CWPSTRUCT * const cwpstruct = (CWPSTRUCT*)lParam;
    // message が 0xFFFF より上は、システムによって予約されている領域
    if( cwpstruct ){
      TCHAR buffer[1024];
      _sntprintf_s ( buffer , _TRUNCATE ,
                     TEXT("callwndproc %s hwnd: %llx, msg: 0x%04x, wParam:%llx, lParam%llx\n"),
                     (wParam ? TEXT("this") : TEXT("any")) ,
                     (unsigned long long int)(cwpstruct->hwnd), cwpstruct->message , cwpstruct->wParam ,cwpstruct->lParam );
      //OutputDebugString( buffer );
    }
  }
  return CallNextHookEx( NULL , code , wParam , lParam );
}
*/

static unsigned __stdcall uithread_entry( HINSTANCE instance , uithread_arg* arg )
{
  UNREFERENCED_PARAMETER( instance );
  UNREFERENCED_PARAMETER( arg );

  WNDCLASSEX wndcls= {0};
  wndcls.cbSize = sizeof( WNDCLASSEX );
  wndcls.style = CS_HREDRAW | CS_VREDRAW ;
  wndcls.lpfnWndProc = wnd_proc;
  wndcls.cbClsExtra = 0;
  wndcls.cbWndExtra = 0;
  wndcls.hInstance = instance;
  wndcls.hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
  wndcls.hCursor = LoadCursor( NULL , IDC_ARROW );
  wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndcls.lpszMenuName = NULL;
  wndcls.lpszClassName = TEXT("ex-hook-main");
  wndcls.hIconSm       = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);

  ATOM atom = RegisterClassEx( &wndcls ) ;
  if(atom){

    HWND hWnd = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW ,
                                LPCTSTR( atom ),
                                TEXT("ex-hoook-main"),
                                WS_OVERLAPPEDWINDOW ,
                                CW_USEDEFAULT,CW_USEDEFAULT,
                                100,100,
                                NULL,
                                NULL,
                                instance,
                                NULL );
    VERIFY( hWnd );
    if( hWnd ){

      (void)ShowWindow( hWnd , SW_SHOWDEFAULT );

       // window hook の導入
      //HHOOK call_hook = SetWindowsHookEx( WH_CALLWNDPROC , callwndproc, instance, GetCurrentThreadId() );
      HHOOK get_hook = SetWindowsHookEx( WH_GETMESSAGE , hook_GetMsgProc , instance , GetCurrentThreadId() );
      
      PostMessage( hWnd, WM_APP_NULL, 0,0 );
      MSG msg = {0};
      BOOL bRet = FALSE;
      while( 0 != (bRet = GetMessage( &msg , NULL , 0 ,0 ) )){
        if( bRet == -1 ){
          break;
        }else{

          if( msg.hwnd != NULL ){
            TranslateMessage( &msg );
            DispatchMessage( &msg );
          }else{
            TCHAR buffer[1024];
            _sntprintf_s ( buffer , _TRUNCATE ,
                           TEXT("GetMessage() return hwnd: %llx, msg:0x%04x, wParam:%llx, lParam%llx\n"),
                           (unsigned long long int)(msg.hwnd), msg.message , msg.wParam ,msg.lParam );
            OutputDebugString( buffer );
          }
        }
      }
      VERIFY( UnhookWindowsHookEx( get_hook ) );
      //VERIFY( UnhookWindowsHookEx( call_hook ) );
    }
    
    VERIFY( UnregisterClass( LPCTSTR(atom), instance ) );
  }else{
    assert(!"RegisterClassEx( &wndcls )" );
  }
  
  return 0;
}

static unsigned __stdcall uithread_entry0( void * arg )
{
  HRESULT const hr = CoInitializeEx( NULL , COINIT_APARTMENTTHREADED );
  assert( S_OK == hr );
  if( SUCCEEDED( hr ) ){
    unsigned const result = uithread_entry( GetModuleHandle(NULL), (uithread_arg*)arg );
    CoUninitialize();
    return result;
  }
  return 3;
}

int main(int argc,char* argv[])
{
  (void)std::locale::global( std::locale(""));

  UNREFERENCED_PARAMETER( argc );
  UNREFERENCED_PARAMETER( argv );
  
  HRESULT const hr = CoInitializeEx( NULL ,  COINIT_MULTITHREADED );
  assert( S_OK == hr );
  if( SUCCEEDED( hr ) ){
    INITCOMMONCONTROLSEX initcommoncontrolsex = { sizeof( INITCOMMONCONTROLSEX ), ICC_WIN95_CLASSES };
    VERIFY( InitCommonControlsEx(&initcommoncontrolsex ) );

    uithread_arg thread_arg = {};
    uintptr_t thread_handle_value = _beginthreadex( NULL, 0, uithread_entry0 , &thread_arg , 0, NULL );
    HANDLE thread_handle = (HANDLE)thread_handle_value;

    HANDLE waitlist[] = { thread_handle };
    for(;;){
      const DWORD w = MsgWaitForMultipleObjectsEx( sizeof( waitlist ) / sizeof( waitlist[0] ),
                                                   waitlist ,
                                                   INFINITE ,
                                                   QS_ALLEVENTS ,
                                                   MWMO_ALERTABLE | MWMO_INPUTAVAILABLE  );
      switch( w ){
      case WAIT_OBJECT_0:
        {
          DWORD exitCode = 0;
          (void)GetExitCodeThread( thread_handle , &exitCode );
          PostQuitMessage( exitCode );
        }
        break;
      case (WAIT_OBJECT_0 + sizeof( waitlist ) / sizeof( waitlist[0] )):
        break;
      case WAIT_ABANDONED_0:
        goto end_of_thread;
      case WAIT_IO_COMPLETION:
        break;
      case WAIT_TIMEOUT:
        break;
      case WAIT_FAILED:
        goto end_of_thread;
      default:
        break;
      }
      {
        MSG msg = {0};
        while( PeekMessage( &msg , NULL , 0 ,0 , PM_REMOVE ) ){
          if( WM_QUIT == msg.message ){
            goto end_of_thread;
          }
          TranslateMessage( &msg );
          DispatchMessage( &msg );
        }
      }
    }
  end_of_thread:
    VERIFY( CloseHandle( thread_handle ) );
    CoUninitialize();
  }
  return EXIT_SUCCESS;
}
