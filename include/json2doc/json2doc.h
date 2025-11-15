#ifndef JSON2DOC_H
#define JSON2DOC_H

#include <string>

namespace json2doc
{
    class Json2Doc
    {
    public:
        Json2Doc();
        ~Json2Doc();
        std::string getVersion() const;
        bool loadJson(const std::string &jsonData);
        std::string convertToDocument(const std::string &templatePath);

    private:
        std::string jsonData_;
    };

} // namespace json2doc

#endif // JSON2DOC_H
