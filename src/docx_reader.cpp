#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "json2doc/docx_reader.h"

/**
 * @brief Implementation of DocxReader class for handling DOCX files
 *
 * @author Ewerton Silva Santos <ewerton_dc@hotmail.com> - GitHub: EwertonDCSilv
 * @details This class provides methods to:
 * - Open a DOCX file
 * - Decompress the DOCX (ZIP) file to a temporary directory
 * - Read and write the main document XML
 * - Recompress the modified contents back into a DOCX file
 * - Clean up temporary files and directories
 */
namespace json2doc
{
    /**
     * @brief Construct a new DocxReader object
     *
     * @details Initializes member variables to default values
     */
    DocxReader::DocxReader()
        : filePath_(""), tempPath_(""), xmlContent_(""), lastError_(""), isOpen_(false)
    {
    }

    /**
     * @brief Destroy the DocxReader object
     */
    DocxReader::~DocxReader()
    {
        cleanup();
    }

    /**
     * @brief Get the path to the temporary directory used for decompression
     *
     * @return std::string The temporary directory path
     */
    std::string DocxReader::getTempPath() const
    {
        return this->tempPath_;
    }

    /**
     * @brief Get the last error message
     *
     * @return std::string The last error message
     */
    std::string DocxReader::getLastError() const
    {
        return lastError_;
    }

    /**
     * @brief Open a DOCX file
     * @param filePath Path to the DOCX file
     * @return true if the file was opened successfully, false otherwise
     */
    bool DocxReader::open(const std::string &filePath)
    {
        this->filePath_ = filePath;
        std::ifstream file(filePath);
        if (!file.good())
        {
            this->lastError_ = "File does not exist: " + filePath;
            this->isOpen_ = false;
            return false;
        }
        file.close();

        this->isOpen_ = true;
        this->lastError_ = "";
        return true;
    }

    /**
     * @brief Create a temporary directory for extracting DOCX contents
     *
     * @return Path to the created temporary directory or empty string on failure
     */
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

    /**
     * @brief Clean up temporary files and directories
     *
     * @details Removes the temporary directory created during decompression
     * if it exists, and resets the tempPath_ member variable.
     */
    void DocxReader::cleanup()
    {
        std::string command;
        if (this->tempPath_.empty())
        {
            return;
        }

        command = "rm -rf \"" + this->tempPath_ + "\"";
        if (system(command.c_str()) == 0)
        {
            this->tempPath_ = "";
        }
    }

    /**
     * @brief Read the main document XML content from the decompressed DOCX
     *
     * @return std::string The XML content as a string, or empty string on failure
     */
    std::string DocxReader::readDocumentXml()
    {
        if (this->tempPath_.empty())
        {
            this->lastError_ = "File not decompressed yet";
            return "";
        }

        // The main document is at word/document.xml
        std::string documentPath = this->tempPath_ + "/word/document.xml";

        std::ifstream xmlFile(documentPath);
        if (!xmlFile.is_open())
        {
            this->lastError_ = "Failed to open document.xml at: " + documentPath;
            return "";
        }

        std::stringstream buffer;
        buffer << xmlFile.rdbuf();
        this->xmlContent_ = buffer.str();
        xmlFile.close();

        return this->xmlContent_;
    }

    /**
     * @brief Write modified XML content back to the main document XML
     *
     * @param xmlContent The modified XML content to write
     * @return true if writing was successful, false otherwise
     */
    bool DocxReader::writeDocumentXml(const std::string &xmlContent)
    {
        if (this->tempPath_.empty())
        {
            this->lastError_ = "File not decompressed yet";
            return false;
        }

        // The main document is at word/document.xml
        std::string documentPath = this->tempPath_ + "/word/document.xml";

        std::ofstream xmlFile(documentPath);
        if (!xmlFile.is_open())
        {
            this->lastError_ = "Failed to open document.xml for writing at: " + documentPath;
            return false;
        }

        xmlFile << xmlContent;
        xmlFile.close();
        this->xmlContent_ = xmlContent;

        return true;
    }

