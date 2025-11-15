#include "json2doc/json2doc.h"

/**
 * @author Ewerton Silva Santos <ewerton_dc@hotmai.com> - GitHub: EwertonDCSilv
 * @brief Implementation of Json2Doc class
 * @namespace json2doc
 * @details
 *
 * TODO: Check necessity of this class
 * TODO: Check rename of this file and class
 */
namespace json2doc
{

    /**
     * @brief Construct a new Json2Doc object
     */
    Json2Doc::Json2Doc() : jsonData_("")
    {
    }

    /**
     * @brief Destroy the Json2Doc object
     */
    Json2Doc::~Json2Doc()
    {
    }

    /**
     * @brief Get the Version object
     * @return std::string The version string
     */
    std::string Json2Doc::getVersion() const
    {
        // TODO: get this value from a git tag when building
        return "1.0.0";
    }

    /**
     * @brief Load JSON data from a string
     * @param jsonData The JSON data as a string
     * @return true if loading was successful
     * @return false if loading failed
     */
    bool Json2Doc::loadJson(const std::string &jsonData)
    {
        if (jsonData.empty())
        {
            return false;
        }
        jsonData_ = jsonData;
        return true;
    }

    /**
     * @brief Convert loaded JSON to document format
     * @param templatePath Path to the document template
     * @return std::string The generated document content
     */
    std::string Json2Doc::convertToDocument(const std::string &templatePath)
    {
        if (jsonData_.empty())
        {
            return "";
        }

        return "Document generated from template: " + templatePath +
               " with JSON data";
    }

} // namespace json2doc