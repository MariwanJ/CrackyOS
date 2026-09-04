#!/usr/bin/env bash
set -e
# Create the build directory
mkdir -p build_l476rg
# Change to the build directory
cd build_l476rg
# Configure the project
cmake .. -GNinja \
    -DCMAKE_BUILD_TYPE=Develop \
    -DMBED_TARGET=NUCLEO_L476RG

# Build the project
ninja -j20
# Go back to the project root
cd ..
# Check that the binary was created
if [[ ! -f build_l476rg/CrackyOS.bin ]]; then
    echo "ERROR: build_l476rg/CrackyOS.bin was not created."
    exit 1
fi
echo "Build successful: build_l476rg/CrackyOS.bin"
# Deploy here, if desired
# ./deploy.sh