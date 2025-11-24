@echo off
REM Build script for Windows (MSVC)
REM Usage: build.bat [clean] [release|debug]

setlocal enabledelayedexpansion
set BUILD_TYPE=Release
set CLEAN_BUILD=0
set BUILD_DIR=build_windows

if "%1"=="clean" (
	set CLEAN_BUILD=1
	shift
)

if "%1"=="debug" set BUILD_TYPE=Debug
if "%1"=="release" set BUILD_TYPE=Release

if exist %BUILD_DIR% (
	if %CLEAN_BUILD%==1 (
		echo Cleaning build directory...
		rmdir /s /q %BUILD_DIR%
	)
)

if not exist %BUILD_DIR% (
	echo Creating build directory...
	mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if errorlevel 1 (
	echo CMake configuration failed!
	exit /b 1
)

echo Building extension...
cmake --build . --config %BUILD_TYPE%

if errorlevel 1 (
	echo Build failed!
	exit /b 1
)

echo Build completed successfully!
echo Binary located in: addons\eam\bin\
