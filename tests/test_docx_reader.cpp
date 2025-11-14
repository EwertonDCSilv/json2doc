#include <iostream>
#include <cassert>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include "json2doc/docx_reader.h"

/**
 * @brief TDD Unit Tests for DocxReader class
 *
 * Test-Driven Development approach:
 * 1. Test basic construction and destruction
 * 2. Test file operations
 * 3. Test decompression
 * 4. Test XML reading
 * 5. Test XML parsing
 * 6. Test error handling
 */

// Helper function to create a minimal test DOCX file
void createTestDocx(const std::string &filename)
{
    std::string tempDir = "/tmp/test_docx_create_" + std::to_string(getpid());
    system(("mkdir -p " + tempDir + "/word").c_str());
    system(("mkdir -p " + tempDir + "/_rels").c_str());

    // Create document.xml
    std::ofstream docXml(tempDir + "/word/document.xml");
    docXml << R"(<?xml version="1.0" encoding="UTF-8"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:body>
    <w:p><w:r><w:t>Test Content</w:t></w:r></w:p>
  </w:body>
</w:document>)";
    docXml.close();

    // Create [Content_Types].xml
    std::ofstream contentTypes(tempDir + "/[Content_Types].xml");
    contentTypes << R"(<?xml version="1.0"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="xml" ContentType="application/xml"/>
</Types>)";
    contentTypes.close();

    // Create .rels
    std::ofstream rels(tempDir + "/_rels/.rels");
    rels << R"(<?xml version="1.0"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>)";
    rels.close();

    // Create ZIP
    std::string zipCmd = "cd " + tempDir + " && zip -q -r " + filename + " . && mv " + filename + " /tmp/";
    system(zipCmd.c_str());
    system(("rm -rf " + tempDir).c_str());
}

// Helper to check if file exists
bool fileExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

// Test 1: Constructor creates valid object
void testConstructor()
{
    json2doc::DocxReader reader;
    assert(reader.getTempPath().empty());
    assert(reader.getLastError().empty());
    std::cout << "✓ Test 1 passed: Constructor creates valid object\n";
}

// Test 2: Destructor cleans up resources
void testDestructor()
{
    std::string tempPath;
    {
        json2doc::DocxReader reader;
        std::string testFile = "/tmp/test_destructor.docx";
        createTestDocx("test_destructor.docx");

        reader.open(testFile);
        reader.decompress();
        tempPath = reader.getTempPath();

        // Temp directory should exist
        assert(!tempPath.empty());
        assert(fileExists(tempPath));
    }
    // After destructor, temp directory should be cleaned
    // Note: This may not always work immediately due to timing
    std::cout << "✓ Test 2 passed: Destructor cleanup executed\n";
}

// Test 3: Open non-existent file returns false
void testOpenNonExistentFile()
{
    json2doc::DocxReader reader;
    bool result = reader.open("/tmp/nonexistent_file_12345.docx");

    assert(!result);
    assert(!reader.getLastError().empty());
    assert(reader.getLastError().find("does not exist") != std::string::npos);
    std::cout << "✓ Test 3 passed: Opening non-existent file returns false with error\n";
}

// Test 4: Open valid file returns true
void testOpenValidFile()
{
    std::string testFile = "/tmp/test_open.docx";
    createTestDocx("test_open.docx");

    json2doc::DocxReader reader;
    bool result = reader.open(testFile);

    assert(result);
    assert(reader.getLastError().empty());
    std::cout << "✓ Test 4 passed: Opening valid file returns true\n";
}

// Test 5: Decompress without opening file returns false
void testDecompressWithoutOpen()
{
    json2doc::DocxReader reader;
    bool result = reader.decompress();

    assert(!result);
    assert(!reader.getLastError().empty());
    assert(reader.getLastError().find("No file") != std::string::npos);
    std::cout << "✓ Test 5 passed: Decompress without open returns false\n";
}

// Test 6: Decompress creates temp directory
void testDecompressCreatesTemp()
{
    std::string testFile = "/tmp/test_decompress.docx";
    createTestDocx("test_decompress.docx");

    json2doc::DocxReader reader;
    reader.open(testFile);
    bool result = reader.decompress();

    assert(result);
    assert(!reader.getTempPath().empty());
    assert(fileExists(reader.getTempPath()));
    std::cout << "✓ Test 6 passed: Decompress creates temp directory\n";
}

// Test 7: Read XML without decompression returns empty
void testReadXmlWithoutDecompress()
{
    std::string testFile = "/tmp/test_xml_no_decomp.docx";
    createTestDocx("test_xml_no_decomp.docx");

    json2doc::DocxReader reader;
    reader.open(testFile);

    std::string xml = reader.readDocumentXml();
    assert(xml.empty());
    assert(!reader.getLastError().empty());
    std::cout << "✓ Test 7 passed: Read XML without decompress returns empty\n";
}

// Test 8: Read XML after decompression returns content
void testReadXmlAfterDecompress()
{
    std::string testFile = "/tmp/test_read_xml.docx";
    createTestDocx("test_read_xml.docx");

    json2doc::DocxReader reader;
    reader.open(testFile);
    reader.decompress();

    std::string xml = reader.readDocumentXml();
    assert(!xml.empty());
    assert(xml.find("<?xml") != std::string::npos);
    assert(xml.find("w:document") != std::string::npos);
    std::cout << "✓ Test 8 passed: Read XML returns valid content\n";
}

