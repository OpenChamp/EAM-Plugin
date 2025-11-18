#include "identifier.hpp"
#include "data_cache_manager.hpp"
#include "xml_loader.hpp"

#include <gdextension_interface.h>

#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/translation.hpp>
#include <godot_cpp/classes/translation_server.hpp>

using namespace godot;

// Load translation from JSON data for given locale.
static inline Ref<Translation> _load_translation_from_json(JSON* lang_data, String locale){
	Ref<Translation> translation;
	translation.instantiate();

	translation->set_locale(locale);

	if (lang_data == nullptr){
		return translation;
	}

	if (lang_data->get_data().get_type() != Variant::DICTIONARY){
		return translation;
	}

	Dictionary lang_data_dict = static_cast<Dictionary>(lang_data->get_data());

	auto keys = lang_data_dict.keys();
	for (int64_t i = 0; i < keys.size(); i++){
		String key = keys[i];
		String value = lang_data_dict[key];

		translation->add_message(key, value);
	}

	return translation;
}


// Index and load language files from directory.
static inline void _load_lang_files(String pack_path, String asset_group, HashMap<String, String>& asset_map){
	auto lang_dir = DirAccess::open(pack_path + "/" + asset_group + "/lang");
	if (lang_dir == nullptr){
		UtilityFunctions::print("Failed to open lang directory for " + asset_group + " in " + pack_path);
		return;
	}

	lang_dir->list_dir_begin();
	String lang_file = "";

	while ((lang_file = lang_dir->get_next()) != ""){
		if (lang_dir->current_is_dir()){
			continue;
		}

		if (!lang_file.ends_with(".json")){
			continue;
		}

		String lang_path = pack_path + "/" + asset_group + "/lang/" + lang_file;
		String locale = lang_file.substr(0, lang_file.length() - 5);

		Ref<Resource> raw_lang_data = ResourceLoader::get_singleton()->load(lang_path);
		if (raw_lang_data == nullptr || !raw_lang_data.is_valid()){
			UtilityFunctions::print("Failed to load lang file: " + lang_path);
			continue;
		}

		if (!raw_lang_data->is_class("JSON")){
			UtilityFunctions::print("Lang file is not a JSON file: " + lang_path);
			continue;
		}

		auto lang_data = Object::cast_to<JSON>(raw_lang_data.ptr());
		if (lang_data == nullptr){
			UtilityFunctions::print("Failed to cast lang data to JSON: " + lang_path);
			continue;
		}

		auto new_translation = _load_translation_from_json(lang_data, locale);
		TranslationServer::get_singleton()->add_translation(new_translation);
	}
}


// Index all font files in directory.
static inline void _index_fonts(String pack_path, String asset_group, HashMap<String, String>& asset_map){
	auto font_dir = DirAccess::open(pack_path + "/" + asset_group + "/fonts");
	if (font_dir == nullptr){
		UtilityFunctions::print("Failed to open font directory");
		return;
	}
	
	UtilityFunctions::print("loading fonts for " + pack_path + "/" + asset_group);

	font_dir->list_dir_begin();
	String font_name = "";
	while ((font_name = font_dir->get_next()) != ""){
		if (font_dir->current_is_dir()){
			continue;
		}

		if (font_name.ends_with(".import")){
			font_name = font_name.substr(0, font_name.length() - 7);
		}

		// Index font.
		String font_path = pack_path + "/" + asset_group + "/fonts/" + font_name;
		String font_basename = font_name.get_basename();
		
		Ref<Identifier> font_id = Identifier::from_values(asset_group, "fonts/" + font_basename);

		asset_map[font_id->to_string()] = font_path;
		UtilityFunctions::print("Indexed font: " + font_id->to_string());
		
	}
}


// Cache patch data files for gamemodes.
// Iterates through all gamemode folders and patch types.
static inline void _cache_patch_data(String pack_path, String asset_group, HashMap<String, String>& asset_map){
	auto patches_dir = DirAccess::open(pack_path + "/" + asset_group + "/patchdata");
	if (patches_dir == nullptr){
		UtilityFunctions::print("Failed to open patchdata directory");
		return;
	}

	patches_dir->list_dir_begin();
	String gamemode_name = "";

	while ((gamemode_name = patches_dir->get_next()) != ""){
		if (!patches_dir->current_is_dir()){
			continue;
		}

		UtilityFunctions::print("Caching patch data for gamemode: " + gamemode_name);

		String manifest_path = pack_path + "/" + asset_group + "/patchdata/" + gamemode_name + "/manifest.json";
		Ref<Identifier> manifest_id = Identifier::for_resource("gamemode://" + asset_group + ":" + gamemode_name);
		if (manifest_id == nullptr){
			UtilityFunctions::print("Failed to create manifest identifier");
			continue;
		}
		asset_map[manifest_id->to_string()] = manifest_path;

		Vector<String> patch_types = {"characters", "units", "items", "misc", "map"};

		for (String patch_type : patch_types){

			auto gamemode_dir = DirAccess::open(pack_path + "/" + asset_group + "/patchdata/" + gamemode_name + "/" + patch_type);
			if (gamemode_dir == nullptr){
				UtilityFunctions::print("Failed to open gamemode directory: " + gamemode_name);
				continue;
			}

			gamemode_dir->list_dir_begin();
			String patch_file = "";
			while ((patch_file = gamemode_dir->get_next()) != ""){
				if (gamemode_dir->current_is_dir()){
					continue;
				}

				if (!patch_file.ends_with(".json")){
					continue;
				}

				String patch_path = pack_path + "/" + asset_group + "/patchdata/" + gamemode_name + "/" + patch_type + "/" + patch_file;
				String file_hash = DataCacheManager::get_singleton()->cache_file(patch_path);
				
				UtilityFunctions::print("Cached file '" + patch_path + "' with hash: " + file_hash);
			}
		}
	}
}

