@echo off
REM Build script for Windows using Visual Studio 2022

echo Building OpenGL Learning Project with Visual Studio 2022...
echo.

REM Configure with Visual Studio 2022
echo Configuring...
cmake -B build-msvc -S . -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: CMake configuration failed!
    echo Make sure Visual Studio 2022 Build Tools are installed.
    echo Install with: winget install Microsoft.VisualStudio.2022.BuildTools
    echo.
    pause
    exit /b 1
)

echo.
echo Building Release version...
cmake --build build-msvc --config Release
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed!
    echo.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build complete!
echo Executable: build-msvc\bin\Release\OpenGLLearning.exe
echo ========================================
echo.
pause
