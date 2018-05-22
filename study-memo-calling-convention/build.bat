@echo off
cd /d %~dp0

if "%1"=="" ( 
echo "usage: %0 [x86|x64]"
exit /b
) 

set COMMON_OPT=/nologo /Wall /Od /GS- /guard:cf- /EHs-c- /Oi- /FAs

rem x86 or x64
set ARCH_NAME=%~1

call :BUILD_REGULAR_C cdecl /Gd
call :BUILD_REGULAR_C fastcall /Gr
call :BUILD_REGULAR_C stdcall /Gz
call :BUILD_REGULAR_C vectorcall "/Gv /arch:SSE2"
cl /TP %COMMON_OPT% /Fa"thiscall_%ARCH_NAME%.asm" /Fo"thiscall_%ARCH_NAME%.obj" /Fe"thiscall_%ARCH_NAME%.exe" /wd4514 /wd4820 thiscall.cpp
if "%ARCH_NAME%"=="x86" (
cl /TC %COMMON_OPT% /Fa"naked_unnaked.asm" /Fo"naked_unnaked.obj" /Fe"naked_unnaked.exe" naked_unnaked.c
cl /TC %COMMON_OPT% /Fa"naked_unnaked_custom.asm" /Fo"naked_unnaked_custom.obj" /Fe"naked_unnaked_custom.exe" naked_unnaked_custom.c
)
exit /b

:BUILD_REGULAR_C
set CC_SUFFIX=%~1
set CC_OPT=%~2
cl /TC %COMMON_OPT% %CC_OPT% /Fa"regular_%ARCH_NAME%_%CC_SUFFIX%.asm" /Fo"regular_%ARCH_NAME%_%CC_SUFFIX%.obj" /Fe"regular_%ARCH_NAME%_%CC_SUFFIX%.exe" regular.c
exit /b
