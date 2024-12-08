@echo off
setlocal

set BULLET_REPO=https://github.com/bulletphysics/bullet3.git
set BULLET_DIR=%cd%\thirdparty\bullet

if exist "%BULLET_DIR%" (
    echo Bullet is already cloned. Skipping download.
) else (
    echo Cloning Bullet repository...
    git clone %BULLET_REPO% %BULLET_DIR%
)

:: Build Bullet using CMake
echo Configuring Bullet with CMake...
cd %BULLET_DIR%

if not exist "build" mkdir build
cd build

:: Run CMake to configure Bullet for Visual Studio
cmake -G "Visual Studio 16 2019" -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%BULLET_DIR%\install ..

cmake --build . --config Release --target INSTALL

echo Bullet installation completed!

cd %cd%

endlocal
pause

