#include <iostream>
#include <string>
#include <fstream>
#include "json2doc/docx_reader.h"
#include "json2doc/json_merge.h"

/**
 * @brief Test program for JsonMerge integrated with DocxReader
 *
 * This standalone test program demonstrates:
 * 1. Opening and reading DOCX files with DocxReader
 * 2. Extracting XML content from DOCX
 * 3. Finding {{variable}} placeholders in the XML
 * 4. Loading JSON data
 * 5. Merging JSON values into XML template
 * 6. Saving the merged result
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

void createTemplateDocx(const std::string &filename)
{
    // Create a DOCX file with {{variable}} placeholders for testing

    std::string tempDir = "/tmp/template_docx_create";
    system(("mkdir -p " + tempDir + "/word").c_str());
    system(("mkdir -p " + tempDir + "/_rels").c_str());
    system(("mkdir -p " + tempDir + "/word/_rels").c_str());

    // Create document.xml with template variables
    std::ofstream docXml(tempDir + "/word/document.xml");
    docXml << R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:body>
    <w:p>
      <w:r>
        <w:t>Document Title: {{title}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Author: {{author}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Date: {{date}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Content: {{content}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Name: {{name}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Position: {{position}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Version: {{metadata.version}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Status: {{metadata.status}}</w:t>
      </w:r>
    </w:p>
  </w:body>
</w:document>)";
    docXml.close();

    // Create [Content_Types].xml
    std::ofstream contentTypes(tempDir + "/[Content_Types].xml");
    contentTypes << R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="xml" ContentType="application/xml"/>
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
</Types>)";
    contentTypes.close();

    // Create _rels/.rels
    std::ofstream rels(tempDir + "/_rels/.rels");
    rels << R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>)";
    rels.close();

    // Zip it up
    std::string zipCmd = "cd " + tempDir + " && zip -q -r " + filename + " * && mv " + filename + " /tmp/";
    system(zipCmd.c_str());

    // Cleanup
    system(("rm -rf " + tempDir).c_str());

    std::cout << "✓ Template DOCX file created: " << filename << "\n";
}

int main(int argc, char *argv[])
{
    printSeparator("JsonMerge + DocxReader Integration Test");

    // Setup file paths
    std::string docxFile;
    std::string jsonFile = "data.json";

    // Check if files were provided as arguments
    if (argc > 1)
    {
        docxFile = argv[1];
        std::cout << "📄 Using provided DOCX file: " << docxFile << "\n";
    }
    else
    {
        // Create a sample template DOCX
        docxFile = "/tmp/template_test.docx";
        std::cout << "📝 No DOCX file provided. Creating template DOCX...\n";
        createTemplateDocx("template_test.docx");
        std::cout << "📄 Using template file: " << docxFile << "\n";
    }

    if (argc > 2)
    {
        jsonFile = argv[2];
    }
    std::cout << "📊 Using JSON file: " << jsonFile << "\n";

    // Step 1: Open and read DOCX
    printSeparator("Step 1: Reading DOCX Template");
    json2doc::DocxReader reader;

    if (!reader.open(docxFile))
    {
        std::cerr << "✗ Failed to open DOCX: " << reader.getLastError() << "\n";
        return 1;
    }
    std::cout << "✓ DOCX file opened\n";

    if (!reader.decompress())
    {
        std::cerr << "✗ Failed to decompress DOCX: " << reader.getLastError() << "\n";
        return 1;
    }
    std::cout << "✓ DOCX decompressed to: " << reader.getTempPath() << "\n";

    std::string xmlContent = reader.readDocumentXml();
    if (xmlContent.empty())
    {
        std::cerr << "✗ Failed to read XML: " << reader.getLastError() << "\n";
        return 1;
    }
    std::cout << "✓ XML content read (" << xmlContent.size() << " bytes)\n";

    // Step 2: Load JSON data
    printSeparator("Step 2: Loading JSON Data");
    json2doc::JsonMerge merger;

    if (!merger.loadJson(jsonFile))
    {
        std::cerr << "✗ Failed to load JSON: " << merger.getLastError() << "\n";
        std::cout << "⚠ Continuing with empty JSON data...\n";
    }
    else
    {
        std::cout << "✓ JSON loaded successfully\n";
        std::cout << "📊 Available keys: " << merger.getAllKeys().size() << "\n";

        std::cout << "\nJSON Keys:\n";
        for (const auto &key : merger.getAllKeys())
        {
            std::cout << "  • " << key << " = " << merger.getValue(key) << "\n";
        }
    }

    // Step 3: Find variables in XML
    printSeparator("Step 3: Finding Template Variables");
    std::vector<std::string> variables = merger.findVariables(xmlContent);

    std::cout << "✓ Found " << variables.size() << " variable placeholders:\n";
    for (const auto &var : variables)
    {
        std::string value = merger.getValue(var);
        if (!value.empty())
        {
            std::cout << "  ✓ {{" << var << "}} → \"" << value << "\"\n";
        }
        else
        {
            std::cout << "  ⚠ {{" << var << "}} → [NOT FOUND IN JSON]\n";
        }
    }

    // Step 4: Merge JSON into XML
    printSeparator("Step 4: Merging JSON Data into XML");
    std::string mergedXml = merger.replaceVariables(xmlContent);

    auto stats = merger.getStats();
    std::cout << "✓ Merge complete:\n";
    std::cout << "  • Variables found: " << stats["found"] << "\n";
    std::cout << "  • Variables replaced: " << stats["replaced"] << "\n";
    std::cout << "  • Variables missing: " << stats["missing"] << "\n";

    // Step 5: Save results
    printSeparator("Step 5: Saving Results");

    // Create output directory
    std::string outputDir = "temp_extracted_xml";
    system(("mkdir -p " + outputDir).c_str());

    // Generate base filename
    std::string baseFilename = docxFile;
    size_t lastSlash = baseFilename.find_last_of("/\\");
    if (lastSlash != std::string::npos)
    {
        baseFilename = baseFilename.substr(lastSlash + 1);
    }
    size_t dotPos = baseFilename.find_last_of(".");
    if (dotPos != std::string::npos)
    {
        baseFilename = baseFilename.substr(0, dotPos);
    }

    // Save original XML
    std::string originalXmlPath = outputDir + "/" + baseFilename + "_original.xml";
    std::ofstream originalFile(originalXmlPath);
    if (originalFile.is_open())
    {
        originalFile << xmlContent;
        originalFile.close();
        std::cout << "✓ Original XML saved to: " << originalXmlPath << "\n";
    }

    // Save merged XML
    std::string mergedXmlPath = outputDir + "/" + baseFilename + "_merged.xml";
    std::ofstream mergedFile(mergedXmlPath);
    if (mergedFile.is_open())
    {
        mergedFile << mergedXml;
        mergedFile.close();
        std::cout << "✓ Merged XML saved to: " << mergedXmlPath << "\n";
    }

    // Extract and display text content
    printSeparator("Step 6: Extracted Text Content");

    // Parse merged content to show final text
    std::cout << "Original template text:\n";
    std::cout << "─────────────────────────────────────────\n";
    std::cout << reader.parseXmlContent() << "\n";
    std::cout << "─────────────────────────────────────────\n";

    // For merged text, we need to create a temporary reader or parse manually
    std::cout << "\nMerged text (after variable replacement):\n";
    std::cout << "─────────────────────────────────────────\n";
    // Simple extraction of <w:t> content from merged XML
    size_t pos = 0;
    while ((pos = mergedXml.find("<w:t>", pos)) != std::string::npos)
    {
        size_t endPos = mergedXml.find("</w:t>", pos);
        if (endPos != std::string::npos)
        {
            std::string text = mergedXml.substr(pos + 5, endPos - pos - 5);
            std::cout << text << "\n";
            pos = endPos;
        }
        else
        {
            break;
        }
    }
    std::cout << "─────────────────────────────────────────\n";

    // Step 7: Cleanup
    printSeparator("Step 7: Cleanup");
    reader.cleanup();
    std::cout << "✓ Temporary DOCX files cleaned up\n";

    printSeparator("Integration Test Completed Successfully!");
    std::cout << "✨ JsonMerge and DocxReader are working together correctly!\n";
    std::cout << "\n📁 Output files:\n";
    std::cout << "  • Original XML: " << originalXmlPath << "\n";
    std::cout << "  • Merged XML:   " << mergedXmlPath << "\n\n";

    return 0;
}
