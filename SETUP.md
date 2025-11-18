# Setting up Index Manager as a Standalone Repository

The index_manager addon has been extracted into a standalone folder structure to prepare it as its own repository. This document outlines the setup and next steps.

## Folder Structure

```
index_manager/
├── addons/
│   └── index_manager/              # Godot plugin files
│       ├── plugin.cfg
│       ├── index_manager_plugin.gd
│       ├── index_manager_dock.gd
│       └── index_manager_dock.tscn
├── extensions/
│   ├── include/                    # C++ header files
│   │   ├── base_include.hpp
│   │   ├── identifier.hpp
│   │   ├── dynamic_asset_indexer.hpp
│   │   ├── data_cache_manager.hpp
│   │   ├── dynmaic_prefix_handler.hpp
│   │   └── register_types.hpp
│   └── src/                        # C++ source files
│       ├── identifier.cpp
│       ├── indexing_functions.cpp
│       ├── dynamic_asset_indexer.cpp
│       ├── data_cache_manager.cpp
│       ├── dynmaic_prefix_handler.cpp
│       └── register_types.cpp
├── CMakeLists.txt                  # Build configuration
├── README.md                        # Project documentation
├── LICENSE                          # License file
└── .gitignore                       # Git ignore rules
```

## Prerequisites for Building

To compile the C++ extension independently, you'll need to set up the godot-cpp submodule in the extensions directory:

```bash
cd index_manager/extensions
git clone https://github.com/godotengine/godot-cpp.git
cd godot-cpp
git checkout 4.3  # or appropriate version
```

Alternatively, you can use a git submodule reference:

```bash
git submodule add https://github.com/godotengine/godot-cpp.git eam-plugin/extensions/godot-cpp
```

## Building Instructions

### Linux/Mac
```bash
cd index_manager
mkdir -p build
cd build
cmake .. -G Ninja
ninja
```

### Windows (with MSVC)
```bash
cd index_manager
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

## Integration with Main Project

If you want to keep this as a separate submodule in the main OpenChamp client project:

1. Remove the old addons/index_manager folder
2. Add this repository as a submodule:
   ```bash
   git submodule add <repo-url> index_manager
   ```

3. Update your project.godot to point to the new addon location in the submodule

## Next Steps to Create a Standalone Repository

1. **Initialize Git Repository**
   ```bash
   cd index_manager
   git init
   git add .
   git commit -m "Initial commit: Extract index_manager as standalone project"
   ```

2. **Create GitHub Repository**
   - Create a new repository on GitHub (e.g., `openchamp/index_manager`)
   - Add as remote and push

3. **Add Required CI/CD**
   - Add GitHub Actions for automated builds (Linux, macOS, Windows)
   - Configure release artifacts to build compiled extensions

4. **Setup Release Pipeline**
   - Create pre-release build artifacts for each platform
   - Document versioning strategy

5. **Documentation**
   - Add additional docs/ folder with API documentation
   - Create CONTRIBUTING.md for contribution guidelines
   - Add examples/ folder with usage examples

## Module Symbol Updates

Note: The C++ module uses the symbol `index_manager_library_init` (defined in register_types.cpp) instead of `openchamp_library_init`. This allows the extension to be compiled and used independently.

When creating a .gdextension file for this project, use:

```ini
[configuration]
entry_symbol = "index_manager_library_init"
compatibility_minimum = "4.3"

[libraries]
linux.debug.x86_64 = "res://bin/libindex_manager.linux.template_debug.x86_64.so"
linux.release.x86_64 = "res://bin/libindex_manager.linux.template_release.x86_64.so"
windows.debug.x86_64 = "res://bin/libindex_manager.windows.template_debug.amd64.dll"
windows.release.x86_64 = "res://bin/libindex_manager.windows.template_release.amd64.dll"
macos.debug = "res://bin/libindex_manager.darwin.template_debug.arm64.dylib"
macos.release = "res://bin/libindex_manager.darwin.template_release.arm64.dylib"
```

## Important Notes

- The `Identifier`, `DynamicAssetIndexer`, `DataCacheManager`, and `DynmaicPrefixHandler` classes are registered as global singletons in the `AssetIndexer` and `DataCache` names
- The plugin requires the C++ extension to be compiled and available in the bin/ directory
- Asset packs should follow the defined structure with asset groups and asset types
- The system supports both default assets (res://default_assets) and external packs (user://external)
