#include "entity_template_manager.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>

using namespace godot;

void EntityTemplateManager::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_entity_template", "entity_name"), &EntityTemplateManager::get_entity_template);
	ClassDB::bind_method(D_METHOD("get_available_entities"), &EntityTemplateManager::get_available_entities);
	ClassDB::bind_method(D_METHOD("dump_templates"), &EntityTemplateManager::dump_templates);
}

Ref<EntityTemplateManager> EntityTemplateManager::_EntityTemplateManagerSingleton{};

EntityTemplateManager::EntityTemplateManager() {}

EntityTemplateManager::~EntityTemplateManager() {}

Dictionary EntityTemplateManager::xml_node_to_dict(std::shared_ptr<XmlNode> node) {
	Dictionary dict;
	
	if (!node) {
		return dict;
	}

	// Add all attributes
	for (const auto& [key, attr] : node->attributes) {
		switch (attr.type) {
		case XmlAttribute::FLOAT:
			dict[String(key.c_str())] = attr.float_value;
			break;
		case XmlAttribute::INT:
			dict[String(key.c_str())] = attr.int_value;
			break;
		case XmlAttribute::BOOL:
			dict[String(key.c_str())] = attr.bool_value;
			break;
		case XmlAttribute::STRING:
		default:
			dict[String(key.c_str())] = String(attr.string_value.c_str());
			break;
		}
	}

	// Add text content if present
	if (!node->text_content.empty()) {
		dict["_text"] = String(node->text_content.c_str());
	}

	// Recursively add children
	if (!node->children.empty()) {
		Dictionary children_dict;
		for (const auto& child : node->children) {
			if (child) {
				String child_name = String(child->name.c_str());
				// If multiple children with same name, store as array
				if (children_dict.has(child_name)) {
					Variant existing = children_dict[child_name];
					if (existing.get_type() == Variant::ARRAY) {
						Array arr = existing;
						arr.append(xml_node_to_dict(child));
						children_dict[child_name] = arr;
					} else {
						Array arr;
						arr.append(existing);
						arr.append(xml_node_to_dict(child));
						children_dict[child_name] = arr;
					}
				} else {
					children_dict[child_name] = xml_node_to_dict(child);
				}
			}
		}
		if (children_dict.size() > 0) {
			dict["_children"] = children_dict;
		}
	}

	return dict;
}

Dictionary EntityTemplateManager::get_entity_template(String entity_name) {
	Dictionary empty_dict;

	// Check cache first
	if (template_cache.has(entity_name)) {
		UtilityFunctions::print("Retrieved entity template from cache: ", entity_name);
		return template_cache[entity_name];
	}

	// Try to load from patchdata/default/entities/
	String entity_path = "res://patchdata/default/entities/" + entity_name + ".xml";
	
	if (!FileAccess::file_exists(entity_path)) {
		UtilityFunctions::push_error("Entity file not found: ", entity_path);
		return empty_dict;
	}

	// Load and parse the XML
	String entity_content = FileAccess::get_file_as_string(entity_path);
	XmlDocument doc = XmlLoader::parse_xml_string(entity_content.utf8().get_data());
	
	if (!doc.is_valid || !doc.root) {
		UtilityFunctions::push_error("Failed to parse entity XML: ", entity_path);
		return empty_dict;
	}

	// Convert to dictionary
	Dictionary template_dict = xml_node_to_dict(doc.root);
	
	// Cache it
	template_cache[entity_name] = template_dict;
	
	UtilityFunctions::print("Loaded entity template: ", entity_name);
	return template_dict;
}

TypedArray<String> EntityTemplateManager::get_available_entities() {
	TypedArray<String> entities;
	
	// List all XML files in patchdata/default/entities/
	String entities_path = "res://patchdata/default/entities/";
	
	auto entities_dir = DirAccess::open(entities_path);
	if (entities_dir == nullptr) {
		UtilityFunctions::push_error("Failed to open entities directory: ", entities_path);
		return entities;
	}

	entities_dir->list_dir_begin();
	String file = "";

	while ((file = entities_dir->get_next()) != "") {
		if (!entities_dir->current_is_dir() && file.ends_with(".xml")) {
			String entity_name = file.substr(0, file.length() - 4); // Remove .xml
			entities.append(entity_name);
		}
	}

	return entities;
}

void EntityTemplateManager::dump_templates() {
	UtilityFunctions::print("=== Entity Template Manager ===");
	UtilityFunctions::print("Cached templates: ", template_cache.size());
	
	for (const auto& [key, value] : template_cache) {
		UtilityFunctions::print("  - ", key.ascii().get_data());
	}
	
	TypedArray<String> available = get_available_entities();
	UtilityFunctions::print("Available entities in patchdata: ", available.size());
	for (int i = 0; i < available.size(); i++) {
		UtilityFunctions::print("  - ", available[i]);
	}
}