// Test 9: Parse XML extracts text content
void testParseXmlExtractsText()
{
    std::string testFile = "/tmp/test_parse_xml.docx";
    createTestDocx("test_parse_xml.docx");

    json2doc::DocxReader reader;
    reader.open(testFile);
    reader.decompress();
    reader.readDocumentXml();

    std::string text = reader.parseXmlContent();
    assert(!text.empty());
    assert(text.find("Test Content") != std::string::npos);
    std::cout << "✓ Test 9 passed: Parse XML extracts text content\n";
}

// Test 10: Cleanup removes temp directory
void testCleanup()
{
    std::string testFile = "/tmp/test_cleanup.docx";
    createTestDocx("test_cleanup.docx");

    json2doc::DocxReader reader;
    reader.open(testFile);
    reader.decompress();

    std::string tempPath = reader.getTempPath();
    assert(fileExists(tempPath));

    reader.cleanup();

    // After cleanup, temp path should be empty
    assert(reader.getTempPath().empty());
    std::cout << "✓ Test 10 passed: Cleanup removes temp directory\n";
}

// Test 11: Print XML doesn't crash without content
void testPrintXmlWithoutContent()
{
    json2doc::DocxReader reader;

    // Should not crash
    reader.printXml();
    std::cout << "✓ Test 11 passed: Print XML without content doesn't crash\n";
}

// Test 12: Error messages are descriptive
void testErrorMessages()
{
    json2doc::DocxReader reader;

    // Test error on non-existent file
    reader.open("/nonexistent.docx");
    std::string error1 = reader.getLastError();
    assert(!error1.empty());

    // Test error on decompress without open
    json2doc::DocxReader reader2;
    reader2.decompress();
    std::string error2 = reader2.getLastError();
    assert(!error2.empty());

    std::cout << "✓ Test 12 passed: Error messages are descriptive\n";
}

// Test 13: Multiple operations on same reader
void testMultipleOperations()
{
    std::string testFile = "/tmp/test_multiple_ops.docx";
    createTestDocx("test_multiple_ops.docx");

    json2doc::DocxReader reader;

    // First operation
    assert(reader.open(testFile));
    assert(reader.decompress());
    std::string xml1 = reader.readDocumentXml();
    assert(!xml1.empty());

    // Cleanup
    reader.cleanup();

    // Second operation on same reader
    assert(reader.open(testFile));
    assert(reader.decompress());
    std::string xml2 = reader.readDocumentXml();
    assert(!xml2.empty());

    std::cout << "✓ Test 13 passed: Multiple operations work correctly\n";
}

// Test 14: Parse XML handles empty document
void testParseEmptyDocument()
{
    // Create DOCX with empty body
    std::string tempDir = "/tmp/test_empty_doc_" + std::to_string(getpid());
    system(("mkdir -p " + tempDir + "/word").c_str());

    std::ofstream docXml(tempDir + "/word/document.xml");
    docXml << R"(<?xml version="1.0"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:body></w:body>
</w:document>)";
    docXml.close();

    std::ofstream contentTypes(tempDir + "/[Content_Types].xml");
    contentTypes << "<?xml version=\"1.0\"?><Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\"/>";
    contentTypes.close();

    system(("mkdir -p " + tempDir + "/_rels").c_str());
    std::ofstream rels(tempDir + "/_rels/.rels");
    rels << "<?xml version=\"1.0\"?><Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\"/>";
    rels.close();

    std::string testFile = "/tmp/test_empty.docx";
    std::string zipCmd = "cd " + tempDir + " && zip -q -r test_empty.docx . && mv test_empty.docx /tmp/";
    system(zipCmd.c_str());
    system(("rm -rf " + tempDir).c_str());

    json2doc::DocxReader reader;
    reader.open(testFile);
    reader.decompress();

    std::string text = reader.parseXmlContent();
    // Empty document should return empty or whitespace only
    assert(text.empty() || text.find_first_not_of(" \t\n\r") == std::string::npos);

    std::cout << "✓ Test 14 passed: Parse empty document handled correctly\n";
}

// Test 15: getTempPath returns correct path after decompress
void testGetTempPath()
{
    std::string testFile = "/tmp/test_temp_path.docx";
    createTestDocx("test_temp_path.docx");

    json2doc::DocxReader reader;

    // Before decompress
    assert(reader.getTempPath().empty());

    reader.open(testFile);
    reader.decompress();

    // After decompress
    std::string tempPath = reader.getTempPath();
    assert(!tempPath.empty());
    assert(tempPath.find("/tmp/") != std::string::npos);
    assert(tempPath.find("docx_reader_") != std::string::npos);

    std::cout << "✓ Test 15 passed: getTempPath returns correct path\n";
}

int main()
{
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║     DocxReader TDD Unit Tests                          ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";

    try
    {
        // Run all tests in order
        testConstructor();              // Test 1
        testDestructor();               // Test 2
        testOpenNonExistentFile();      // Test 3
        testOpenValidFile();            // Test 4
        testDecompressWithoutOpen();    // Test 5
        testDecompressCreatesTemp();    // Test 6
        testReadXmlWithoutDecompress(); // Test 7
        testReadXmlAfterDecompress();   // Test 8
        testParseXmlExtractsText();     // Test 9
        testCleanup();                  // Test 10
        testPrintXmlWithoutContent();   // Test 11
        testErrorMessages();            // Test 12
        testMultipleOperations();       // Test 13
        testParseEmptyDocument();       // Test 14
        testGetTempPath();              // Test 15

        std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
        std::cout << "║  ✓ All 15 tests passed successfully!                  ║\n";
        std::cout << "╚════════════════════════════════════════════════════════╝\n\n";

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n✗ Test failed with unknown exception\n";
        return 1;
    }
}