// Load and cache XML entity data files.
static inline void _load_entity_data(String pack_path, String asset_group, HashMap<String, String>& asset_map){
	auto entities_dir = DirAccess::open(pack_path + "/" + asset_group + "/entities");
	if (entities_dir == nullptr){
		UtilityFunctions::print("Failed to open entities directory");
		return;
	}

	entities_dir->list_dir_begin();
	String entity_file = "";

	while ((entity_file = entities_dir->get_next()) != ""){
		if (entities_dir->current_is_dir()){
			continue;
		}

		if (!entity_file.ends_with(".xml")){
			continue;
		}

		String entity_path = pack_path + "/" + asset_group + "/entities/" + entity_file;
		
		// Load and parse XML file
		XmlDocument doc = XmlLoader::load_xml_file(entity_path.utf8().get_data());
		if (!doc.is_valid){
			UtilityFunctions::push_error("Failed to load entity XML: ", entity_path);
			continue;
		}

		if (!doc.root){
			UtilityFunctions::push_error("Entity XML has no root node: ", entity_path);
			continue;
		}

		// Get entity ID from root element
		XmlAttribute id_attr = doc.root->get_attribute("id");
		if (id_attr.string_value.empty()){
			UtilityFunctions::push_error("Entity XML missing id attribute: ", entity_path);
			continue;
		}

		// Cache the entire entity file
		String file_hash = DataCacheManager::get_singleton()->cache_file(entity_path);
		UtilityFunctions::print("Cached entity file '" + entity_path + "' with ID: " + String(id_attr.string_value.c_str()) + " hash: " + file_hash);
	}
}


// Recursively index resources of given type.
// Handles nested directories and file filtering.
static inline void _index_resources(
	String pack_path,
	String asset_group,
	HashMap<String, String>& asset_map,
	String resource_type,
	String resource_subdir
){
	auto resource_dir = DirAccess::open(pack_path + "/" + asset_group + "/" + resource_subdir);
	if (resource_dir == nullptr){
		UtilityFunctions::push_error("Failed to open " + resource_type + " directory");
		return;
	}
	
	UtilityFunctions::print("loading " + resource_type + " for " + pack_path + "/" + asset_group + "/" + resource_subdir);

	resource_dir->list_dir_begin();
	String resource_name = "";
	while ((resource_name = resource_dir->get_next()) != ""){
		if (resource_dir->current_is_dir()){
			_index_resources(pack_path, asset_group, asset_map, resource_type, resource_subdir + "/" + resource_name);
		}else{

			if (resource_name.ends_with(".bin")){
				continue;
			}
			
			if (resource_name.ends_with(".import")){
				resource_name = resource_name.substr(0, resource_name.length() - 7);
			}

			// Load/Index Texture.
			String resource_path = pack_path + "/" + asset_group + "/" + resource_subdir + "/" + resource_name;
			String resource_basename = resource_name.get_basename();
			
			Ref<Identifier> resource_id = Identifier::from_values(asset_group, resource_subdir + "/" + resource_basename);

			asset_map[resource_id->to_string()] = resource_path;
			UtilityFunctions::print("Indexed " + resource_type + ": " + resource_id->to_string());
			UtilityFunctions::print(" at path: " + resource_path);
		}
	}
}


// Index all asset types within a group.
static inline void _index_asset_group(String pack_path, String asset_group, HashMap<String, String>& asset_map){
	auto group_dir = DirAccess::open(pack_path + "/" + asset_group);
	if (group_dir == nullptr){
		UtilityFunctions::push_error("Failed to open group directory: " + asset_group);
		return;
	}

	group_dir->list_dir_begin();
	String asset_type = group_dir->get_next();

	while (asset_type != ""){
        UtilityFunctions::print("Indexing asset type: " + asset_type + " in " + pack_path + "/" + asset_group);
		if (group_dir->current_is_dir()){
			// Use different function depending on asset type.
			if (asset_type == "lang"){
				_load_lang_files(pack_path, asset_group, asset_map);
			}else if (asset_type == "fonts"){
				_index_fonts(pack_path, asset_group, asset_map);
			}else if (asset_type == "patchdata"){
				_cache_patch_data(pack_path, asset_group, asset_map);
			}else if (asset_type == "entities"){
				_load_entity_data(pack_path, asset_group, asset_map);
			}else{
				_index_resources(pack_path, asset_group, asset_map, asset_type, asset_type);
			}
		}

		asset_type = group_dir->get_next();
	}
}


// Index all asset groups within a pack.
static inline void _index_asset_pack(String pack_path, HashMap<String, String>& asset_map){
	auto pack_dir = DirAccess::open(pack_path);
	if (pack_dir == nullptr){
		UtilityFunctions::push_error("Failed to open pack directory: " + pack_path);
		return;
	}

	pack_dir->list_dir_begin();
	String asset_group = pack_dir->get_next();

	while (asset_group != ""){
        UtilityFunctions::print("Indexing asset group: " + asset_group + " in " + pack_path);

		if (pack_dir->current_is_dir()){
			_index_asset_group(pack_path, asset_group, asset_map);
		}
			
		asset_group = pack_dir->get_next();
	}
}
