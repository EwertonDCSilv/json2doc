#ifndef JSON2DOC_ARGS_PARSER_H
#define JSON2DOC_ARGS_PARSER_H

#include <string>
#include <map>

namespace json2doc
{

    class ArgsParser
    {
    public:
        ArgsParser(int argc, char *argv[]);

        bool hasFlag(const std::string &flag) const;
        std::string getValue(const std::string &option) const;
        bool isValid() const;
        std::string getErrorMessage() const;

    private:
        std::map<std::string, std::string> options;
        std::map<std::string, bool> flags;
        bool valid;
        std::string errorMessage;

        void parse(int argc, char *argv[]);
        bool isOption(const std::string &arg) const;
    };

} // namespace json2doc

#endif // JSON2DOC_ARGS_PARSER_H
