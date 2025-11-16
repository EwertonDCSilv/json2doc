#include "json2doc/help.h"
#include <sstream>

/**
 * @author Ewerton Silva Santos <ewerton_dc@hotmai.com> - GitHub: EwertonDCSilv
 * @brief Help and usage message generator for json2doc CLI
 * @namespace json2doc
 * @details
 * This class provides static methods to generate usage and help messages
 * for the json2doc command-line interface (CLI) tool.
 */
namespace json2doc
{

    std::string Help::getUsageMessage(const char *programName)
    {
        std::ostringstream oss;
        oss << "Usage: " << programName << " --doc <template_path> --json <json_file>\n"
            << "       " << programName << " --help\n"
            << "       " << programName << " --version\n";
        return oss.str();
    }

    std::string Help::getHelpMessage()
    {
        std::ostringstream oss;
        oss << "\n"
            << "╔═══════════════════════════════════════════════════════════════╗\n"
            << "║                       JSON2Doc Converter                      ║\n"
            << "╚═══════════════════════════════════════════════════════════════╝\n"
            << "\n"
            << "DESCRIPTION:\n"
            << "  Convert JSON data into formatted documents using templates.\n"
            << "\n"
            << "USAGE:\n"
            << "  json2doc --doc <template_path> --json <json_file>\n"
            << "  json2doc --help\n"
            << "  json2doc --version\n"
            << "\n"
            << "REQUIRED OPTIONS:\n"
            << "  --doc, -d <path>   Path to the document template file (.docx)\n"
            << "  --json, -j <path>  Path to the JSON data file (.json)\n"
            << "\n"
            << "OTHER OPTIONS:\n"
            << "  -h, --help         Display this help message and exit\n"
            << "  -v, --version      Display version information and exit\n"
            << "\n"
            << "EXAMPLES:\n"
            << "  json2doc template.docx\n"
            << "  json2doc ./templates/report.docx\n"
            << "  json2doc --help\n"
            << "\n"
            << "For more information, visit: https://github.com/EwertonDCSilv/json2doc\n"
            << "\n";
        return oss.str();
    }

} // namespace json2doc
