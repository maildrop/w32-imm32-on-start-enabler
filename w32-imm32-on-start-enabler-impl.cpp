#include <tchar.h>
#include <windows.h>
#include <array>
#include "windows-hook.h"
#include "emacs-module.h"

extern "C"{
  __declspec( dllexport ) int plugin_is_GPL_compatible = 1;
};

template<typename emacs_env_t>
static inline void
fset( emacs_env_t* env , emacs_value symbol, emacs_value function )
{
  std::array<emacs_value , 2> args = {symbol,function};
  env->funcall( env , env->intern( env , u8"fset" ) , args.size(),  args.data() );
  return;
}

template<typename emacs_env_t>
static inline emacs_value
message_utf8( emacs_env_t* env , const std::string& utf8_text ) 
{
  std::array<emacs_value , 1> args = { env->make_string( env, utf8_text.c_str() ,  utf8_text.size() ) };
  return env->funcall( env , env->intern( env, u8"message" ), args.size(),  args.data() );
}

template<typename emacs_env_t>
static emacs_value
Fw32__not_implemented( emacs_env *env,
                              ptrdiff_t nargs , emacs_value[] ,
                              void* ) EMACS_NOEXCEPT
{
  message_utf8( env , std::string( u8"not implemented" ) );
  return env->intern( env, u8"nil" );
}

template<typename emacs_env_t>
static emacs_value
Fw32_imm32_on_start_enabler_impl_hwnd_to_threadid( emacs_env* env,
                                                   ptrdiff_t nargs , emacs_value args[] ,
                                                   void *data ) EMACS_NOEXCEPT
{
  if( 1 != nargs )
    return env->intern( env, u8"nil");
  HWND hWnd = reinterpret_cast<HWND>( env->extract_integer( env , args[0] ));
  if( IsWindow( hWnd )){
    DWORD threadid = GetWindowThreadProcessId( hWnd , nullptr );
    return env->make_integer( env , intmax_t( threadid ));
  }
  return env->intern( env, u8"nil" );
}

template<typename emacs_env_t>
static emacs_value
Fw32_imm32_on_start_enabler_impl_inject( emacs_env* env,
                                  ptrdiff_t nargs , emacs_value args[] ,
                                  void *data ) EMACS_NOEXCEPT
{
  if( 1 != nargs )
    return env->intern( env , u8"nil" );
  DWORD threadid = env->extract_integer( env,args[0] );
  if( threadid ){
    return env->make_integer(env, intmax_t(w32_imm32_on_start_enabler_impl_inject( threadid ) ));
  }
  return env->intern( env, u8"nil" );
}

template<typename emacs_env_t>
static emacs_value
Fw32_imm32_on_start_enabler_impl_deinject( emacs_env* env,
                                  ptrdiff_t nargs , emacs_value args[] ,
                                  void *data ) EMACS_NOEXCEPT
{
  message_utf8( env , std::string( u8"not implemented" ) );
  return env->intern( env, u8"nil" );
}

template<typename emacs_env_t>
static inline emacs_module_init_impl( emacs_env_t* env ) noexcept
{
  fset( env ,
        env->intern( env , u8"w32-imm32-on-start-enabler-impl-hwnd-to-threadid"),
        (env->make_function( env, 1, 1 , Fw32_imm32_on_start_enabler_impl_hwnd_to_threadid<emacs_env_t> ,
                             u8"HWND to threadid" , NULL )));
  fset( env,
        env->intern( env , u8"w32_imm32_on_start_enabler_impl_inject" ),
        (env->make_function( env, 1, 1 , Fw32_imm32_on_start_enabler_impl_inject<emacs_env_t>,
                             u8"inject GetMessage hook", NULL )));
  fset( env,
        env->intern( env , u8"w32_imm32_on_start_enabler_impl_deinject"),
        (env->make_function( env, 0, 0 , Fw32_imm32_on_start_enabler_impl_deinject<emacs_env_t>,
                             u8"deinject GetMessage hook" , NULL )));

  std::array<emacs_value , 1> provide_args = { env->intern( env, u8"w32-imm32-on-start-enabler-impl") };
  env->funcall( env ,
		env->intern( env , u8"provide"),
		provide_args.size(),
		provide_args.data() );
  return 0;
}


extern "C" __declspec( dllexport ) int
emacs_module_init ( struct emacs_runtime *ert ) EMACS_NOEXCEPT
{
  if( 0 < ert->size && 
      sizeof( emacs_runtime ) <= static_cast<size_t>(ert->size) )
    {
      emacs_env* env = ert->get_environment( ert );
      if( 0 < env->size &&
          sizeof( emacs_env ) <= static_cast<size_t>(env->size) ){
        return emacs_module_init_impl( reinterpret_cast<emacs_env*>(ert->get_environment( ert )) );
      }
    }
  return 0;
}
