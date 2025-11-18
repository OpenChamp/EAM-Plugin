# External Asset Manager

The External Asset Manager is a Godot plugin for the OpenChamp project that manages the dynamic asset system. It provides tools to index assets and cache data for efficient resource loading.

## Features

- Dynamic asset indexing from multiple asset packs
- Asset caching with hash-based management
- Support for multiple asset types (textures, fonts, materials, models, etc.)
- Custom resource loader for dynamic asset paths
- Integrated Godot editor plugin with UI for asset management

### C++ Extension (`extensions/`)

#### Core Classes
- **Identifier**: Manages asset identification with group:name format
- **DynamicAssetIndexer**: Indexes asset files from multiple asset packs
- **DataCacheManager**: Manages hashed data caching for assets
- **DynmaicPrefixHandler**: Custom resource loader for dynamic asset prefixes

## Building

### Requirements
- CMake 3.13+
- Ninja or Make
- C++17 compatible compiler
- Godot C++ bindings

### Build Steps

```bash
cd index_manager
mkdir build && cd build
cmake .. -G Ninja
ninja
```

The compiled extension will be placed in the `bin/` directory.

## Usage

### GDScript API

```gdscript
# Access the global indexer singleton
AssetIndexer.index_files()
var assets = AssetIndexer.get_asset_map()
var path = AssetIndexer.get_asset_path(identifier)

# Access the global cache singleton
DataCache.cache_file("path/to/file.json")
var cached = DataCache.get_cached_json("hash")

# Work with identifiers
var id = Identifier.from_string("group:name")
var id = Identifier.from_values("group", "name")
var prefix = Identifier.get_resource_prefix_from_type("textures")
```

## Asset Structure

Assets should be organized in the following structure:

```
asset_pack/
├── asset_group/
│   ├── textures/
│   ├── fonts/
│   ├── models/
│   ├── materials/
│   ├── shaders/
│   ├── styles/
│   ├── audio/
│   ├── maps/
│   ├── units/
│   ├── patchdata/
│   │   └── gamemode_name/
│   │       ├── manifest.json
│   │       ├── characters/
│   │       ├── units/
│   │       ├── items/
│   │       ├── misc/
│   │       └── map/
│   └── lang/
└── ...
```

## License

This project is licensed under the GNU General Public License v3.0 - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please ensure your code follows the existing style and includes appropriate documentation.
