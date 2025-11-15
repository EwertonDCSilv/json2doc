#ifndef DOCX_READER_H
#define DOCX_READER_H

#include <string>
#include <vector>
#include <map>

namespace json2doc
{

    class DocxReader
    {
    public:
        DocxReader();
        ~DocxReader();
        std::string getTempPath() const;
        std::string getLastError() const;
        bool open(const std::string &filePath);
        void cleanup();
        std::string readDocumentXml();
        bool writeDocumentXml(const std::string &xmlContent);
        bool decompress();
        bool recompress(const std::string &outputPath);

        // TODO: remove this method
        std::string parseXmlContent();
        void printXml() const;

    private:
        std::string filePath_;
        std::string tempPath_;
        std::string xmlContent_;
        std::string lastError_;
        bool isOpen_;

        std::string createTempDirectory();

        // TODO: remove this method
        std::vector<std::string> extractTagContent(const std::string &xml, const std::string &tag);
    };

} // namespace json2doc

#endif // DOCX_READER_H
