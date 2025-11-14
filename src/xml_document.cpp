#include "json2doc/xml_document.h"
#include <pugixml.hpp>
#include <fstream>
#include <sstream>
#include <regex>
#include <functional>

namespace json2doc
{

    // PIMPL implementation to hide pugixml details from header
    class XmlDocument::Impl
    {
    public:
        pugi::xml_document doc;
        bool valid = false;

        Impl() = default;
        ~Impl() = default;
    };

    XmlDocument::XmlDocument()
        : pImpl_(std::make_unique<Impl>()),
          lastError_("")
    {
    }

    XmlDocument::~XmlDocument()
    {
        clear();
    }

    bool XmlDocument::loadFromString(const std::string &xmlContent)
    {
        clear();

        pugi::xml_parse_result result = pImpl_->doc.load_string(xmlContent.c_str());

        if (result)
        {
            pImpl_->valid = true;
            lastError_ = "";
            return true;
        }
        else
        {
            pImpl_->valid = false;
            lastError_ = std::string("XML parsing error: ") + result.description() +
                         " at offset " + std::to_string(result.offset);
            return false;
        }
    }

    bool XmlDocument::loadFromFile(const std::string &filePath)
    {
        clear();

        pugi::xml_parse_result result = pImpl_->doc.load_file(filePath.c_str());

        if (result)
        {
            pImpl_->valid = true;
            lastError_ = "";
            return true;
        }
        else
        {
            pImpl_->valid = false;
            lastError_ = std::string("XML file error: ") + result.description() +
                         " (file: " + filePath + ")";
            return false;
        }
    }

    std::string XmlDocument::toString() const
    {
        if (!pImpl_->valid)
        {
            return "";
        }

        std::ostringstream oss;
        pImpl_->doc.save(oss, "  ");
        return oss.str();
    }

    std::vector<XmlDocument::XmlNode> XmlDocument::query(const std::string &xpath) const
    {
        std::vector<XmlNode> results;

        if (!pImpl_->valid)
        {
            return results;
        }

        try
        {
            pugi::xpath_node_set nodes = pImpl_->doc.select_nodes(xpath.c_str());

            for (const auto &xpathNode : nodes)
            {
                pugi::xml_node node = xpathNode.node();
                if (node)
                {
                    XmlNode xmlNode;
                    xmlNode.name = node.name();
                    xmlNode.value = node.text().as_string();

                    // Build unique XPath for this specific node
                    // Count preceding siblings with same name to get position
                    int position = 1;
                    for (pugi::xml_node sibling = node.previous_sibling(node.name()); sibling; sibling = sibling.previous_sibling(node.name()))
                    {
                        position++;
                    }

                    // Build path from root to this node
                    std::string nodePath;
                    pugi::xml_node current = node;
                    while (current && current.type() != pugi::node_document)
                    {
                        int siblingPos = 1;
                        for (pugi::xml_node sib = current.previous_sibling(current.name()); sib; sib = sib.previous_sibling(current.name()))
                        {
                            siblingPos++;
                        }

                        std::string part = "/";
                        part += current.name();
                        if (siblingPos > 1 || current.next_sibling(current.name()))
                        {
                            part += "[" + std::to_string(siblingPos) + "]";
                        }

                        nodePath = part + nodePath;
                        current = current.parent();
                    }

                    xmlNode.path = nodePath;

                    // Get full text including children
                    std::ostringstream oss;
                    node.print(oss, "", pugi::format_raw);
                    xmlNode.fullText = oss.str();

                    // Get attributes
                    for (const auto &attr : node.attributes())
                    {
                        xmlNode.attributes[attr.name()] = attr.value();
                    }

                    results.push_back(xmlNode);
                }
            }
        }
        catch (const pugi::xpath_exception &e)
        {
            const_cast<XmlDocument *>(this)->lastError_ = std::string("XPath error: ") + e.what();
        }

        return results;
    }

    std::vector<std::string> XmlDocument::findTextNodes(const std::string &xpath) const
    {
        std::vector<std::string> results;

        if (!pImpl_->valid)
        {
            return results;
        }

        try
        {
            pugi::xpath_node_set nodes = pImpl_->doc.select_nodes(xpath.c_str());

            for (const auto &xpathNode : nodes)
            {
                if (xpathNode.node())
                {
                    std::string text = xpathNode.node().text().as_string();
                    if (!text.empty())
                    {
                        results.push_back(text);
                    }
                }
                else if (xpathNode.attribute())
                {
                    results.push_back(xpathNode.attribute().value());
                }
            }
        }
        catch (const pugi::xpath_exception &e)
        {
            const_cast<XmlDocument *>(this)->lastError_ = std::string("XPath error: ") + e.what();
        }

        return results;
    }

