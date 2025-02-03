@echo on
setlocal

REM Set the Qt installation directory (Update this if Qt is installed elsewhere)
set QT_DIR=C:\Qt\6.8.1\msvc2022_64

REM Set the build configuration (Debug or Release)
set BUILD_CONFIG=Debug

REM Set the application executable path
set APP_PATH=.\build\%BUILD_CONFIG%\SvgViewer.exe

REM Add Qt bin directory to PATH so Qt DLLs can be found
set PATH=%QT_DIR%\bin;%PATH%

REM Set the platform plugin path for Qt
set QT_QPA_PLATFORM_PLUGIN_PATH=%QT_DIR%\plugins

REM Ensure the executable exists before running
if not exist "%APP_PATH%" (
    echo Error: Executable not found at "%APP_PATH%"
    exit /b 1
)

REM Run the application
echo Running SvgViewer...
"%APP_PATH%"

endlocal
