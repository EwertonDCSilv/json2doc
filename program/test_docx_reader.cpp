#include <iostream>
#include <string>
#include <fstream>
#include "json2doc/docx_reader.h"

/**
 * @brief Test program for DocxReader class
 *
 * This standalone test program demonstrates the usage of DocxReader
 * to open, decompress, and parse DOCX files.
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

void createSampleDocx(const std::string &filename)
{
    // Create a minimal DOCX file for testing
    // This creates the necessary structure with a simple document.xml

    std::string tempDir = "/tmp/sample_docx_create";
    system(("mkdir -p " + tempDir + "/word").c_str());
    system(("mkdir -p " + tempDir + "/_rels").c_str());
    system(("mkdir -p " + tempDir + "/word/_rels").c_str());

    // Create minimal document.xml
    std::ofstream docXml(tempDir + "/word/document.xml");
    docXml << R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:body>
    <w:p>
      <w:r>
        <w:t>Hello from DocxReader!</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>This is a test document.</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Testing DOCX parsing functionality.</w:t>
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

    std::cout << "✓ Sample DOCX file created: " << filename << "\n";
}

int main(int argc, char *argv[])
{
    printSeparator("DocxReader Test Program");

    std::string testFile;

    // Check if a file was provided as argument
    if (argc > 1)
    {
        testFile = argv[1];
        std::cout << "📄 Using provided file: " << testFile << "\n";
    }
    else
    {
        // Create a sample DOCX for testing
        testFile = "/tmp/sample_test.docx";
        std::cout << "📝 No file provided. Creating sample DOCX...\n";
        createSampleDocx("sample_test.docx");
        std::cout << "📄 Using sample file: " << testFile << "\n";
    }

    // Create DocxReader instance
    json2doc::DocxReader reader;

    // Test 1: Open file
    printSeparator("Test 1: Opening DOCX File");
    if (reader.open(testFile))
    {
        std::cout << "✓ File opened successfully\n";
    }
    else
    {
        std::cerr << "✗ Failed to open file: " << reader.getLastError() << "\n";
        return 1;
    }

    // Test 2: Decompress file
    printSeparator("Test 2: Decompressing DOCX");
    if (reader.decompress())
    {
        std::cout << "✓ File decompressed successfully\n";
        std::cout << "📁 Temp directory: " << reader.getTempPath() << "\n";
    }
    else
    {
        std::cerr << "✗ Failed to decompress: " << reader.getLastError() << "\n";
        return 1;
    }

    // Test 3: Read XML content
    printSeparator("Test 3: Reading Document XML");
    std::string xmlContent = reader.readDocumentXml();
    if (!xmlContent.empty())
    {
        std::cout << "✓ XML content read successfully\n";
        std::cout << "📊 XML size: " << xmlContent.size() << " bytes\n";

        // Create temp directory in project for extracted XMLs
        std::string projectTempDir = "temp_extracted_xml";
        system(("mkdir -p " + projectTempDir).c_str());

        // Generate filename from original DOCX
        std::string baseFilename = testFile;
        size_t lastSlash = baseFilename.find_last_of("/\\");
        if (lastSlash != std::string::npos)
        {
            baseFilename = baseFilename.substr(lastSlash + 1);
        }
        // Remove .docx extension
        size_t dotPos = baseFilename.find_last_of(".");
        if (dotPos != std::string::npos)
        {
            baseFilename = baseFilename.substr(0, dotPos);
        }

        // Save XML to file
        std::string xmlOutputPath = projectTempDir + "/" + baseFilename + "_document.xml";
        std::ofstream xmlFile(xmlOutputPath);
        if (xmlFile.is_open())
        {
            xmlFile << xmlContent;
            xmlFile.close();
            std::cout << "💾 XML saved to: " << xmlOutputPath << "\n";
        }
        else
        {
            std::cerr << "⚠ Warning: Could not save XML file\n";
        }
    }
    else
    {
        std::cerr << "✗ Failed to read XML: " << reader.getLastError() << "\n";
        return 1;
    }

    // Test 4: Print XML
    printSeparator("Test 4: Printing XML Content");
    reader.printXml();

    // Test 5: Parse XML content
    printSeparator("Test 5: Parsing and Extracting Text");
    std::string parsedText = reader.parseXmlContent();
    if (!parsedText.empty())
    {
        std::cout << "✓ Text extracted successfully\n";
        std::cout << "\n📝 Extracted text:\n";
        std::cout << "─────────────────────────────────────────\n";
        std::cout << parsedText << "\n";
        std::cout << "─────────────────────────────────────────\n";
    }
    else
    {
        std::cout << "⚠ No text content found in document\n";
    }

    // Test 6: Cleanup
    printSeparator("Test 6: Cleanup");
    reader.cleanup();
    std::cout << "✓ Temporary files cleaned up\n";

    printSeparator("All Tests Completed Successfully!");
    std::cout << "✨ DocxReader is working correctly!\n\n";

    return 0;
}
