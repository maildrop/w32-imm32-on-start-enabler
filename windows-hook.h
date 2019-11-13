#if !defined( WINDOW_HOOK_H_HEADER_GUARD_9c8936b9_7088_44db_9868_7b8a5ed911fc )
#define WINDOW_HOOK_H_HEADER_GUARD_9c8936b9_7088_44db_9868_7b8a5ed911fc 1

#include <tchar.h>
#include <windows.h>

#if defined( __cplusplus )
extern "C"{
#endif /* defined( __cplusplus ) */
  HHOOK w32_imm32_on_start_enabler_impl_inject(DWORD threadid);
  BOOL w32_imm32_on_start_enabler_impl_deinject();
#if defined( __cplusplus )
}
#endif /* defined( __cplusplus ) */
#endif /* !defined( WINDOW_HOOK_H_HEADER_GUARD_9c8936b9_7088_44db_9868_7b8a5ed911fc ) */
