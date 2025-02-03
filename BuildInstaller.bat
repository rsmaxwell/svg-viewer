@echo off
setlocal


REM Define variables
set Configuration=Debug
set InnoSetupPath=C:\Program Files (x86)\Inno Setup 6
set IssFile=./installer/InstallerSetup.iss
set QtPlatformPath=C:\Qt\6.8.1\msvc2022_64

REM Run the installer builder and capture the exit code
call "%InnoSetupPath%\iscc" "/DqtPlatformPath=%QtPlatformPath%" "/Dconfig=%Configuration%" "%IssFile%"

REM Check the return code
if %errorlevel% neq 0 (
    echo ERROR: The installer build failed with exit code %errorlevel%.
    exit /b %errorlevel%
) else (
    echo SUCCESS: The installer was built successfully.
)

endlocal
