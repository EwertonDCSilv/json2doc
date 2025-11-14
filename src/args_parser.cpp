#include "json2doc/args_parser.h"
#include <cstring>

namespace json2doc
{

    ArgsParser::ArgsParser(int argc, char *argv[]) : valid(true)
    {
        parse(argc, argv);
    }

    void ArgsParser::parse(int argc, char *argv[])
    {
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            // Check for flags (--help, -h, --version, -v)
            if (arg == "--help" || arg == "-h")
            {
                flags["help"] = true;
            }
            else if (arg == "--version" || arg == "-v")
            {
                flags["version"] = true;
            }
            // Check for options with values
            else if (isOption(arg))
            {
                std::string option = arg;

                // Check if there's a value following this option
                if (i + 1 < argc && !isOption(argv[i + 1]))
                {
                    ++i;
                    std::string value = argv[i];

                    // Remove leading dashes from option name
                    if (option.substr(0, 2) == "--")
                    {
                        option = option.substr(2);
                    }
                    else if (option[0] == '-')
                    {
                        option = option.substr(1);
                    }

                    options[option] = value;
                }
                else
                {
                    valid = false;
                    errorMessage = "Option " + option + " requires a value";
                    return;
                }
            }
            else
            {
                valid = false;
                errorMessage = "Unknown argument: " + arg;
                return;
            }
        }
    }

    bool ArgsParser::isOption(const std::string &arg) const
    {
        return arg.length() > 1 && arg[0] == '-';
    }

    bool ArgsParser::hasFlag(const std::string &flag) const
    {
        auto it = flags.find(flag);
        return it != flags.end() && it->second;
    }

    std::string ArgsParser::getValue(const std::string &option) const
    {
        auto it = options.find(option);
        if (it != options.end())
        {
            return it->second;
        }
        return "";
    }

    bool ArgsParser::isValid() const
    {
        return valid;
    }

    std::string ArgsParser::getErrorMessage() const
    {
        return errorMessage;
    }

} // namespace json2doc
