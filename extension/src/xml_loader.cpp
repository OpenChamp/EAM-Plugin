#include "xml_loader.hpp"

#include <filesystem>
#include <godot_cpp/variant/utility_functions.hpp>

// PugiXML from local copy
#include "pugixml.hpp"

using namespace godot;

// XmlAttribute implementation
XmlAttribute XmlNode::get_attribute(const std::string& attr_name) const {
    auto it = attributes.find(attr_name);
    if (it != attributes.end()) {
        return it->second;
    }
    return XmlAttribute();
}

std::shared_ptr<XmlNode> XmlNode::find_child(const std::string& child_name) const {
    for (const auto& child : children) {
        if (child && child->name == child_name) {
            return child;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<XmlNode>> XmlNode::find_children(const std::string& child_name) const {
    std::vector<std::shared_ptr<XmlNode>> result;
    for (const auto& child : children) {
        if (child && child->name == child_name) {
            result.push_back(child);
        }
    }
    return result;
}

// XmlLoader implementation
std::shared_ptr<XmlNode> XmlLoader::convert_pugi_node(const pugi::xml_node& pugi_node) {
    auto node = std::make_shared<XmlNode>();
    node->name = pugi_node.name();
    node->text_content = pugi_node.text().as_string();
    
    // Convert attributes
    for (const auto& attr : pugi_node.attributes()) {
        XmlAttribute xml_attr;
        xml_attr.string_value = attr.as_string();
        xml_attr.type = XmlAttribute::STRING;
        
        // Try to parse as other types
        std::string value_str = attr.as_string();
        if (!value_str.empty()) {
            // Try bool first
            if (value_str == "true" || value_str == "1") {
                xml_attr.bool_value = true;
                xml_attr.type = XmlAttribute::BOOL;
            } else if (value_str == "false" || value_str == "0") {
                xml_attr.bool_value = false;
                xml_attr.type = XmlAttribute::BOOL;
            } else {
                // Try to parse as number (int or float)
                bool is_number = true;
                bool has_dot = false;
                size_t start = (value_str[0] == '-' || value_str[0] == '+') ? 1 : 0;
                
                for (size_t i = start; i < value_str.length(); ++i) {
                    if (value_str[i] == '.') {
                        if (has_dot) {
                            is_number = false;
                            break;
                        }
                        has_dot = true;
                    } else if (!std::isdigit(value_str[i])) {
                        is_number = false;
                        break;
                    }
                }
                
                if (is_number && start < value_str.length()) {
                    if (has_dot) {
                        xml_attr.float_value = std::stof(value_str);
                        xml_attr.type = XmlAttribute::FLOAT;
                    } else {
                        xml_attr.int_value = std::stoi(value_str);
                        xml_attr.type = XmlAttribute::INT;
                    }
                }
                // else: keep as string (default type)
            }
        }
        
        node->attributes[attr.name()] = xml_attr;
    }
    
    // Recursively convert children
    for (const auto& child : pugi_node.children()) {
        if (child.type() == pugi::node_element) {
            node->children.push_back(convert_pugi_node(child));
        }
    }
    
    return node;
}

XmlDocument XmlLoader::load_xml_file(const std::string& file_path) {
    XmlDocument doc;
    
    // Check if file exists
    if (!std::filesystem::exists(file_path)) {
        doc.is_valid = false;
        doc.error_message = "File does not exist: " + file_path;
        UtilityFunctions::push_error("XmlLoader: ", String(doc.error_message.c_str()));
        return doc;
    }
    
    // Load file with pugixml
    pugi::xml_document pugi_doc;
    pugi::xml_parse_result result = pugi_doc.load_file(file_path.c_str());
    
    if (!result) {
        doc.is_valid = false;
        doc.error_message = std::string("Failed to parse XML file: ") + result.description();
        UtilityFunctions::push_error("XmlLoader: ", String(doc.error_message.c_str()));
        return doc;
    }
    
    // Convert root node
    doc.root = convert_pugi_node(pugi_doc.document_element());
    doc.is_valid = true;
    
    UtilityFunctions::print("Successfully loaded XML file: ", String(file_path.c_str()));
    return doc;
}

XmlDocument XmlLoader::parse_xml_string(const std::string& xml_content) {
    XmlDocument doc;
    
    pugi::xml_document pugi_doc;
    pugi::xml_parse_result result = pugi_doc.load_string(xml_content.c_str());
    
    if (!result) {
        doc.is_valid = false;
        doc.error_message = std::string("Failed to parse XML string: ") + result.description();
        UtilityFunctions::push_error("XmlLoader: ", String(doc.error_message.c_str()));
        return doc;
    }
    
    // Convert root node
    doc.root = convert_pugi_node(pugi_doc.document_element());
    doc.is_valid = true;
    
    return doc;
}

std::vector<std::string> XmlLoader::list_files_from_directory(const std::string& path, const std::string& file_extension) {
    std::vector<std::string> file_names;
    
    // Verify path exists
    if (!std::filesystem::exists(path)) {
        UtilityFunctions::push_error("XmlLoader::list_files_from_directory: Path does not exist: ", String(path.c_str()));
        return file_names;
    }
    
    // Recursively iterate through directory
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().extension() == file_extension) {
            file_names.push_back(entry.path().string());
        }
    }
    
    return file_names;
}
