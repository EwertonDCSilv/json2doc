#ifndef JSON2DOC_H
#define JSON2DOC_H

#include <string>

namespace json2doc {

/**
 * @brief Main class for JSON to Document conversion
 * 
 * This class provides the main interface for converting JSON data
 * to various document formats.
 */
class Json2Doc {
public:
    /**
     * @brief Construct a new Json2Doc object
     */
    Json2Doc();

    /**
     * @brief Destroy the Json2Doc object
     */
    ~Json2Doc();

    /**
     * @brief Get the version of the library
     * 
     * @return std::string The version string
     */
    std::string getVersion() const;

    /**
     * @brief Load JSON data from a string
     * 
     * @param jsonData The JSON data as a string
     * @return true if loading was successful
     * @return false if loading failed
     */
    bool loadJson(const std::string& jsonData);

    /**
     * @brief Convert loaded JSON to document format
     * 
     * @param templatePath Path to the document template
     * @return std::string The generated document content
     */
    std::string convertToDocument(const std::string& templatePath);

private:
    std::string jsonData_;
};

} // namespace json2doc

#endif // JSON2DOC_H
