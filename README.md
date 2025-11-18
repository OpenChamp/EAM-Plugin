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

The compiled extension will be placed in the `/addons/eam/bin` directory.

## Usage

By default, the plugin will load res://default_assets and user://external to find assets to use.

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

## === Spawn an entity from an XML template manually === ##

# 1. Load Template
var template = EntityTemplates.get_entity_template("cannon_minion")

# 2. Access Template Properties
var entity_id = template["id"]

# 3. Access Template Components
var movement = template.get("_children", {}).get("movement", {})
var stats = template.get("_children", {}).get("stats", {})

# 4. Create new Entity
var minion = minion_scene.instantiate()
minion.name = entity_id

# 5. Apply Relevant Information
if stats.size() > 0:
    minion.max_health = stats.get("max_health", 100)
    minion.health = stats.get("health", 100)
    minion.move_speed = stats.get("move_speed", 5.0)
    # etc...

add_child(minion)
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
