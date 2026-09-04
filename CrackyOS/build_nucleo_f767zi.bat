REM  Create the build directory to save our object files and the bin/hex file
mkdir build_f767zi
REM Change directory to the created build directory
cd build_f767zi
REM Run cmake command that configure the project: Change your target dependently. Do not use '-', you should use '_'
cmake .. -GNinja  -DCMAKE_BUILD_TYPE=Develop -DMBED_TARGET=NUCLEO_F767ZI
if errorlevel 1 exit /b 1
REM Run 'ninja' to start compiling the project
ninja -j20
if errorlevel 1 exit /b 1
REM  go back to the root directory
cd ..
if not exist build_f767zi\CrackyOS.bin exit /b 1
REM -Remove blow comments if you wan to deploy also
REM call deploy.bat
REM if errorlevel 1 exit /b 1