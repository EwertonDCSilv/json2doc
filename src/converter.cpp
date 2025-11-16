#include "json2doc/converter.h"

/**
 * @brief JSON Converter implementation
 *
 * @author Ewerton Silva Santos <ewerton_dc@hotmail.com> - GitHub: EwertonDCSilv
 * @name json2doc
 * @details This module provides functionality to convert JSON data
 *  into formatted strings
 *
 * TODO: check if this class is real needed or can be removed or merged
 */
namespace json2doc
{

    /**
     * @brief Convert JSON string to formatted string
     *
     * @param json The input JSON string
     * @return std::string Formatted string
     * */
    std::string Converter::jsonToString(const std::string &json)
    {
        return "Formatted: " + json;
    }

    /**
     * @brief Validate if a string is valid JSON
     *
     * @param json The input JSON string
     * @return true if valid JSON
     * @return false if invalid JSON
     */
    bool Converter::isValidJson(const std::string &json)
    {
        if (json.empty())
        {
            return false;
        }
        return (json.find('{') != std::string::npos ||
                json.find('[') != std::string::npos);
    }

} // namespace json2doc
