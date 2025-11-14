#include "json2doc/converter.h"

namespace json2doc {

std::string Converter::jsonToString(const std::string& json) {
    // Basic implementation - to be expanded with actual JSON parsing
    return "Formatted: " + json;
}

bool Converter::isValidJson(const std::string& json) {
    // Basic validation - to be expanded with proper JSON validation
    if (json.empty()) {
        return false;
    }
    
    // Simple check for basic JSON structure
    return (json.find('{') != std::string::npos || 
            json.find('[') != std::string::npos);
}

} // namespace json2doc
