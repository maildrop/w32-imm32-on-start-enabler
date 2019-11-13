#include <tchar.h>
#include <windows.h>

#include "windows-hook.h"

static HHOOK hhook = 0;

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
          return CallNextHookEx( NULL , code , wParam , lParam );
        }
      }
    }
  }
  return CallNextHookEx( NULL, code , wParam , lParam );
}

HHOOK w32_imm32_on_start_enabler_impl_inject(DWORD threadid)
{
  if( hhook ){
    return hhook;
  }else{
    return (hhook = SetWindowsHookEx( WH_GETMESSAGE , hook_GetMsgProc, GetModuleHandle( NULL ), threadid ));
  }
}

BOOL w32_imm32_on_start_enabler_impl_deinject()
{
  if( hhook ){
    return UnhookWindowsHookEx( hhook );
  }else{
    return FALSE;
  }
}
