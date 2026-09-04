cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19...3.22)

#### Initialize Mbed OS build system.

####################################################################################################
### Block of including .json5 files.
### Files of this block must be included before the app.cmake.

#[[ Set path of mbed_app.json (necessary at all times) ]]
set(MBED_APP_JSON_PATH mbed_app.json5)

###-------------------------------------------------------------------------------------------------
#[[ This part is dedicated for custom targets only! The settings below activate targets from
custom_targets.json5 and upload method config. If custom targets are not used the lines below
should be commented. ]]

#[[ Here set path for custom_targets.json5 (this is our default) ]]
#set(CUSTOM_TARGETS_JSON_PATH custom_targets/custom_targets.json5)

#[[ Here you can set path for custom upload config .cmake (optional example) ]]
#set(CUSTOM_UPLOAD_CFG_PATH ${CMAKE_SOURCE_DIR}/${MBED_TARGET}/${MBED_TARGET}.cmake)

#[[ Note: For custom target you need also an upload method and we have few options how you can do
that:
- use the variable CUSTOM_UPLOAD_CFG_PATH above to set the location of your config file
- use the default expected path for custom targets upload method config file:
MY_PROJECT/custom_targets/upload_method_cfg
- of course you can set upload method parameters directly via cmake in this file
For more visit https://github.com/mbed-ce/mbed-os/wiki/Upload-Methods ]]

### End of block
####################################################################################################
### Include Mbed toolchain setup file

include(mbed-os/tools/cmake/mbed_toolchain_setup.cmake)

####################################################################################################
### Derive board variant from MBED_TARGET
#
# Example:
# MBED_TARGET=NUCLEO_F767ZI
# BOARD_VARIANT=f767zi
#
# The part after the last '_' is extracted and converted to lowercase.
####################################################################################################

if(NOT DEFINED MBED_TARGET OR MBED_TARGET STREQUAL "")
message(FATAL_ERROR
"MBED_TARGET must be specified, for example: "
"-DMBED_TARGET=NUCLEO_F767ZI"
)
endif()

string(REGEX REPLACE "^.*_" "" BOARD_VARIANT "${MBED_TARGET}")
string(TOLOWER "${BOARD_VARIANT}" BOARD_VARIANT)

message(STATUS "MBED_TARGET   = ${MBED_TARGET}")
message(STATUS "BOARD_VARIANT = ${BOARD_VARIANT}")

######################################################################################################
### Set up your project
######################################################################################################

project(CrackyOS
# VERSION 1.0.0
LANGUAGES C CXX ASM)

### Include Mbed project setup file

include(mbed_project_setup)

######################################################################################################
### Block of including project folders
#[[ If using a custom target, the subdirectory containing the custom target must be included before
the mbed-os subdir, otherwise the next line should be commented ]]
#add_subdirectory(custom_targets)
###---------------------------------------------------------------------------------------------------
## Add mbed-os subdirectory (necessary every time)

add_subdirectory(mbed-os)

###---------------------------------------------------------------------------------------------------
## Add another subdirectory, for example subdirectory of a library (if needed)
#add_subdirectory(YourLibrary)
### End of block
######################################################################################################
######################################################################################################
### Application sources
######################################################################################################
# Collect common application sources.

file(GLOB APP_SOURCES CONFIGURE_DEPENDS
${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# Add the hardware implementation for the selected board.
#
# Example:
# MBED_TARGET=NUCLEO_F767ZI
# BOARD_VARIANT=f767zi
# -> src/f767zi/nucleo_f767zi_hw.cpp
#

set(BOARD_HW_SOURCE
${CMAKE_CURRENT_SOURCE_DIR}/src/${BOARD_VARIANT}/nucleo_${BOARD_VARIANT}_hw.cpp
)

# Make sure the board-specific source actually exists.

if(NOT EXISTS "${BOARD_HW_SOURCE}")
message(FATAL_ERROR
"Board-specific hardware source not found:\n"
"  ${BOARD_HW_SOURCE}\n"
"MBED_TARGET   = ${MBED_TARGET}\n"
"BOARD_VARIANT = ${BOARD_VARIANT}"
)
endif()

message(STATUS "Board HW source = ${BOARD_HW_SOURCE}")

list(APPEND APP_SOURCES
${BOARD_HW_SOURCE}
)

######################################################################################################
### Create executable
######################################################################################################

add_executable(${PROJECT_NAME} ${APP_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/src
)

######################################################################################################
### Board-specific compile definition
######################################################################################################

string(TOUPPER "${BOARD_VARIANT}" BOARD_VARIANT_UPPER)

target_compile_definitions(${PROJECT_NAME} PRIVATE
TARGET_NUCLEO_${BOARD_VARIANT_UPPER}
)

message(STATUS "Board compile definition = TARGET_NUCLEO_${BOARD_VARIANT_UPPER}")

######################################################################################################
### Link libraries
######################################################################################################
#[[ For more about this configuration visit wiki page MbedOS-configuration
https://github.com/mbed-ce/mbed-os/wiki/MbedOS-configuration#configuration-via-cmake-files ]]
# Link Mbed OS core library.
# This is called mbed-os when using the Mbed RTOS, and mbed-baremetal when using
# the bare metal version. This if statement checks to see which is in use and links it.
#
# Important: The mbed-os and mbed-baremetal libraries are object libraries, so they should ONLY be
# linked to executables, not to other libraries. Libraries should link to mbed-core-flags instead.

if("MBED_CONF_TARGET_APPLICATION_PROFILE=bare-metal" IN_LIST MBED_CONFIG_DEFINITIONS)
target_link_libraries(${PROJECT_NAME}
mbed-baremetal
mbed-storage-sd
mbed-storage-fat
)
else()
target_link_libraries(${PROJECT_NAME}
mbed-os
mbed-storage-sd
mbed-storage-fat
)
endif()

### Link user library (if needed)
#target_link_libraries(${PROJECT_NAME} YourLibrary)
######################################################################################################
### Post-build
######################################################################################################

# This generates build rules to generate bin/hex files, and to flash the code if you have configured
# an upload method.

mbed_set_post_build(${PROJECT_NAME})

#[[ Note: if you wish to use a custom linker script instead of the default Mbed one for
your target, you can do that with:
mbed_set_post_build(${PROJECT_NAME} path/to/linker_script.ld)
]]
