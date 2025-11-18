#pragma once

#include "base_include.hpp"
#include "xml_loader.hpp"

namespace godot {

/**
 * Manager for loading and caching entity templates from XML data.
 * Provides convenient access to entity definitions for spawning entities in-game.
 */
class EntityTemplateManager : public RefCounted {
	GDCLASS(EntityTemplateManager, RefCounted)

private:
	// Cache of loaded entity templates keyed by entity ID
	HashMap<String, Dictionary> template_cache;
	bool templates_indexed = false;

	static Ref<EntityTemplateManager> _EntityTemplateManagerSingleton;

protected:
	static void _bind_methods();

public:
	static Ref<EntityTemplateManager> get_singleton() { 
		if (_EntityTemplateManagerSingleton.is_null()) {
			_EntityTemplateManagerSingleton.instantiate();
		}
		return _EntityTemplateManagerSingleton;
	}

	static void destory_singleton() {
		if (_EntityTemplateManagerSingleton.is_valid()) {
			_EntityTemplateManagerSingleton.unref();
		}
	}

	EntityTemplateManager();
	~EntityTemplateManager();
	
	/**
	 * Load an entity template by name.
	 * @param entity_name The name of the entity (e.g. "cannon_minion")
	 * @return A Dictionary containing the entity template data, or empty dict if not found
	 */
	Dictionary get_entity_template(String entity_name);
	
	/**
	 * Get all loaded entity template names.
	 * @return Array of entity names available
	 */
	TypedArray<String> get_available_entities();
	
	/**
	 * Create an entity instance from a template.
	 * Returns a Dictionary with all the entity properties and components ready to apply.
	 * @param entity_name The name of the entity (e.g. "minions/cannon_minion")
	 * @return A Dictionary with organized entity data (id, stats, components, etc.)
	 */
	Dictionary create_entity_instance(String entity_name);
	
	/**
	 * Dump all loaded templates (for debugging).
	 */
	void dump_templates();

private:
	/**
	 * Convert parsed XmlNode to a Godot Dictionary.
	 */
	static Dictionary xml_node_to_dict(std::shared_ptr<XmlNode> node);
};

} //namespace godot
