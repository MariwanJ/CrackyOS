REM  Create the build directory to save our object files and the bin/hex file
mkdir build_l476rg
REM Change directory to the created build directory
cd build_l476rg
REM Run cmake command that configure the project: Change your target dependently. Do not use '-', you should use '_'
cmake .. -GNinja  -DCMAKE_BUILD_TYPE=Develop -DMBED_TARGET=NUCLEO_L476RG
if errorlevel 1 exit /b 1
REM Run 'ninja' to start compiling the project
ninja -j20
if errorlevel 1 exit /b 1
REM  go back to the root directory
cd ..
if not exist build_l476rg\CrackyOS.bin exit /b 1
REM -Remove blow comments if you wan to deploy also
REM call deploy.bat
REM if errorlevel 1 exit /b 1