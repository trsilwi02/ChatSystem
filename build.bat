@echo off
REM ChatSystem Build Script for MinGW/GCC
REM Builds server and client, copies all dependencies

setlocal enabledelayedexpansion

echo ===== ChatSystem Build Script =====
echo.

REM Define paths
set QT_PATH=C:\Qt\6.11.0\mingw_64
set GCC_PATH=C:\Strawberry\c\bin
set BUILD_DIR=D:\ChatSystem\build
set SOURCE_DIR=D:\ChatSystem

echo [1/5] Cleaning previous build...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%" >nul 2>&1
)
mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

echo [2/5] Running CMake configuration...
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_PATH%" "%SOURCE_DIR%" || (
    echo ERROR: CMake failed!
    pause
    exit /b 1
)

echo [3/5] Compiling targets...
mingw32-make || (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo [4/5] Copying Qt runtime libraries...
copy /Y "%QT_PATH%\bin\Qt6Core.dll" . >nul
copy /Y "%QT_PATH%\bin\Qt6Gui.dll" . >nul
copy /Y "%QT_PATH%\bin\Qt6Widgets.dll" . >nul
copy /Y "%QT_PATH%\bin\Qt6Network.dll" . >nul
copy /Y "%QT_PATH%\bin\Qt6Concurrent.dll" . >nul

echo Copying Qt platform plugins...
if not exist "plugins" mkdir plugins
xcopy /Y /E /I "%QT_PATH%\plugins\platforms" "plugins\platforms" >nul
xcopy /Y /E /I "%QT_PATH%\plugins\imageformats" "plugins\imageformats" >nul
xcopy /Y /E /I "%QT_PATH%\plugins\styles" "plugins\styles" >nul

echo [5/5] Copying GCC runtime libraries...
copy /Y "%GCC_PATH%\libstdc++-6.dll" . >nul 2>&1
copy /Y "%GCC_PATH%\libgcc_s_seh-1.dll" . >nul 2>&1
copy /Y "%GCC_PATH%\libwinpthread-1.dll" . >nul 2>&1

echo.
echo ===== Build Complete! =====
echo Output: %BUILD_DIR%
echo.
echo Executables:
echo   - ChatServer.exe
echo   - ChatClient.exe
echo.
echo To run: run_system.bat
echo.
pause
