#ifndef JSON2DOC_HELP_H
#define JSON2DOC_HELP_H

#include <string>

namespace json2doc
{

    class Help
    {
    public:
        static std::string getUsageMessage(const char *programName);
        static std::string getHelpMessage();
    };

} // namespace json2doc

#endif // JSON2DOC_HELP_H