    /**
     * @brief Decompress the opened DOCX file into a temporary directory
     *
     * @return true if decompression was successful, false otherwise
     */
    bool DocxReader::decompress()
    {
        char buffer[256];
        std::string result = "";
        int status = 0;

        if (!this->isOpen_)
        {
            this->lastError_ = "No file is currently open";
            return false;
        }

        this->tempPath_ = createTempDirectory();
        if (this->tempPath_.empty())
        {
            return false;
        }

        // Use unzip command to extract the DOCX file
        // TODO: Replace system call with a proper ZIP library for better portability and error handling
        std::string command = "unzip -q -o \"" + this->filePath_ + "\" -d \"" + this->tempPath_ + "\" 2>&1";
        FILE *pipe = popen(command.c_str(), "r");

        if (!pipe)
        {
            this->lastError_ = "Failed to execute unzip command";
            return false;
        }

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }

        status = pclose(pipe);
        if (status != 0)
        {
            this->lastError_ = "Unzip failed: " + result;
            return false;
        }

        return true;
    }

    /**
     * @brief Recompress the modified contents back into a DOCX file
     *
     * @param outputPath Path to save the recompressed DOCX file
     * @return true if recompression was successful, false otherwise
     */
    bool DocxReader::recompress(const std::string &outputPath)
    {
        char cwd[1024];
        char buffer[256];
        std::string absOutputPath = outputPath;
        std::string command;
        std::string result = "";

        if (this->tempPath_.empty())
        {
            this->lastError_ = "No temporary directory to compress";
            return false;
        }

        // Convert output path to absolute if it's relative
        if (outputPath[0] != '/' && ((getcwd(cwd, sizeof(cwd)) != nullptr)))
        {
            absOutputPath = std::string(cwd) + "/" + outputPath;
        }

        // Change to temp directory and zip contents
        // We need to zip from inside the temp directory to avoid including the temp path itself
        command = "cd \"" + this->tempPath_ + "\" && zip -q -r \"" + absOutputPath + "\" . 2>&1";

        // TODO: Replace system call with a proper ZIP library for better portability and error handling
        FILE *pipe = popen(command.c_str(), "r");
        if (!pipe)
        {
            this->lastError_ = "Failed to execute zip command";
            return false;
        }

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }

        if (pclose(pipe) != 0)
        {
            this->lastError_ = "Zip failed: " + result;
            return false;
        }

        return true;
    }

    /**
     * @brief Extract content between specified XML tags
     *
     * @deprecated This method is deprecated, use xmlParseLib
     * @param xml The XML string to search within
     * @param tag The XML tag to extract content from
     * @return std::vector<std::string> List of contents found between the specified tags
     */
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

    /**
     * @brief Parse the XML content to extract text elements
     *
     * @deprecated This method is deprecated, use xmlParseLib
     * @return std::string Concatenated text content extracted from the XML
     */
    std::string DocxReader::parseXmlContent()
    {
        if (this->xmlContent_.empty())
        {
            readDocumentXml();
        }

        if (this->xmlContent_.empty())
        {
            return "";
        }

        // Extract text content from <w:t> tags (Word text runs)
        std::vector<std::string> textElements = extractTagContent(this->xmlContent_, "w:t");

        std::string result;
        for (const auto &text : textElements)
        {
            result += text + " ";
        }

        return result;
    }

    /**
     * @brief Print the loaded XML content to standard output
     *
     * @details If no XML content is loaded, informs the user to call readDocumentXml() first
     */
    void DocxReader::printXml() const
    {
        if (this->xmlContent_.empty())
        {
            std::cout << "No XML content loaded. Call readDocumentXml() first.\n";
            return;
        }

        std::cout << "=== DOCX XML Content ===\n";
        std::cout << this->xmlContent_ << "\n";
        std::cout << "=== End of XML ===\n";
    }

} // namespace json2doc
