#!/usr/bin/env bash
set -e
# Create the build directory
mkdir -p build_l4r5zi
# Change to the build directory
cd build_l4r5zi
# Configure the project
cmake .. -GNinja \
    -DCMAKE_BUILD_TYPE=Develop \
    -DMBED_TARGET=NUCLEO_L4R5ZI

# Build the project
ninja -j20
# Go back to the project root
cd ..
# Check that the binary was created
if [[ ! -f build_l4r5zi/CrackyOS.bin ]]; then
    echo "ERROR: build_l4r5zi/CrackyOS.bin was not created."
    exit 1
fi
echo "Build successful: build_l4r5zi/CrackyOS.bin"
# Deploy here, if desired
# ./deploy.sh