    std::vector<XmlDocument::XmlNode> XmlDocument::findTemplateNodes() const
    {
        std::vector<XmlNode> results;

        if (!pImpl_->valid)
        {
            return results;
        }

        // Regex to find {{variable}} patterns
        std::regex varRegex(R"(\{\{[^}]+\}\})");

        // Recursive function to traverse nodes
        std::function<void(pugi::xml_node)> traverse = [&](pugi::xml_node node)
        {
            std::string text = node.text().as_string();
            if (std::regex_search(text, varRegex))
            {
                XmlNode xmlNode;
                xmlNode.name = node.name();
                xmlNode.value = text;

                // Build simple path
                std::string path = "/";
                pugi::xml_node current = node;
                std::vector<std::string> pathParts;
                while (current && current != pImpl_->doc)
                {
                    pathParts.insert(pathParts.begin(), current.name());
                    current = current.parent();
                }
                for (const auto &part : pathParts)
                {
                    if (!part.empty())
                    {
                        path += part + "/";
                    }
                }
                xmlNode.path = path;

                results.push_back(xmlNode);
            }

            // Recurse into children
            for (pugi::xml_node child : node.children())
            {
                traverse(child);
            }
        };

        // Start traversal from root
        traverse(pImpl_->doc.document_element());

        return results;
    }

    int XmlDocument::replaceText(const std::string &xpath, const std::string &oldText, const std::string &newText)
    {
        if (!pImpl_->valid)
        {
            return 0;
        }

        int count = 0;

        try
        {
            pugi::xpath_node_set nodes = pImpl_->doc.select_nodes(xpath.c_str());

            for (auto &xpathNode : nodes)
            {
                pugi::xml_node node = xpathNode.node();
                if (node)
                {
                    std::string text = node.text().as_string();
                    size_t pos = text.find(oldText);
                    if (pos != std::string::npos)
                    {
                        text.replace(pos, oldText.length(), newText);
                        node.text().set(text.c_str());
                        count++;
                    }
                }
            }
        }
        catch (const pugi::xpath_exception &e)
        {
            lastError_ = std::string("XPath error: ") + e.what();
        }

        return count;
    }

    int XmlDocument::replaceVariables(const std::map<std::string, std::string> &variables)
    {
        if (!pImpl_->valid)
        {
            return 0;
        }

        int totalReplacements = 0;
        std::regex varRegex(R"(\{\{([^}]+)\}\})");

        // Recursive function to traverse and replace in nodes
        std::function<void(pugi::xml_node)> traverse = [&](pugi::xml_node node)
        {
            pugi::xml_text textNode = node.text();
            std::string text = textNode.as_string();

            if (!text.empty() && std::regex_search(text, varRegex))
            {
                // Replace all variables in this text
                std::string result = text;
                std::smatch match;
                std::string::const_iterator searchStart(result.cbegin());

                // Collect all replacements first
                std::vector<std::pair<size_t, size_t>> positions;
                std::vector<std::string> replacements;

                while (std::regex_search(searchStart, result.cend(), match, varRegex))
                {
                    std::string varName = match[1].str();
                    // Trim whitespace
                    varName.erase(0, varName.find_first_not_of(" \t\n\r"));
                    varName.erase(varName.find_last_not_of(" \t\n\r") + 1);

                    auto it = variables.find(varName);
                    if (it != variables.end())
                    {
                        size_t pos = std::distance(result.cbegin(), match[0].first);
                        positions.push_back({pos, match[0].length()});
                        replacements.push_back(it->second);
                        totalReplacements++;
                    }

                    searchStart = match.suffix().first;
                }

                // Apply replacements from end to beginning
                for (int i = positions.size() - 1; i >= 0; i--)
                {
                    result.replace(positions[i].first, positions[i].second, replacements[i]);
                }

                // Update the node text
                if (result != text)
                {
                    textNode.set(result.c_str());
                }
            }

            // Recurse into children
            for (pugi::xml_node child : node.children())
            {
                traverse(child);
            }
        };

        // Start traversal from root
        traverse(pImpl_->doc.document_element());

        return totalReplacements;
    }

    std::string XmlDocument::getTextContent() const
    {
        if (!pImpl_->valid)
        {
            return "";
        }

        std::ostringstream oss;

        // Recursive function to collect text
        std::function<void(pugi::xml_node)> traverse = [&](pugi::xml_node node)
        {
            std::string text = node.text().as_string();
            if (!text.empty())
            {
                oss << text << " ";
            }

            // Recurse into children
            for (pugi::xml_node child : node.children())
            {
                traverse(child);
            }
        };

        traverse(pImpl_->doc.document_element());

        return oss.str();
    }

    XmlDocument::XmlNode XmlDocument::getNode(const std::string &xpath) const
    {
        XmlNode result;

        if (!pImpl_->valid)
        {
            return result;
        }

        try
        {
            pugi::xpath_node xpathNode = pImpl_->doc.select_node(xpath.c_str());
            pugi::xml_node node = xpathNode.node();

            if (node)
            {
                result.name = node.name();
                result.value = node.text().as_string();
                result.path = xpath;

                for (const auto &attr : node.attributes())
                {
                    result.attributes[attr.name()] = attr.value();
                }
            }
        }
        catch (const pugi::xpath_exception &e)
        {
            const_cast<XmlDocument *>(this)->lastError_ = std::string("XPath error: ") + e.what();
        }

        return result;
    }

