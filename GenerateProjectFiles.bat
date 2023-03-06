@ECHO OFF

PUSHD %~dp0

SET VSWHERE="%~dp0\tools\vswhere\vswhere.exe"
SET CMAKE="cmake"

REM Detect latest version of Visual Studio.
FOR /F "usebackq delims=." %%i IN (`%VSWHERE% -latest -prerelease -requires Microsoft.VisualStudio.Workload.NativeGame -property installationVersion`) DO (
    SET VS_VERSION=%%i
)

IF %VS_VERSION% == 17 (
    SET CMAKE_GENERATOR="Visual Studio 17 2022"
    SET CMAKE_BINARY_DIR=VS2022CL
) ELSE IF %VS_VERSION% == 16 (
    SET CMAKE_GENERATOR="Visual Studio 16 2019"
    SET CMAKE_BINARY_DIR=VS2019CL
) ELSE IF %VS_VERSION% == 15 (
    SET CMAKE_GENERATOR="Visual Studio 15 2017"
    SET CMAKE_BINARY_DIR=VS2017CL
) ELSE IF %VS_VERSION% == 14 (
    SET CMAKE_GENERATOR="Visual Studio 14 2015"
    SET CMAKE_BINARY_DIR=VS2015CL
) ELSE (
    ECHO.
    ECHO ***********************************************************************
    ECHO *                                                                     *
    ECHO *                                ERROR                                *
    ECHO *                                                                     *
    ECHO ***********************************************************************
    ECHO No compatible version of Microsoft Visual Studio detected.
    ECHO Please make sure you have Visual Studio 2015 ^(or newer^) and the 
    ECHO "Game Development with C++" workload installed before running this script.
    ECHO. 
    PAUSE
    GOTO :Exit
)

ECHO CMake Generator: %CMAKE_GENERATOR%
ECHO CMake Binary Directory: %CMAKE_BINARY_DIR%
ECHO.

MKDIR %CMAKE_BINARY_DIR% 2>NUL
XCOPY /Y ".clang-format" "%CMAKE_BINARY_DIR%\"
PUSHD %CMAKE_BINARY_DIR%

%CMAKE% -DCMAKE_BUILD_TYPE=DEBUG -G %CMAKE_GENERATOR% -A x64 "%~dp0"

IF %ERRORLEVEL% NEQ 0 (
    PAUSE
) ELSE (
    @REM START TopiaEngine.sln
)

POPD

:Exit

POPD
