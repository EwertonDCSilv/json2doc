#ifndef JSON_MERGE_H
#define JSON_MERGE_H

#include <string>
#include <vector>
#include <map>

namespace json2doc
{

    /**
     * @brief Class for merging JSON data into XML templates with {{variable}} placeholders
     *
     * This class handles:
     * - Loading and parsing JSON data
     * - Finding {{variable}} placeholders in XML content
     * - Replacing placeholders with corresponding JSON values
     * - Supporting nested JSON structures with dot notation (e.g., {{metadata.version}})
     */
    class JsonMerge
    {
    public:
        /**
         * @brief Construct a new JsonMerge object
         */
        JsonMerge();

        /**
         * @brief Destroy the JsonMerge object
         */
        ~JsonMerge();

        /**
         * @brief Load JSON data from a file
         *
         * @param jsonFilePath Path to the JSON file
         * @return true if JSON was successfully loaded and parsed
         * @return false if loading or parsing failed
         */
        bool loadJson(const std::string &jsonFilePath);

        /**
         * @brief Load JSON data from a string
         *
         * @param jsonString JSON content as string
         * @return true if JSON was successfully parsed
         * @return false if parsing failed
         */
        bool loadJsonString(const std::string &jsonString);

        /**
         * @brief Find all {{variable}} placeholders in text
         *
         * @param text The text to search for placeholders
         * @return std::vector<std::string> List of found variable names (without {{ }})
         */
        std::vector<std::string> findVariables(const std::string &text) const;

        /**
         * @brief Replace all {{variable}} placeholders with JSON values
         *
         * @param text The text containing placeholders
         * @return std::string Text with placeholders replaced
         */
        std::string replaceVariables(const std::string &text) const;

        /**
         * @brief Get a JSON value by key (supports dot notation for nested access)
         *
         * @param key The key to look up (e.g., "name" or "metadata.version")
         * @return std::string The value as string, or empty string if not found
         */
        std::string getValue(const std::string &key) const;

        /**
         * @brief Check if a key exists in the JSON data
         *
         * @param key The key to check (supports dot notation)
         * @return true if the key exists
         * @return false if the key does not exist
         */
        bool hasKey(const std::string &key) const;

        /**
         * @brief Get all available keys in the JSON data
         *
         * @return std::vector<std::string> List of all keys (including nested with dot notation)
         */
        std::vector<std::string> getAllKeys() const;

        /**
         * @brief Get the last error message
         *
         * @return std::string The error message
         */
        std::string getLastError() const;

        /**
         * @brief Get statistics about the last merge operation
         *
         * @return std::map<std::string, int> Map with "found", "replaced", "missing" counts
         */
        std::map<std::string, int> getStats() const;

        /**
         * @brief Clear all loaded JSON data
         */
        void clear();

    private:
        std::map<std::string, std::string> jsonData_;
        std::string lastError_;
        mutable std::map<std::string, int> lastStats_;

        /**
         * @brief Parse JSON string into the internal map
         *
         * @param jsonString The JSON content
         * @return true if parsing was successful
         * @return false if parsing failed
         */
        bool parseJson(const std::string &jsonString);

        /**
         * @brief Extract value from JSON string by key
         *
         * @param jsonString The JSON content
         * @param key The key to extract
         * @param startPos Starting position to search from
         * @return std::string The extracted value
         */
        std::string extractJsonValue(const std::string &jsonString, const std::string &key, size_t startPos = 0) const;

        /**
         * @brief Parse nested JSON objects and flatten to dot notation
         *
         * @param jsonString The JSON content
         * @param prefix Current prefix for nested keys
         */
        void parseNestedJson(const std::string &jsonString, const std::string &prefix = "");

        /**
         * @brief Trim whitespace and quotes from a string
         *
         * @param str The string to trim
         * @return std::string Trimmed string
         */
        std::string trim(const std::string &str) const;

        /**
         * @brief Extract JSON object content between { }
         *
         * @param jsonString The JSON content
         * @param startPos Starting position
         * @return std::string The extracted object content
         */
        std::string extractObject(const std::string &jsonString, size_t startPos) const;

        /**
         * @brief Extract JSON array content between [ ]
         *
         * @param jsonString The JSON content
         * @param startPos Starting position
         * @return std::string The extracted array content
         */
        std::string extractArray(const std::string &jsonString, size_t startPos) const;
    };

} // namespace json2doc

#endif // JSON_MERGE_H
