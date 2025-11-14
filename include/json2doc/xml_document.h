#ifndef XML_DOCUMENT_H
#define XML_DOCUMENT_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace json2doc
{

    /**
     * @brief Wrapper class for XML document operations using pugixml
     *
     * This class provides:
     * - XML parsing and manipulation
     * - XPath query support
     * - Node traversal and modification
     * - Integration with JsonMerge for template processing
     */
    class XmlDocument
    {
    public:
        /**
         * @brief Node information structure
         */
        struct XmlNode
        {
            std::string path;     // XPath to this node
            std::string name;     // Node name
            std::string value;    // Node text content
            std::string fullText; // Full text including children
            std::map<std::string, std::string> attributes;
        };

        /**
         * @brief Construct a new XmlDocument object
         */
        XmlDocument();

        /**
         * @brief Destroy the XmlDocument object
         */
        ~XmlDocument();

        /**
         * @brief Load XML from string
         *
         * @param xmlContent The XML content as string
         * @return true if successfully parsed
         * @return false if parsing failed
         */
        bool loadFromString(const std::string &xmlContent);

        /**
         * @brief Load XML from file
         *
         * @param filePath Path to XML file
         * @return true if successfully loaded and parsed
         * @return false if loading or parsing failed
         */
        bool loadFromFile(const std::string &filePath);

        /**
         * @brief Get XML content as string
         *
         * @return std::string The formatted XML content
         */
        std::string toString() const;

        /**
         * @brief Execute XPath query and return matching nodes
         *
         * @param xpath The XPath expression
         * @return std::vector<XmlNode> List of matching nodes
         */
        std::vector<XmlNode> query(const std::string &xpath) const;

        /**
         * @brief Find all text nodes matching XPath
         *
         * @param xpath The XPath expression (default: all text nodes)
         * @return std::vector<std::string> List of text values
         */
        std::vector<std::string> findTextNodes(const std::string &xpath = "//text()") const;

        /**
         * @brief Find all nodes containing {{variable}} placeholders
         *
         * @return std::vector<XmlNode> Nodes with template variables
         */
        std::vector<XmlNode> findTemplateNodes() const;

        /**
         * @brief Replace text in nodes matching XPath
         *
         * @param xpath XPath expression to find nodes
         * @param oldText Text to replace
         * @param newText Replacement text
         * @return int Number of replacements made
         */
        int replaceText(const std::string &xpath, const std::string &oldText, const std::string &newText);

        /**
         * @brief Replace all {{variable}} placeholders using a value map
         *
         * @param variables Map of variable names to values
         * @return int Number of replacements made
         */
        int replaceVariables(const std::map<std::string, std::string> &variables);

        /**
         * @brief Get all text content from document
         *
         * @return std::string Concatenated text content
         */
        std::string getTextContent() const;

        /**
         * @brief Get node by XPath (first match)
         *
         * @param xpath The XPath expression
         * @return XmlNode The first matching node (empty if not found)
         */
        XmlNode getNode(const std::string &xpath) const;

        /**
         * @brief Set node text content by XPath
         *
         * @param xpath The XPath expression
         * @param text New text content
         * @return true if node was found and updated
         * @return false if node was not found
         */
        bool setNodeText(const std::string &xpath, const std::string &text);

        /**
         * @brief Get attribute value from node
         *
         * @param xpath XPath to node
         * @param attributeName Name of attribute
         * @return std::string Attribute value or empty string
         */
        std::string getAttributeValue(const std::string &xpath, const std::string &attributeName) const;

        /**
         * @brief Set attribute value on node
         *
         * @param xpath XPath to node
         * @param attributeName Name of attribute
         * @param value New attribute value
         * @return true if successful
         * @return false if node not found
         */
        bool setAttributeValue(const std::string &xpath, const std::string &attributeName, const std::string &value);

        /**
         * @brief Check if document is valid (was successfully loaded)
         *
         * @return true if valid
         * @return false if not loaded or parsing failed
         */
        bool isValid() const;

        /**
         * @brief Get XML content of a node by XPath
         *
         * @param xpath XPath expression to select node
         * @return std::string XML content of the node
         */
        std::string getNodeXml(const std::string &xpath) const;

        /**
         * @brief Replace a node with new XML content
         *
         * @param xpath XPath expression to select node to replace
         * @param newXml New XML content
         * @return true if replacement was successful
         * @return false if node not found or XML invalid
         */
        bool replaceNode(const std::string &xpath, const std::string &newXml);

        /**
         * @brief Get text content from node by XPath
         *
         * @param xpath XPath expression to select node
         * @return std::string Text content of the node
         */
        std::string getTextContent(const std::string &xpath) const;

        /**
         * @brief Get last error message
         *
         * @return std::string Error description
         */
        std::string getLastError() const;

        /**
         * @brief Clear document and reset state
         */
        void clear();

    private:
        class Impl; // Forward declaration for PIMPL pattern
        std::unique_ptr<Impl> pImpl_;
        std::string lastError_;
    };

} // namespace json2doc

#endif // XML_DOCUMENT_H
