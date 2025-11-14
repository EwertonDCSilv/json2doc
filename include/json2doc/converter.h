#ifndef JSON2DOC_CONVERTER_H
#define JSON2DOC_CONVERTER_H

#include <string>

namespace json2doc {

/**
 * @brief Converter utility class for various conversion operations
 * 
 * This class provides utility functions for converting between
 * different data formats.
 */
class Converter {
public:
    /**
     * @brief Convert JSON string to formatted output
     * 
     * @param json The JSON string to convert
     * @return std::string The formatted output
     */
    static std::string jsonToString(const std::string& json);

    /**
     * @brief Validate JSON format
     * 
     * @param json The JSON string to validate
     * @return true if JSON is valid
     * @return false if JSON is invalid
     */
    static bool isValidJson(const std::string& json);
};

} // namespace json2doc

#endif // JSON2DOC_CONVERTER_H
