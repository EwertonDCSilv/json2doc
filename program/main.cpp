#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>

#include "json2doc/json2doc.h"
#include "json2doc/converter.h"
#include "json2doc/help.h"
#include "json2doc/args_parser.h"

int main(int argc, char *argv[])
{
    json2doc::Json2Doc converter;
    std::string jsonData;
    std::string templatePath;
    std::string jsonFilePath;
    std::string result;
    std::ifstream jsonFile;
    std::stringstream buffer;
    json2doc::ArgsParser args(argc, argv);

    if (args.hasFlag("help"))
    {
        std::cout << json2doc::Help::getHelpMessage();
        return 0;
    }

    if (args.hasFlag("version"))
    {
        std::cout << "JSON2Doc version " << converter.getVersion() << "\n";
        return 0;
    }

    if (!args.isValid())
    {
        std::cerr << "❌ Error: " << args.getErrorMessage() << "\n\n";
        std::cerr << json2doc::Help::getUsageMessage(argv[0]);
        std::cerr << "\nTip: Use --help for more information\n";
        return 1;
    }

    templatePath = args.getValue("doc");
    if (templatePath.empty())
    {
        templatePath = args.getValue("d");
    }

    jsonFilePath = args.getValue("json");
    if (jsonFilePath.empty())
    {
        jsonFilePath = args.getValue("j");
    }

    if (templatePath.empty())
    {
        std::cerr << "❌ Error: Missing required arguments\n\n";
        std::cerr << json2doc::Help::getUsageMessage(argv[0]);
        std::cerr << "\nTip: Both --doc isrequired\n";
        return 1;
    }
    if (jsonFilePath.empty())
    {
        std::cerr << "❌ Error: Missing required arguments\n\n";
        std::cerr << json2doc::Help::getUsageMessage(argv[0]);
        std::cerr << "\nTip: Both --json is required\n";
        return 1;
    }

    // Run conversion
    std::cout << "┌────────────────────────────────────────┐\n";
    std::cout << "│      JSON2Doc - Document Converter     │\n";
    std::cout << "└────────────────────────────────────────┘\n\n";
    std::cout << "📦 Version: " << converter.getVersion() << "\n\n";

    // Read JSON file
    std::cout << "📂 Reading JSON file: " << jsonFilePath << "\n";
    jsonFile.open(jsonFilePath);
    if (!jsonFile.is_open())
    {
        std::cerr << "✗ Failed to open JSON file: " << jsonFilePath << "\n";
        return 1;
    }

    buffer << jsonFile.rdbuf();
    jsonData = buffer.str();
    jsonFile.close();
    std::cout << "✓ JSON file loaded\n";

    // Validate JSON
    if (json2doc::Converter::isValidJson(jsonData))
    {
        std::cout << "✓ JSON is valid\n";
    }
    else
    {
        std::cerr << "✗ JSON is invalid\n";
        return 1;
    }

    // Load JSON data
    if (converter.loadJson(jsonData))
    {
        std::cout << "✓ JSON loaded successfully\n\n";
    }
    else
    {
        std::cerr << "✗ Failed to load JSON\n";
        return 1;
    }

    // Convert to document
    std::cout << "📄 Template: " << templatePath << "\n";
    std::cout << "🔄 Converting...\n\n";

    result = converter.convertToDocument(templatePath);

    std::cout << "─────────────────────────────────────────\n";
    std::cout << "Conversion Result:\n";
    std::cout << result << "\n";
    std::cout << "─────────────────────────────────────────\n";
    std::cout << "\n✨ Done!\n";

    return 0;
}
