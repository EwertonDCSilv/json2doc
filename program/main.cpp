#include <iostream>
#include "json2doc/json2doc.h"
#include "json2doc/converter.h"

int main() {
    std::cout << "JSON2Doc Library Example\n";
    std::cout << "========================\n\n";

    // Create an instance of Json2Doc
    json2doc::Json2Doc converter;
    
    // Display version
    std::cout << "Library Version: " << converter.getVersion() << "\n\n";

    // Example JSON data
    std::string jsonData = R"({
        "title": "Sample Document",
        "author": "John Doe",
        "content": "This is a sample document."
    })";

    // Validate JSON
    if (json2doc::Converter::isValidJson(jsonData)) {
        std::cout << "JSON is valid\n";
    } else {
        std::cout << "JSON is invalid\n";
        return 1;
    }

    // Load JSON data
    if (converter.loadJson(jsonData)) {
        std::cout << "JSON loaded successfully\n\n";
    } else {
        std::cout << "Failed to load JSON\n";
        return 1;
    }

    // Convert to document
    std::string templatePath = "template.docx";
    std::string result = converter.convertToDocument(templatePath);
    
    std::cout << "Conversion Result:\n";
    std::cout << result << "\n";

    return 0;
}
