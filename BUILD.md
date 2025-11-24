# Cross-Platform Build Guide

## Overview
This project uses CMake to compile a GDExtension that works on Windows, macOS, and Linux.

## Prerequisites

### Windows
- Visual Studio 2022 (with C++ build tools)
- CMake 3.13+
- Python 3.8+ (for SCons if needed)

### macOS
- Xcode Command Line Tools: `xcode-select --install`
- CMake: `brew install cmake`
- Python 3.8+

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git python3
```

### Linux (Fedora/RHEL)
```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git python3
```

## Building

### Windows
```bash
build.bat [clean] [release|debug]
```

Example:
```bash
build.bat clean release  # Clean build in Release mode
build.bat debug          # Debug build (keeps cache)
```

### macOS/Linux
```bash
./build.sh [clean] [release|debug] [jobs]
```

Example:
```bash
./build.sh clean release 8     # Clean build in Release mode with 8 parallel jobs
./build.sh debug               # Debug build with default jobs
```

## WSL2 Compilation Optimization

If using WSL2 on Windows, compilation of godot-cpp can be very slow. Here are optimization strategies:

### Option 1: Pre-cache godot-cpp (Recommended)
Build once fully, then subsequent builds reuse the cache:

```bash
cd extension/godot-cpp
cmake --build . -j$(nproc)
cd ../..
./build.sh clean release
```

### Option 2: Use ccache for faster rebuilds
```bash
sudo apt-get install ccache
export CC="ccache gcc"
export CXX="ccache g++"
./build.sh clean release
```

### Option 3: Use WSL2 with faster filesystem
Place the project on the native WSL2 filesystem (`/home/user/project`) rather than mounted Windows drives (`/mnt/c/`) for significantly faster builds.

### Option 4: Use Ninja instead of Make
```bash
sudo apt-get install ninja-build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

## Line Endings

The `.gitattributes` file automatically normalizes line endings:
- C++, CMake, and shell scripts use **LF** (Unix)
- Windows batch files use **CRLF** (Windows)

The CMakeLists.txt also includes automatic line ending normalization for `pugixml.cpp` when building on Unix platforms.

## Binary Output

After building, the compiled extension appears in: `addons/eam/bin/`

Naming format: `external_asset_manager.{OS}.template_{BUILD_TYPE}.{ARCH}`

Examples:
- `external_asset_manager.windows.template_release.x86_64.dll`
- `external_asset_manager.linux.template_release.x86_64.so`
- `external_asset_manager.macos.template_release.x86_64.dylib`

## Troubleshooting

### "pugixml.cpp: No such file or directory" on Linux/macOS
This usually means line ending issues. The CMakeLists.txt now auto-fixes this. If it persists:
```bash
dos2unix extension/include/pugixml.cpp
# OR
sed -i 's/\r$//' extension/include/pugixml.cpp
```

### Slow WSL2 builds
See "WSL2 Compilation Optimization" above. Using the native WSL2 filesystem provides 10-20x speedup.

### "compiler not found" on macOS
Install Xcode Command Line Tools:
```bash
xcode-select --install
```

### CMake configuration fails with godot-cpp
Ensure godot-cpp submodule is initialized:
```bash
git submodule update --init --recursive
```
