#include "json2doc/json_merge.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cctype>

namespace json2doc
{

    JsonMerge::JsonMerge()
        : lastError_("")
    {
        lastStats_["found"] = 0;
        lastStats_["replaced"] = 0;
        lastStats_["missing"] = 0;
    }

    JsonMerge::~JsonMerge()
    {
        clear();
    }

    bool JsonMerge::loadJson(const std::string &jsonFilePath)
    {
        std::ifstream file(jsonFilePath);
        if (!file.is_open())
        {
            lastError_ = "Cannot open file: " + jsonFilePath;
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        return loadJsonString(buffer.str());
    }

    bool JsonMerge::loadJsonString(const std::string &jsonString)
    {
        clear();
        return parseJson(jsonString);
    }

    std::vector<std::string> JsonMerge::findVariables(const std::string &text) const
    {
        std::vector<std::string> variables;
        std::regex varRegex(R"(\{\{([^}]+)\}\})");
        std::smatch match;

        std::string::const_iterator searchStart(text.cbegin());
        while (std::regex_search(searchStart, text.cend(), match, varRegex))
        {
            std::string varName = trim(match[1].str());
            variables.push_back(varName);
            searchStart = match.suffix().first;
        }

        return variables;
    }

    std::string JsonMerge::replaceVariables(const std::string &text) const
    {
        lastStats_["found"] = 0;
        lastStats_["replaced"] = 0;
        lastStats_["missing"] = 0;

        std::string result = text;
        std::regex varRegex(R"(\{\{([^}]+)\}\})");
        std::smatch match;

        std::string::const_iterator searchStart(result.cbegin());
        std::vector<std::pair<size_t, size_t>> replacements;
        std::vector<std::string> values;

        // Find all matches first
        while (std::regex_search(searchStart, result.cend(), match, varRegex))
        {
            lastStats_["found"]++;
            
            std::string varName = trim(match[1].str());
            std::string value = getValue(varName);

            size_t pos = std::distance(result.cbegin(), match[0].first);
            size_t len = match[0].length();

            if (!value.empty())
            {
                lastStats_["replaced"]++;
                replacements.push_back({pos, len});
                values.push_back(value);
            }
            else
            {
                lastStats_["missing"]++;
            }

            searchStart = match.suffix().first;
        }

        // Replace from end to beginning to maintain positions
        for (int i = replacements.size() - 1; i >= 0; i--)
        {
            result.replace(replacements[i].first, replacements[i].second, values[i]);
        }

        return result;
    }

    std::string JsonMerge::getValue(const std::string &key) const
    {
        std::string cleanKey = trim(key);

        // Direct lookup
        auto it = jsonData_.find(cleanKey);
        if (it != jsonData_.end())
        {
            return it->second;
        }

        // Try nested lookup with dot notation
        size_t dotPos = cleanKey.find('.');
        if (dotPos != std::string::npos)
        {
            std::string prefix = cleanKey.substr(0, dotPos);
            std::string suffix = cleanKey.substr(dotPos + 1);

            // Look for the nested key
            it = jsonData_.find(cleanKey);
            if (it != jsonData_.end())
            {
                return it->second;
            }
        }

        return "";
    }

    bool JsonMerge::hasKey(const std::string &key) const
    {
        return !getValue(key).empty();
    }

    std::vector<std::string> JsonMerge::getAllKeys() const
    {
        std::vector<std::string> keys;
        for (const auto &pair : jsonData_)
        {
            keys.push_back(pair.first);
        }
        return keys;
    }

    std::string JsonMerge::getLastError() const
    {
        return lastError_;
    }

    std::map<std::string, int> JsonMerge::getStats() const
    {
        return lastStats_;
    }

    void JsonMerge::clear()
    {
        jsonData_.clear();
        lastError_ = "";
        lastStats_["found"] = 0;
        lastStats_["replaced"] = 0;
        lastStats_["missing"] = 0;
    }

    bool JsonMerge::parseJson(const std::string &jsonString)
    {
        try
        {
            parseNestedJson(jsonString, "");
            return true;
        }
        catch (const std::exception &e)
        {
            lastError_ = std::string("JSON parse error: ") + e.what();
            return false;
        }
    }

    void JsonMerge::parseNestedJson(const std::string &jsonString, const std::string &prefix)
    {
        std::string cleaned = trim(jsonString);
        if (cleaned.empty() || cleaned.front() != '{')
        {
            return;
        }

        size_t pos = 1; // Skip opening {
        while (pos < cleaned.length())
        {
            // Skip whitespace
            while (pos < cleaned.length() && std::isspace(cleaned[pos]))
            {
                pos++;
            }

            if (pos >= cleaned.length() || cleaned[pos] == '}')
            {
                break;
            }

            // Find key
            if (cleaned[pos] != '"')
            {
                pos++;
                continue;
            }

            size_t keyStart = pos + 1;
            size_t keyEnd = cleaned.find('"', keyStart);
            if (keyEnd == std::string::npos)
            {
                break;
            }

            std::string key = cleaned.substr(keyStart, keyEnd - keyStart);
            pos = keyEnd + 1;

            // Skip to colon
            while (pos < cleaned.length() && cleaned[pos] != ':')
            {
                pos++;
            }
            pos++; // Skip colon

            // Skip whitespace
            while (pos < cleaned.length() && std::isspace(cleaned[pos]))
            {
                pos++;
            }

            // Determine value type and extract
            std::string fullKey = prefix.empty() ? key : prefix + "." + key;

            if (cleaned[pos] == '"')
            {
                // String value
                size_t valueStart = pos + 1;
                size_t valueEnd = cleaned.find('"', valueStart);
                if (valueEnd != std::string::npos)
                {
                    std::string value = cleaned.substr(valueStart, valueEnd - valueStart);
                    jsonData_[fullKey] = value;
                    pos = valueEnd + 1;
                }
            }
            else if (cleaned[pos] == '{')
            {
                // Nested object
                std::string obj = extractObject(cleaned, pos);
                parseNestedJson(obj, fullKey);
                pos += obj.length() + 1;
            }
            else if (cleaned[pos] == '[')
            {
                // Array
                std::string arr = extractArray(cleaned, pos);
                jsonData_[fullKey] = arr;
                pos += arr.length() + 1;
            }
            else
            {
                // Number, boolean, or null
                size_t valueStart = pos;
                while (pos < cleaned.length() && cleaned[pos] != ',' && cleaned[pos] != '}' && cleaned[pos] != ']')
                {
                    pos++;
                }
                std::string value = trim(cleaned.substr(valueStart, pos - valueStart));
                jsonData_[fullKey] = value;
            }

            // Skip to next element
            while (pos < cleaned.length() && (cleaned[pos] == ',' || std::isspace(cleaned[pos])))
            {
                pos++;
            }
        }
    }

    std::string JsonMerge::extractObject(const std::string &jsonString, size_t startPos) const
    {
        int braceCount = 0;
        size_t pos = startPos;

        while (pos < jsonString.length())
        {
            if (jsonString[pos] == '{')
            {
                braceCount++;
            }
            else if (jsonString[pos] == '}')
            {
                braceCount--;
                if (braceCount == 0)
                {
                    return jsonString.substr(startPos, pos - startPos + 1);
                }
            }
            pos++;
        }

        return "";
    }

    std::string JsonMerge::extractArray(const std::string &jsonString, size_t startPos) const
    {
        int bracketCount = 0;
        size_t pos = startPos;

        while (pos < jsonString.length())
        {
            if (jsonString[pos] == '[')
            {
                bracketCount++;
            }
            else if (jsonString[pos] == ']')
            {
                bracketCount--;
                if (bracketCount == 0)
                {
                    return jsonString.substr(startPos, pos - startPos + 1);
                }
            }
            pos++;
        }

        return "";
    }

    std::string JsonMerge::trim(const std::string &str) const
    {
        if (str.empty())
        {
            return str;
        }

        size_t start = 0;
        size_t end = str.length() - 1;

        while (start <= end && std::isspace(str[start]))
        {
            start++;
        }

        while (end >= start && std::isspace(str[end]))
        {
            end--;
        }

        if (start > end)
        {
            return "";
        }

        return str.substr(start, end - start + 1);
    }

    std::string JsonMerge::extractJsonValue(const std::string &jsonString, const std::string &key, size_t startPos) const
    {
        // This is a helper method for future enhancements
        return "";
    }

} // namespace json2doc
