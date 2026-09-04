#First we create the build directory
mkdir -p build;
#change directory to the created folder
cd build;
#Run cmake to configure the project and compile the project after that
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Develop -DMBED_TARGET=NUCLEO_F767ZI && ninja -j20;
#Deoply the binary to the MCU

REM Deploy the binary to the MCU
for %%F in (*.bin) do (
    echo Flashing %%F
    C:\Users\mariwanj\AppData\Local\stm32cube\bundles\programmer\2.23.0\bin\STM32_Programmer_CLI.exe -c port=SWD -w "%%F" 0x08000000 -v -rst
    goto :flash_done
)

REM Go back to the root directory
cd ..