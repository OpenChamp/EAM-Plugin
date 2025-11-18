#include "register_types.hpp"

#include "identifier.hpp"
#include "dynamic_asset_indexer.hpp"
#include "dynmaic_prefix_handler.hpp"
#include "data_cache_manager.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/engine.hpp>
#include "godot_cpp/classes/resource_loader.hpp"

using namespace godot;

// Register all extension classes with Godot.
void register_external_asset_manager_types() {
	ClassDB::register_class<Identifier>();
	ClassDB::register_class<DynamicAssetIndexer>();
	ClassDB::register_class<DataCacheManager>();
	ClassDB::register_class<DynmaicPrefixHandler>(true);
}

// Initialize module and register singletons.
void initialize_external_asset_manager_module(ModuleInitializationLevel p_level) {
	switch (p_level) {
	case MODULE_INITIALIZATION_LEVEL_CORE:
		register_external_asset_manager_types();
		return;
	case MODULE_INITIALIZATION_LEVEL_SCENE:
		// Register singletons with engine.
		Engine::get_singleton()->register_singleton("AssetIndexer", DynamicAssetIndexer::get_singleton().ptr());
		Engine::get_singleton()->register_singleton("DataCache", DataCacheManager::get_singleton().ptr());

		// Index assets at startup.
		DynamicAssetIndexer::get_singleton()->index_files();
		DataCacheManager::get_singleton()->index_files();

		// Register resource format loader.
		ResourceLoader::get_singleton()->add_resource_format_loader(DynmaicPrefixHandler::get_singleton(), true);
		return;
	}
}


// Cleanup and unregister singletons.
void uninitialize_external_asset_manager_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	Engine::get_singleton()->unregister_singleton("AssetIndexer");
	DynamicAssetIndexer::destory_singleton();

	Engine::get_singleton()->unregister_singleton("DataCache");
	DataCacheManager::destory_singleton();

	ResourceLoader::get_singleton()->remove_resource_format_loader(DynmaicPrefixHandler::get_singleton());
	DynmaicPrefixHandler::destory_singleton();
}


extern "C" {
// Entry point for GDExtension initialization.
GDExtensionBool GDE_EXPORT external_asset_manager_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_external_asset_manager_module);
	init_obj.register_terminator(uninitialize_external_asset_manager_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_CORE);

	return init_obj.init();
}
}
