#include <tchar.h>
#include <windows.h>
#include "windows-hook.h"

#include <cassert>

static HHOOK hhook = 0;

static LRESULT CALLBACK hook_GetMsgProc( int code , WPARAM wParam , LPARAM lParam )
{
  if( code < 0 ){
    /*
      see section Return value 
      https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644981(v=vs.85)
      
      "If code is less than zero, the hook procedure must return the value returned by CallNextHookEx" 
    */
    return CallNextHookEx( NULL , code , wParam , lParam );
  }else if( HC_ACTION == code){
    if( PM_REMOVE == wParam ){
      MSG* const msg = reinterpret_cast<MSG*>(lParam);

      /* 
         The Emacs message pump is broken.
         When msg.hwnd is null, The emacs message pump dose not sent to DispatchMessage().
         This behavior interferes with the behavior of WM_TIMER and third-party IMEs that use threaded messages.

         So this message hook call DispatchMessage () at here. 
      */

      if( (msg) && ( NULL == msg->hwnd) ){
        switch( msg->message ){
        case WM_QUIT:
          return CallNextHookEx( NULL , code , wParam , lParam );
          break;
        default:
          {
            const LRESULT lResult = CallNextHookEx( NULL , code , wParam , lParam );
            DispatchMessage( msg ); 
            return lResult;
          }
          break;
        }
      }
    }
    static_assert( HC_ACTION == 0 , "HC_ACTION == 0" );
    /*
      see section Return value 
      https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644981(v=vs.85)

      "if code is greater than or equal to zero,
      it is highly recommended that you call CallNextHookEx and return the value it returns;
      otherwise, other applications that have installed WH_GETMESSAGE hooks will not receive hook notifications
      and may behave incorrectly as a result.
      If the hook procedure does not call CallNextHookEx, the return value should be zero."
    */
    return CallNextHookEx( NULL, code , wParam , lParam );
  }else{
    assert( 0 < code );
    /*
      see section Return value 
      https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644981(v=vs.85)

      "if code is greater than or equal to zero,
      it is highly recommended that you call CallNextHookEx and return the value it returns;
      otherwise, other applications that have installed WH_GETMESSAGE hooks will not receive hook notifications
      and may behave incorrectly as a result.
      If the hook procedure does not call CallNextHookEx, the return value should be zero."
    */
    return CallNextHookEx( NULL, code , wParam , lParam );
  }
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
