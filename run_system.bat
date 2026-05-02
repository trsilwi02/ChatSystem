@echo off
REM Chat System Launcher - Starts Server and Multiple Clients

cd /d D:\ChatSystem\build

REM Set Qt plugin path
set QT_PLUGIN_PATH=%CD%\plugins

REM Check if server is already running
tasklist /FI "IMAGENAME eq ChatServer.exe" 2>NUL | find /I /N "ChatServer.exe">NUL
if "%ERRORLEVEL%"=="0" (
    echo ChatServer is already running
) else (
    echo Starting ChatServer...
    start "" ChatServer.exe
    timeout /t 2 /nobreak
)

echo Starting ChatClient #1...
start "" ChatClient.exe

echo Starting ChatClient #2 in 2 seconds...
timeout /t 2 /nobreak
start "" ChatClient.exe

echo.
echo System running!
echo - ChatServer: localhost:8888
echo - Register users and start chatting
echo.
pause
