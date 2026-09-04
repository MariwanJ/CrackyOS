#!/usr/bin/env bash
set -e
# Create the build directory
mkdir -p build_f207zg
# Change to the build directory
cd build_f207zg
# Configure the project
cmake .. -GNinja \
    -DCMAKE_BUILD_TYPE=Develop \
    -DMBED_TARGET=NUCLEO_F207ZG

# Build the project
ninja -j20
# Go back to the project root
cd ..
# Check that the binary was created
if [[ ! -f build_f207zg/CrackyOS.bin ]]; then
    echo "ERROR: build_f207zg/CrackyOS.bin was not created."
    exit 1
fi
echo "Build successful: build_f207zg/CrackyOS.bin"
# Deploy here, if desired
# ./deploy.sh