    bool XmlDocument::setNodeText(const std::string &xpath, const std::string &text)
    {
        if (!pImpl_->valid)
        {
            return false;
        }

        try
        {
            pugi::xpath_node xpathNode = pImpl_->doc.select_node(xpath.c_str());
            pugi::xml_node node = xpathNode.node();

            if (node)
            {
                node.text().set(text.c_str());
                return true;
            }
        }
        catch (const pugi::xpath_exception &e)
        {
            lastError_ = std::string("XPath error: ") + e.what();
        }

        return false;
    }

    std::string XmlDocument::getAttributeValue(const std::string &xpath, const std::string &attributeName) const
    {
        if (!pImpl_->valid)
        {
            return "";
        }

        try
        {
            pugi::xpath_node xpathNode = pImpl_->doc.select_node(xpath.c_str());
            pugi::xml_node node = xpathNode.node();

            if (node)
            {
                return node.attribute(attributeName.c_str()).value();
            }
        }
        catch (const pugi::xpath_exception &e)
        {
            const_cast<XmlDocument *>(this)->lastError_ = std::string("XPath error: ") + e.what();
        }

        return "";
    }

    bool XmlDocument::setAttributeValue(const std::string &xpath, const std::string &attributeName, const std::string &value)
    {
        if (!pImpl_->valid)
        {
            return false;
        }

        try
        {
            pugi::xpath_node xpathNode = pImpl_->doc.select_node(xpath.c_str());
            pugi::xml_node node = xpathNode.node();

            if (node)
            {
                pugi::xml_attribute attr = node.attribute(attributeName.c_str());
                if (attr)
                {
                    attr.set_value(value.c_str());
                }
                else
                {
                    node.append_attribute(attributeName.c_str()).set_value(value.c_str());
                }
                return true;
            }
        }
        catch (const pugi::xpath_exception &e)
        {
            lastError_ = std::string("XPath error: ") + e.what();
        }

        return false;
    }

    bool XmlDocument::isValid() const
    {
        return pImpl_->valid;
    }

    std::string XmlDocument::getNodeXml(const std::string &xpath) const
    {
        if (!pImpl_->valid)
        {
            return "";
        }

        try
        {
            pugi::xpath_node xpathNode = pImpl_->doc.select_node(xpath.c_str());
            pugi::xml_node node = xpathNode.node();

            if (node)
            {
                std::ostringstream oss;
                node.print(oss, "", pugi::format_raw);
                return oss.str();
            }
        }
        catch (const pugi::xpath_exception &)
        {
            // Ignore errors in const method
        }

        return "";
    }

    bool XmlDocument::replaceNode(const std::string &xpath, const std::string &newXml)
    {
        if (!pImpl_->valid)
        {
            return false;
        }

        try
        {
            pugi::xpath_node xpathNode = pImpl_->doc.select_node(xpath.c_str());
            pugi::xml_node node = xpathNode.node();

            if (node)
            {
                pugi::xml_node parent = node.parent();
                if (!parent)
                {
                    return false;
                }

                // Parse new XML
                pugi::xml_document tempDoc;
                pugi::xml_parse_result result = tempDoc.load_string(newXml.c_str(), pugi::parse_default | pugi::parse_fragment);

                if (!result)
                {
                    lastError_ = "Invalid XML in replaceNode";
                    return false;
                }

                // Insert new nodes before the old node
                for (pugi::xml_node child : tempDoc.children())
                {
                    parent.insert_copy_before(child, node);
                }

                // Remove old node
                parent.remove_child(node);

                return true;
            }
        }
        catch (const pugi::xpath_exception &e)
        {
            lastError_ = std::string("XPath error: ") + e.what();
        }

        return false;
    }

    std::string XmlDocument::getTextContent(const std::string &xpath) const
    {
        if (!pImpl_->valid)
        {
            return "";
        }

        try
        {
            pugi::xpath_node xpathNode = pImpl_->doc.select_node(xpath.c_str());
            pugi::xml_node node = xpathNode.node();

            if (node)
            {
                std::string result;
                std::function<void(const pugi::xml_node &)> collectText = [&](const pugi::xml_node &n)
                {
                    for (const auto &child : n.children())
                    {
                        if (child.type() == pugi::node_pcdata || child.type() == pugi::node_cdata)
                        {
                            result += child.value();
                        }
                        else
                        {
                            collectText(child);
                        }
                    }
                };

                collectText(node);
                return result;
            }
        }
        catch (const pugi::xpath_exception &)
        {
            // Ignore errors in const method
        }

        return "";
    }

    std::string XmlDocument::getLastError() const
    {
        return lastError_;
    }

    void XmlDocument::clear()
    {
        if (pImpl_)
        {
            pImpl_->doc.reset();
            pImpl_->valid = false;
        }
        lastError_ = "";
    }

} // namespace json2doc
