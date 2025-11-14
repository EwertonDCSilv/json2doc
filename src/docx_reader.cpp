#include "json2doc/docx_reader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

namespace json2doc
{

    DocxReader::DocxReader()
        : filePath_(""), tempPath_(""), xmlContent_(""), lastError_(""), isOpen_(false)
    {
    }

    DocxReader::~DocxReader()
    {
        cleanup();
    }

    bool DocxReader::open(const std::string &filePath)
    {
        filePath_ = filePath;

        // Check if file exists
        std::ifstream file(filePath);
        if (!file.good())
        {
            lastError_ = "File does not exist: " + filePath;
            isOpen_ = false;
            return false;
        }
        file.close();

        isOpen_ = true;
        lastError_ = "";
        return true;
    }

    std::string DocxReader::createTempDirectory()
    {
        // Create temp directory in /tmp
        char tempTemplate[] = "/tmp/docx_reader_XXXXXX";
        char *tempDir = mkdtemp(tempTemplate);

        if (tempDir == nullptr)
        {
            lastError_ = "Failed to create temporary directory";
            return "";
        }

        return std::string(tempDir);
    }

    bool DocxReader::decompress()
    {
        if (!isOpen_)
        {
            lastError_ = "No file is currently open";
            return false;
        }

        // Create temporary directory
        tempPath_ = createTempDirectory();
        if (tempPath_.empty())
        {
            return false;
        }

        // Use unzip command to extract the DOCX file
        std::string command = "unzip -q -o \"" + filePath_ + "\" -d \"" + tempPath_ + "\" 2>&1";

        FILE *pipe = popen(command.c_str(), "r");
        if (!pipe)
        {
            lastError_ = "Failed to execute unzip command";
            return false;
        }

        char buffer[256];
        std::string result = "";
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }

        int status = pclose(pipe);

        if (status != 0)
        {
            lastError_ = "Unzip failed: " + result;
            return false;
        }

        return true;
    }

    std::string DocxReader::getTempPath() const
    {
        return tempPath_;
    }

    std::string DocxReader::readDocumentXml()
    {
        if (tempPath_.empty())
        {
            lastError_ = "File not decompressed yet";
            return "";
        }

        // The main document is at word/document.xml
        std::string documentPath = tempPath_ + "/word/document.xml";

        std::ifstream xmlFile(documentPath);
        if (!xmlFile.is_open())
        {
            lastError_ = "Failed to open document.xml at: " + documentPath;
            return "";
        }

        std::stringstream buffer;
        buffer << xmlFile.rdbuf();
        xmlContent_ = buffer.str();
        xmlFile.close();

        return xmlContent_;
    }

    std::vector<std::string> DocxReader::extractTagContent(const std::string &xml, const std::string &tag)
    {
        std::vector<std::string> results;
        std::string openTag = "<" + tag;
        std::string closeTag = "</" + tag + ">";

        size_t pos = 0;
        while ((pos = xml.find(openTag, pos)) != std::string::npos)
        {
            // Find the end of the opening tag
            size_t tagEnd = xml.find(">", pos);
            if (tagEnd == std::string::npos)
                break;

            // Find the closing tag
            size_t closePos = xml.find(closeTag, tagEnd);
            if (closePos == std::string::npos)
                break;

            // Extract content between tags
            std::string content = xml.substr(tagEnd + 1, closePos - tagEnd - 1);
            if (!content.empty())
            {
                results.push_back(content);
            }

            pos = closePos + closeTag.length();
        }

        return results;
    }

    std::string DocxReader::parseXmlContent()
    {
        if (xmlContent_.empty())
        {
            readDocumentXml();
        }

        if (xmlContent_.empty())
        {
            return "";
        }

        // Extract text content from <w:t> tags (Word text runs)
        std::vector<std::string> textElements = extractTagContent(xmlContent_, "w:t");

        std::string result;
        for (const auto &text : textElements)
        {
            result += text + " ";
        }

        return result;
    }

    void DocxReader::printXml() const
    {
        if (xmlContent_.empty())
        {
            std::cout << "No XML content loaded. Call readDocumentXml() first.\n";
            return;
        }

        std::cout << "=== DOCX XML Content ===\n";
        std::cout << xmlContent_ << "\n";
        std::cout << "=== End of XML ===\n";
    }

    std::string DocxReader::getLastError() const
    {
        return lastError_;
    }

    void DocxReader::cleanup()
    {
        if (tempPath_.empty())
        {
            return;
        }

        // Remove temporary directory and its contents
        std::string command = "rm -rf \"" + tempPath_ + "\"";
        int result = system(command.c_str());

        if (result == 0)
        {
            tempPath_ = "";
        }
    }

} // namespace json2doc
