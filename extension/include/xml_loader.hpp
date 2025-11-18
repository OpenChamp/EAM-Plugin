#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

// Forward declare pugi namespace to avoid circular includes
namespace pugi {
	class xml_node;
}
struct XmlAttribute {
    std::string string_value;
    float float_value = 0.0f;
    int int_value = 0;
    bool bool_value = false;
    
    enum Type {
        STRING,
        FLOAT,
        INT,
        BOOL
    } type = STRING;
};

/**
 * Generic structure representing an XML element with attributes and child nodes.
 */
struct XmlNode {
    std::string name;
    std::string text_content;
    std::map<std::string, XmlAttribute> attributes;
    std::vector<std::shared_ptr<XmlNode>> children;
    
    /**
     * Get an attribute value, returning empty string if not found.
     */
    XmlAttribute get_attribute(const std::string& attr_name) const;
    
    /**
     * Find a child node by name.
     */
    std::shared_ptr<XmlNode> find_child(const std::string& child_name) const;
    
    /**
     * Get all children with a specific name.
     */
    std::vector<std::shared_ptr<XmlNode>> find_children(const std::string& child_name) const;
};

/**
 * Generic XML document structure representing the root of a parsed XML file.
 */
struct XmlDocument {
    std::shared_ptr<XmlNode> root;
    bool is_valid = false;
    std::string error_message;
};

/**
 * Generic XML parser for loading and parsing XML files.
 * Returns generic data structures that can be used by any client code.
 */
class XmlLoader {
public:
    /**
     * Load and parse an XML file.
     * @param file_path The path to the XML file to load
     * @return An XmlDocument containing the parsed data, or an invalid document on error
     */
    static XmlDocument load_xml_file(const std::string& file_path);
    
    /**
     * Parse XML content from a string.
     * @param xml_content The XML content as a string
     * @return An XmlDocument containing the parsed data, or an invalid document on error
     */
    static XmlDocument parse_xml_string(const std::string& xml_content);
    
    /**
     * List all files with a matching file extension in a directory.
     * @param path The directory to search
     * @param file_extension The file extension to match for, e.g. ".xml"
     * @return A vector of file paths found in the directory
     */
    static std::vector<std::string> list_files_from_directory(const std::string& path, const std::string& file_extension);

private:
    /**
     * Recursively convert a pugi::xml_node to an XmlNode structure.
     */
    static std::shared_ptr<XmlNode> convert_pugi_node(const class pugi::xml_node& pugi_node);
};
