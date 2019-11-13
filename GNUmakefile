ifdef DEBUG
CPPFLAGS:=-DWINVER=_WIN32_WINNT_WINXP -D_WIN32_WINNT=_WIN32_WINNT_WINXP -Iemacs26
CXXFLAGS:=-std=c++11 -O3 -Wall -Werror -g 
else
CPPFLAGS:=-DWINVER=_WIN32_WINNT_WINXP -D_WIN32_WINNT=_WIN32_WINNT_WINXP -Iemacs26 -DNDEBUG=1 
CXXFLAGS:=-std=c++11 -O3 -Wall -Werror
endif

RM:=rm
all_TARGET:=w32-imm32-on-start-enabler-impl.dll
W32_IMM32_ON_START_ENABLER_IMPL_OBJS:=w32-imm32-on-start-enabler-impl.o windows-hook.o

w32-imm32-on-start-enabler-impl.dll: $(W32_IMM32_ON_START_ENABLER_IMPL_OBJS)
	$(CXX) $(CXXFLAGS) --shared -o $@ $(CXXFLAGS) $(CPPFLAGS) -Wl,-Map=$(basename $@).map $+ -limm32 -lComctl32 -lgdi32 -lUser32 

w32-imm32-on-start-enabler-impl.o:w32-imm32-on-start-enabler-impl.cpp windows-hook.h
windows-hook.o:windows-hook.cpp windows-hook.h


clean:
	$(RM) -f $(all_TARGET) $(W32_IMM32_ON_START_ENABLER_IMPL_OBJS) *~
