#ifndef DOCX_READER_H
#define DOCX_READER_H

#include <string>
#include <vector>
#include <map>

namespace json2doc
{

    /**
     * @brief Class for reading and parsing DOCX files
     *
     * This class handles DOCX files by:
     * - Decompressing the DOCX (ZIP) file
     * - Extracting XML content
     * - Parsing and interpreting the XML structure
     */
    class DocxReader
    {
    public:
        /**
         * @brief Construct a new DocxReader object
         */
        DocxReader();

        /**
         * @brief Destroy the DocxReader object and clean up temp files
         */
        ~DocxReader();

        /**
         * @brief Open and read a DOCX file
         *
         * @param filePath Path to the DOCX file
         * @return true if the file was successfully opened
         * @return false if the file could not be opened
         */
        bool open(const std::string &filePath);

        /**
         * @brief Decompress the DOCX file to a temporary directory
         *
         * @return true if decompression was successful
         * @return false if decompression failed
         */
        bool decompress();

        /**
         * @brief Get the path to the temporary extraction directory
         *
         * @return std::string Path to temp directory
         */
        std::string getTempPath() const;

        /**
         * @brief Read the main document XML content
         *
         * @return std::string The XML content from document.xml
         */
        std::string readDocumentXml();

        /**
         * @brief Write modified XML content back to document.xml
         *
         * @param xmlContent The modified XML content to write
         * @return true if write was successful
         * @return false if write failed
         */
        bool writeDocumentXml(const std::string &xmlContent);

        /**
         * @brief Recompress the temporary directory back into a DOCX file
         *
         * @param outputPath Path where the output DOCX file should be saved
         * @return true if recompression was successful
         * @return false if recompression failed
         */
        bool recompress(const std::string &outputPath);

        /**
         * @brief Parse the document XML and extract text content
         *
         * @return std::string Extracted text content
         */
        std::string parseXmlContent();

        /**
         * @brief Print the XML structure to stdout
         */
        void printXml() const;

        /**
         * @brief Get the last error message
         *
         * @return std::string The error message
         */
        std::string getLastError() const;

        /**
         * @brief Clean up temporary files and directories
         */
        void cleanup();

    private:
        std::string filePath_;
        std::string tempPath_;
        std::string xmlContent_;
        std::string lastError_;
        bool isOpen_;

        /**
         * @brief Create a unique temporary directory
         *
         * @return std::string Path to created temp directory
         */
        std::string createTempDirectory();

        /**
         * @brief Extract a simple text value between XML tags
         *
         * @param xml The XML content
         * @param tag The tag name to search for
         * @return std::vector<std::string> Vector of found values
         */
        std::vector<std::string> extractTagContent(const std::string &xml, const std::string &tag);
    };

} // namespace json2doc

#endif // DOCX_READER_H
