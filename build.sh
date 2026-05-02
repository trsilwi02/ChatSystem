#!/bin/bash
# Build script for Chat System - Linux/macOS

set -e

echo "Chat System Build Script"
echo "========================"

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Configure CMake
echo "Configuring CMake..."
if [ "$(uname)" == "Darwin" ]; then
    # macOS
    cmake -G "Unix Makefiles" \
          -DCMAKE_BUILD_TYPE=Release \
          -DQt6_DIR=$(brew --prefix qt)/lib/cmake/Qt6 \
          ..
else
    # Linux
    cmake -G "Unix Makefiles" \
          -DCMAKE_BUILD_TYPE=Release \
          ..
fi

# Build
echo "Building..."
cmake --build . --config Release

echo ""
echo "Build complete!"
echo "Executables:"
echo "  - Server: ./ChatServer"
echo "  - Client: ./ChatClient"
