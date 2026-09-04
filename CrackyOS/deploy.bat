@echo off

REM ============================================================
REM Setup
REM ============================================================
setlocal EnableDelayedExpansion
set "PROGRAMMER=STM32_Programmer_CLI.exe"

REM ============================================================
REM Check variant argument
REM ============================================================
if "%~1"=="" (
    echo Usage: %~nx0 ^<variant^>
    echo Example: %~nx0 MyVariant
    exit /b 1
)

set "VARIANT=%~1"

REM ============================================================
REM Find build directory
REM ============================================================
set "SCRIPT_DIR=%~dp0"
set "BUILD_DIR=%SCRIPT_DIR%build_%VARIANT%"

if not exist "%BUILD_DIR%\" (
    echo Error: build directory not found:
    echo   %BUILD_DIR%
    exit /b 1
)

pushd "%BUILD_DIR%" || (
    echo Error: could not enter build directory.
    exit /b 1
)

REM ============================================================
REM Find and flash the .bin file
REM ============================================================
for %%F in (*.bin) do (
    echo Variant : %VARIANT%
    echo Build   : %BUILD_DIR%
    echo Binary  : %%F
    echo.
    call :flash "%%F"
    set "FLASH_RESULT=!ERRORLEVEL!"
    popd
    endlocal
    exit /b !FLASH_RESULT!
)

REM ============================================================
REM No .bin file found
REM ============================================================
echo No .bin file found in:
echo   %BUILD_DIR%
popd
endlocal
exit /b 1

REM ============================================================
REM Flash firmware with up to 3 attempts
REM ============================================================
:flash

for /l %%N in (1,1,3) do (
    echo Flash attempt %%N/3...
    "%PROGRAMMER%" -c port=SWD mode=UR freq=1000 -w "%~1" 0x08000000 -v -rst
    if not errorlevel 1 (
        echo.
        echo Flash successful.
        exit /b 0
    )
    echo Flash attempt %%N failed.
    if %%N LSS 3 (
        timeout /t 1 /nobreak >nul
    )
)
echo.
echo Flashing failed after 3 attempts.
exit /b 1
```
