#include "json2doc/args_parser.h"
#include <cstring>

/**
 * @author Ewerton Silva Santos <ewerton_dc@hotmail.com> - GitHub: EwertonDCSilv
 * @brief Command-line arguments parser for json2doc CLI
 * @namespace json2doc
 * @details
 * This class handles parsing of command-line arguments for the json2doc
 * command-line interface (CLI) tool. It supports flags and options with values.
 */
namespace json2doc
{
    /**
     * @brief Construct a new ArgsParser object and parse arguments
     * @param argc Argument count
     * @param argv Argument vector
     */
    ArgsParser::ArgsParser(int argc, char *argv[]) : valid(true)
    {
        parse(argc, argv);
    }

    /**
     * @brief Parse command-line arguments
     * @param argc Argument count
     * @param argv Argument vector
     */
    void ArgsParser::parse(int argc, char *argv[])
    {
        std::string argument;
        std::string option;
        std::string value;

        for (int i = 1; i < argc; ++i)
        {
            argument = argv[i];

            if (argument == "--help" || argument == "-h")
            {
                flags["help"] = true;
            }
            else if (argument == "--version" || argument == "-v")
            {
                flags["version"] = true;
            }
            else if (isOption(argument))
            {
                option = argument;

                // Check if there's a value following this option
                if (i + 1 < argc && !isOption(argv[i + 1]))
                {
                    ++i;
                    value = argv[i];

                    // Remove leading dashes from option name
                    if (option.substr(0, 2) == "--")
                    {
                        option = option.substr(2);
                    }
                    else if (option[0] == '-')
                    {
                        option = option.substr(1);
                    }

                    this->options[option] = value;
                }
                else
                {
                    this->valid = false;
                    this->errorMessage = "Option " + option + " requires a value";
                    return;
                }
            }
            else
            {
                this->valid = false;
                this->errorMessage = "Unknown argument: " + argument;
                return;
            }
        }
    }

    /**
     * @brief Check if argument is an option (starts with - or --)
     * @param arg The argument string
     * @return true if it's an option
     * @return false otherwise
     */
    bool ArgsParser::isOption(const std::string &arg) const
    {
        return arg.length() > 1 && arg[0] == '-';
    }

    /**
     * @brief Check if a flag is set
     * @param flag The flag name
     * @return true if the flag is set
     * @return false otherwise
     */
    bool ArgsParser::hasFlag(const std::string &flag) const
    {
        auto it = flags.find(flag);
        return it != flags.end() && it->second;
    }

    /**
     * @brief Get the value of an option
     * @param option The option name
     * @return std::string The option value or empty string if not found
     */
    std::string ArgsParser::getValue(const std::string &option) const
    {
        auto it = this->options.find(option);
        if (it != this->options.end())
        {
            return it->second;
        }
        return "";
    }

    /**
     * @brief Check if the parsed arguments are valid
     * @return true if valid
     * @return false otherwise
     */
    bool ArgsParser::isValid() const
    {
        return this->valid;
    }

    /**
     * @brief Get the error message if parsing failed
     * @return std::string The error message
     */
    std::string ArgsParser::getErrorMessage() const
    {
        return this->errorMessage;
    }

} // namespace json2doc