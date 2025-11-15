#ifndef JSON2DOC_CONVERTER_H
#define JSON2DOC_CONVERTER_H

#include <string>

namespace json2doc
{

    class Converter
    {
    public:
        static std::string jsonToString(const std::string &json);
        static bool isValidJson(const std::string &json);
    };

} // namespace json2doc

#endif // JSON2DOC_CONVERTER_H
