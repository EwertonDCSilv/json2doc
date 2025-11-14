#include "json2doc/json_merge.h"
#include "json2doc/xml_document.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cctype>
#include <iostream>

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
                // Array - extract and parse
                std::string arr = extractArray(cleaned, pos);
                jsonData_[fullKey] = arr;

                // Parse array into jsonArrays_
                parseJsonArray(fullKey, arr);

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

    // ========== XML-based methods implementation ==========

    int JsonMerge::mergeIntoXml(XmlDocument &xmlDoc) const
    {
        if (!xmlDoc.isValid())
        {
            return 0;
        }

        return xmlDoc.replaceVariables(jsonData_);
    }

    std::vector<std::string> JsonMerge::findTemplateNodesInXml(const XmlDocument &xmlDoc) const
    {
        std::vector<std::string> results;

        if (!xmlDoc.isValid())
        {
            return results;
        }

        auto templateNodes = xmlDoc.findTemplateNodes();
        for (const auto &node : templateNodes)
        {
            results.push_back(node.path);
        }

        return results;
    }

    int JsonMerge::replaceVariablesInXml(XmlDocument &xmlDoc, const std::string &xpath) const
    {
        if (!xmlDoc.isValid())
        {
            return 0;
        }

        // Use XmlDocument's built-in variable replacement
        return xmlDoc.replaceVariables(jsonData_);
    }

    std::map<std::string, std::string> JsonMerge::getVariableMap() const
    {
        return jsonData_;
    }

    void JsonMerge::parseJsonArray(const std::string &arrayName, const std::string &arrayString)
    {
        std::vector<std::map<std::string, std::string>> items;

        std::string cleaned = trim(arrayString);
        if (cleaned.empty() || cleaned.front() != '[')
        {
            return;
        }

        size_t pos = 1; // Skip opening [

        while (pos < cleaned.length())
        {
            // Skip whitespace
            while (pos < cleaned.length() && std::isspace(cleaned[pos]))
            {
                pos++;
            }

            if (pos >= cleaned.length() || cleaned[pos] == ']')
            {
                break;
            }

            // Expect an object {
            if (cleaned[pos] == '{')
            {
                std::map<std::string, std::string> item;
                size_t objStart = pos;
                size_t objEnd = pos + 1;
                int braceCount = 1;

                // Find matching closing brace
                while (objEnd < cleaned.length() && braceCount > 0)
                {
                    if (cleaned[objEnd] == '{')
                    {
                        braceCount++;
                    }
                    else if (cleaned[objEnd] == '}')
                    {
                        braceCount--;
                    }
                    objEnd++;
                }

                std::string objStr = cleaned.substr(objStart, objEnd - objStart);

                // Parse object fields
                size_t fieldPos = 1; // Skip opening {
                while (fieldPos < objStr.length())
                {
                    // Skip whitespace
                    while (fieldPos < objStr.length() && std::isspace(objStr[fieldPos]))
                    {
                        fieldPos++;
                    }

                    if (fieldPos >= objStr.length() || objStr[fieldPos] == '}')
                    {
                        break;
                    }

                    // Find key
                    if (objStr[fieldPos] != '"')
                    {
                        fieldPos++;
                        continue;
                    }

                    size_t keyStart = fieldPos + 1;
                    size_t keyEnd = objStr.find('"', keyStart);
                    if (keyEnd == std::string::npos)
                    {
                        break;
                    }

                    std::string key = objStr.substr(keyStart, keyEnd - keyStart);
                    fieldPos = keyEnd + 1;

                    // Skip to colon
                    while (fieldPos < objStr.length() && objStr[fieldPos] != ':')
                    {
                        fieldPos++;
                    }
                    fieldPos++; // Skip colon

                    // Skip whitespace
                    while (fieldPos < objStr.length() && std::isspace(objStr[fieldPos]))
                    {
                        fieldPos++;
                    }

                    // Extract value
                    std::string value;
                    if (objStr[fieldPos] == '"')
                    {
                        // String value
                        size_t valueStart = fieldPos + 1;
                        size_t valueEnd = objStr.find('"', valueStart);
                        if (valueEnd != std::string::npos)
                        {
                            value = objStr.substr(valueStart, valueEnd - valueStart);
                            fieldPos = valueEnd + 1;
                        }
                    }
                    else
                    {
                        // Number, boolean, or null
                        size_t valueStart = fieldPos;
                        while (fieldPos < objStr.length() && objStr[fieldPos] != ',' && objStr[fieldPos] != '}')
                        {
                            fieldPos++;
                        }
                        value = trim(objStr.substr(valueStart, fieldPos - valueStart));
                    }

                    item[key] = value;

                    // Skip comma
                    while (fieldPos < objStr.length() && (objStr[fieldPos] == ',' || std::isspace(objStr[fieldPos])))
                    {
                        fieldPos++;
                    }
                }

                items.push_back(item);
                pos = objEnd;
            }

            // Skip comma and whitespace
            while (pos < cleaned.length() && (cleaned[pos] == ',' || std::isspace(cleaned[pos])))
            {
                pos++;
            }
        }

        jsonArrays_[arrayName] = items;
    }

    std::string JsonMerge::getListName(const std::string &variable) const
    {
        // Check if variable contains a dot (e.g., "items.id")
        size_t dotPos = variable.find('.');
        if (dotPos != std::string::npos)
        {
            std::string listName = variable.substr(0, dotPos);
            // Check if this list exists in jsonArrays_
            if (jsonArrays_.find(listName) != jsonArrays_.end())
            {
                return listName;
            }
        }
        return "";
    }

    std::vector<std::map<std::string, std::string>> JsonMerge::getArrayData(const std::string &listName) const
    {
        auto it = jsonArrays_.find(listName);
        if (it != jsonArrays_.end())
        {
            return it->second;
        }
        return std::vector<std::map<std::string, std::string>>();
    }

    int JsonMerge::processTablesInXml(XmlDocument &xmlDoc) const
    {
        if (!xmlDoc.isValid())
        {
            return 0;
        }

        int totalRowsCreated = 0;

        // Find all table rows in DOCX (w:tr elements)
        auto allRows = xmlDoc.query("//*[local-name()='tr']");

        for (const auto &row : allRows)
        {
            // Get text content of this row to check for list variables
            std::string rowText = xmlDoc.getTextContent(row.path);

            // Find variables in this row
            auto variables = findVariables(rowText);
            if (variables.empty())
            {
                continue;
            }

            // Check if any variable is a list variable
            std::string listName;
            std::vector<std::string> listFields;

            for (const auto &var : variables)
            {
                std::string currentListName = getListName(var);
                if (!currentListName.empty())
                {
                    if (listName.empty())
                    {
                        listName = currentListName;
                    }

                    // Extract field name (part after the dot)
                    size_t dotPos = var.find('.');
                    if (dotPos != std::string::npos)
                    {
                        std::string fieldName = var.substr(dotPos + 1);
                        listFields.push_back(fieldName);
                    }
                }
            }

            // If this row has list variables, process it
            if (!listName.empty())
            {
                auto arrayData = getArrayData(listName);
                if (arrayData.empty())
                {
                    continue;
                }

                // Get the XML content of the template row
                std::string templateRowXml = xmlDoc.getNodeXml(row.path);
                if (templateRowXml.empty())
                {
                    continue;
                }

                // Create new rows for each item in the array
                std::string newRowsXml;
                for (const auto &item : arrayData)
                {
                    std::string newRow = templateRowXml;

                    // Replace each variable in the row
                    for (const auto &var : variables)
                    {
                        std::string currentListName = getListName(var);
                        if (currentListName == listName)
                        {
                            size_t dotPos = var.find('.');
                            if (dotPos != std::string::npos)
                            {
                                std::string fieldName = var.substr(dotPos + 1);
                                auto fieldIt = item.find(fieldName);
                                if (fieldIt != item.end())
                                {
                                    std::string placeholder = "{{" + var + "}}";
                                    std::string value = fieldIt->second;

                                    // Replace all occurrences
                                    size_t pos = 0;
                                    while ((pos = newRow.find(placeholder, pos)) != std::string::npos)
                                    {
                                        newRow.replace(pos, placeholder.length(), value);
                                        pos += value.length();
                                    }
                                }
                            }
                        }
                    }

                    newRowsXml += newRow;
                    totalRowsCreated++;
                }

                // Replace the template row with the new rows
                if (xmlDoc.replaceNode(row.path, newRowsXml))
                {
                    // Success
                }
            }
        }

        return totalRowsCreated;
    }

} // namespace json2doc
