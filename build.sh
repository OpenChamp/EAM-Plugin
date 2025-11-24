#!/bin/bash
# Build script for macOS and Linux
# Usage: ./build.sh [clean] [release|debug] [jobs]

set -e

BUILD_TYPE="Release"
CLEAN_BUILD=0
JOBS=${3:-$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 4)}

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	PLATFORM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
	PLATFORM="macos"
else
	echo "Unsupported platform: $OSTYPE"
	exit 1
fi

BUILD_DIR="build_${PLATFORM}"

# Parse arguments
while [[ $# -gt 0 ]]; do
	case $1 in
		clean)
			CLEAN_BUILD=1
			shift
			;;
		debug)
			BUILD_TYPE="Debug"
			shift
			;;
		release)
			BUILD_TYPE="Release"
			shift
			;;
		*)
			JOBS=$1
			shift
			;;
	esac
done

if [ $CLEAN_BUILD -eq 1 ] && [ -d "$BUILD_DIR" ]; then
	echo "Cleaning build directory..."
	rm -rf "$BUILD_DIR"
fi

if [ ! -d "$BUILD_DIR" ]; then
	echo "Creating build directory..."
	mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Create .gdignore file to exclude from Godot
echo "" > .gdignore

echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

echo "Building extension with $JOBS jobs..."
cmake --build . --config $BUILD_TYPE -- -j$JOBS

echo "Build completed successfully!"
echo "Binary located in: addons/eam/bin/"
