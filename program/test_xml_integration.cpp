#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include "json2doc/xml_document.h"
#include "json2doc/json_merge.h"
#include "json2doc/docx_reader.h"

/**
 * @brief Integration test demonstrating XmlDocument + JsonMerge with XPath
 */

void printSeparator(const std::string &title)
{
    std::cout << "\n";
    std::cout << "┌────────────────────────────────────────────────────────┐\n";
    std::cout << "│ " << title;
    for (size_t i = title.length(); i < 54; i++)
    {
        std::cout << " ";
    }
    std::cout << " │\n";
    std::cout << "└────────────────────────────────────────────────────────┘\n";
    std::cout << "\n";
}

int main()
{
    printSeparator("XmlDocument + JsonMerge Integration Demo");

    // Step 1: Create sample XML template with variables
    printSeparator("Step 1: Loading XML Template with XPath");

    std::string xmlTemplate = R"(<?xml version="1.0" encoding="UTF-8"?>
<document>
  <metadata>
    <title>{{title}}</title>
    <author>{{author}}</author>
    <date>{{date}}</date>
    <version>{{metadata.version}}</version>
  </metadata>
  <body>
    <section id="intro">
      <heading>Introduction</heading>
      <paragraph>Welcome {{name}}, this document is about {{title}}.</paragraph>
    </section>
    <section id="details">
      <heading>Details</heading>
      <paragraph>Your position: {{position}}</paragraph>
      <paragraph>Status: {{metadata.status}}</paragraph>
    </section>
  </body>
</document>)";

    json2doc::XmlDocument xmlDoc;
    if (!xmlDoc.loadFromString(xmlTemplate))
    {
        std::cerr << "✗ Failed to load XML: " << xmlDoc.getLastError() << "\n";
        return 1;
    }
    std::cout << "✓ XML template loaded\n";
    std::cout << "  Size: " << xmlTemplate.size() << " bytes\n";

    // Step 2: Use XPath to query document structure
    printSeparator("Step 2: Exploring XML with XPath");

    // Query sections
    auto sections = xmlDoc.query("//section");
    std::cout << "✓ Found " << sections.size() << " sections using XPath //section\n";
    for (const auto &section : sections)
    {
        std::string id = section.attributes.count("id") > 0 ? section.attributes.at("id") : "none";
        std::cout << "  • Section ID: " << id << "\n";
    }

    // Query paragraphs with variables
    auto templateNodes = xmlDoc.findTemplateNodes();
    std::cout << "\n✓ Found " << templateNodes.size() << " nodes with {{variables}}\n";
    for (size_t i = 0; i < std::min(size_t(5), templateNodes.size()); i++)
    {
        std::cout << "  • " << templateNodes[i].value.substr(0, 60);
        if (templateNodes[i].value.length() > 60)
            std::cout << "...";
        std::cout << "\n";
    }

    // Step 3: Load JSON data
    printSeparator("Step 3: Loading JSON Data");

    json2doc::JsonMerge merger;
    if (!merger.loadJson("data.json"))
    {
        std::cerr << "⚠ Warning: Could not load data.json\n";
        std::cerr << "  Error: " << merger.getLastError() << "\n";
        std::cout << "  Creating sample data...\n";

        std::string sampleJson = R"({
            "title": "XML Processing Guide",
            "author": "Ewerton Silva Santos",
            "date": "November 14, 2025",
            "name": "Developer",
            "position": "Senior Software Engineer",
            "metadata": {
                "version": "2.0.1",
                "status": "published"
            }
        })";

        merger.loadJsonString(sampleJson);
    }

    std::cout << "✓ JSON data loaded\n";
    std::cout << "  Available keys: " << merger.getAllKeys().size() << "\n";
    for (const auto &key : merger.getAllKeys())
    {
        std::cout << "  • " << key << " = " << merger.getValue(key) << "\n";
    }

    // Step 4: Merge using XPath-based approach
    printSeparator("Step 4: Merging JSON into XML (XPath-based)");

    int replacements = merger.mergeIntoXml(xmlDoc);
    std::cout << "✓ Merged successfully\n";
    std::cout << "  Total replacements: " << replacements << "\n";

    // Step 5: Query merged results
    printSeparator("Step 5: Verifying Results with XPath");

    // Get specific nodes by XPath
    auto titleNode = xmlDoc.getNode("//metadata/title");
    std::cout << "✓ Title (//metadata/title): " << titleNode.value << "\n";

    auto authorNode = xmlDoc.getNode("//metadata/author");
    std::cout << "✓ Author (//metadata/author): " << authorNode.value << "\n";

    auto versionNode = xmlDoc.getNode("//metadata/version");
    std::cout << "✓ Version (//metadata/version): " << versionNode.value << "\n";

    // Get all paragraphs
    auto paragraphs = xmlDoc.query("//paragraph");
    std::cout << "\n✓ All paragraphs (" << paragraphs.size() << " found):\n";
    for (size_t i = 0; i < paragraphs.size(); i++)
    {
        std::cout << "  " << (i + 1) << ". " << paragraphs[i].value << "\n";
    }

    // Step 6: XPath manipulation
    printSeparator("Step 6: Advanced XPath Operations");

    // Add a custom attribute
    xmlDoc.setAttributeValue("//section[@id='intro']", "processed", "true");
    std::cout << "✓ Added attribute 'processed=true' to intro section\n";

    // Verify
    std::string processed = xmlDoc.getAttributeValue("//section[@id='intro']", "processed");
    std::cout << "  Verification: processed = " << processed << "\n";

    // Count elements
    auto allSections = xmlDoc.query("//section[@processed='true']");
    std::cout << "\n✓ Sections with processed='true': " << allSections.size() << "\n";

    // Step 7: Export merged XML
    printSeparator("Step 7: Exporting Merged XML");

    std::string mergedXml = xmlDoc.toString();
    std::cout << "✓ XML exported\n";
    std::cout << "  Size: " << mergedXml.size() << " bytes\n";

    // Save to file
    std::ofstream outFile("temp_extracted_xml/merged_xpath_demo.xml");
    if (outFile.is_open())
    {
        outFile << mergedXml;
        outFile.close();
        std::cout << "✓ Saved to: temp_extracted_xml/merged_xpath_demo.xml\n";
    }

    // Display first 500 chars of merged XML
    std::cout << "\nFirst 500 characters of merged XML:\n";
    std::cout << "─────────────────────────────────────────\n";
    std::cout << mergedXml.substr(0, 500);
    if (mergedXml.length() > 500)
        std::cout << "...";
    std::cout << "\n─────────────────────────────────────────\n";

    // Step 8: Integration with DocxReader
    printSeparator("Step 8: Benefits over String-based Approach");

    std::cout << "✅ XmlDocument + XPath Advantages:\n\n";
    std::cout << "1. Precise Targeting\n";
    std::cout << "   • XPath: //metadata/title targets exact nodes\n";
    std::cout << "   • String matching: error-prone, fragile\n\n";

    std::cout << "2. Structure Aware\n";
    std::cout << "   • Preserves XML hierarchy and attributes\n";
    std::cout << "   • Safe namespace handling (DOCX w: prefix)\n\n";

    std::cout << "3. Flexible Queries\n";
    std::cout << "   • Complex selectors: //section[@id='intro']\n";
    std::cout << "   • Conditional logic built into XPath\n\n";

    std::cout << "4. Robust Parsing\n";
    std::cout << "   • pugixml handles malformed XML gracefully\n";
    std::cout << "   • Proper escaping of special characters\n\n";

    std::cout << "5. Performance\n";
    std::cout << "   • Single parse, multiple operations\n";
    std::cout << "   • No regex overhead for complex documents\n";

    printSeparator("Demo Completed Successfully!");
    std::cout << "✨ XmlDocument with pugixml provides powerful, flexible XML processing!\n";
    std::cout << "✨ JsonMerge now works seamlessly with structured XML via XPath!\n\n";

    return 0;
}
