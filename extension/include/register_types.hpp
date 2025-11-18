#pragma once

#include <base_include.hpp>

// Register all extension classes with Godot.
GDE_EXPORT void register_external_asset_manager_types();

// Initialize module singletons and loaders.
GDE_EXPORT void initialize_external_asset_manager_module(godot::ModuleInitializationLevel p_level);

// Cleanup and unregister singletons.
GDE_EXPORT void uninitialize_external_asset_manager_module(godot::ModuleInitializationLevel p_level);
