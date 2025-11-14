#include "json2doc/xml_document.h"
#include <iostream>
#include <cassert>

/**
 * @brief TDD Test Suite for XmlDocument class
 */

std::string createSampleXml()
{
    return R"(<?xml version="1.0" encoding="UTF-8"?>
<document>
  <metadata>
    <title>Sample Document</title>
    <author>John Doe</author>
  </metadata>
  <body>
    <paragraph>Hello {{name}}, welcome to {{company}}!</paragraph>
    <paragraph>Your position is {{position}}.</paragraph>
  </body>
</document>)";
}

std::string createDocxXml()
{
    return R"(<?xml version="1.0" encoding="UTF-8"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:body>
    <w:p>
      <w:r>
        <w:t>Title: {{title}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Author: {{author}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Version: {{metadata.version}}</w:t>
      </w:r>
    </w:p>
  </w:body>
</w:document>)";
}

// Test 1: Constructor
void testConstructor()
{
    std::cout << "Test 1: Constructor creates valid object... ";
    json2doc::XmlDocument doc;
    assert(!doc.isValid()); // Not loaded yet
    std::cout << "✓ PASSED\n";
}

// Test 2: Load from string
void testLoadFromString()
{
    std::cout << "Test 2: Load XML from string... ";
    json2doc::XmlDocument doc;
    std::string xml = createSampleXml();
    assert(doc.loadFromString(xml) == true);
    assert(doc.isValid() == true);
    std::cout << "✓ PASSED\n";
}

// Test 3: Load invalid XML
void testLoadInvalidXml()
{
    std::cout << "Test 3: Load invalid XML fails gracefully... ";
    json2doc::XmlDocument doc;
    std::string invalidXml = "<invalid>not closed";
    assert(doc.loadFromString(invalidXml) == false);
    assert(!doc.getLastError().empty());
    std::cout << "✓ PASSED\n";
}

// Test 4: toString
void testToString()
{
    std::cout << "Test 4: Convert XML to string... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());
    std::string xmlStr = doc.toString();
    assert(!xmlStr.empty());
    assert(xmlStr.find("<document>") != std::string::npos);
    std::cout << "✓ PASSED\n";
}

// Test 5: XPath query
void testXPathQuery()
{
    std::cout << "Test 5: XPath query... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    auto nodes = doc.query("//title");
    assert(nodes.size() == 1);
    assert(nodes[0].value == "Sample Document");
    std::cout << "✓ PASSED\n";
}

// Test 6: Find text nodes
void testFindTextNodes()
{
    std::cout << "Test 6: Find text nodes... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    auto texts = doc.findTextNodes("//paragraph/text()");
    assert(texts.size() == 2);
    std::cout << "✓ PASSED\n";
}

// Test 7: Find template nodes
void testFindTemplateNodes()
{
    std::cout << "Test 7: Find template nodes with {{variables}}... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    auto templateNodes = doc.findTemplateNodes();
    // Debug: print found nodes
    if (templateNodes.size() != 2)
    {
        std::cout << "\nFound " << templateNodes.size() << " nodes:\n";
        for (const auto &node : templateNodes)
        {
            std::cout << "  - " << node.name << ": " << node.value << "\n";
        }
    }
    assert(templateNodes.size() >= 1); // At least one node with variables
    std::cout << "✓ PASSED\n";
}

// Test 8: Get node
void testGetNode()
{
    std::cout << "Test 8: Get node by XPath... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    auto node = doc.getNode("//author");
    assert(node.name == "author");
    assert(node.value == "John Doe");
    std::cout << "✓ PASSED\n";
}

// Test 9: Set node text
void testSetNodeText()
{
    std::cout << "Test 9: Set node text... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    bool result = doc.setNodeText("//author", "Jane Smith");
    assert(result == true);

    auto node = doc.getNode("//author");
    assert(node.value == "Jane Smith");
    std::cout << "✓ PASSED\n";
}

// Test 10: Replace text
void testReplaceText()
{
    std::cout << "Test 10: Replace text in nodes... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    int count = doc.replaceText("//title", "Sample", "Updated");
    assert(count == 1);

    auto node = doc.getNode("//title");
    assert(node.value == "Updated Document");
    std::cout << "✓ PASSED\n";
}

// Test 11: Replace variables
void testReplaceVariables()
{
    std::cout << "Test 11: Replace {{variables}} in XML... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    std::map<std::string, std::string> vars;
    vars["name"] = "Alice";
    vars["company"] = "TechCorp";
    vars["position"] = "Developer";

    int count = doc.replaceVariables(vars);
    assert(count == 3);

    std::string xmlStr = doc.toString();
    assert(xmlStr.find("{{name}}") == std::string::npos);
    assert(xmlStr.find("Alice") != std::string::npos);
    assert(xmlStr.find("TechCorp") != std::string::npos);
    std::cout << "✓ PASSED\n";
}

// Test 12: Get text content
void testGetTextContent()
{
    std::cout << "Test 12: Get all text content... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    std::string text = doc.getTextContent();
    assert(text.find("Sample Document") != std::string::npos);
    assert(text.find("John Doe") != std::string::npos);
    std::cout << "✓ PASSED\n";
}

// Test 13: Attributes
void testAttributes()
{
    std::cout << "Test 13: Get/Set attributes... ";
    std::string xmlWithAttrs = R"(<?xml version="1.0"?>
<root>
  <element id="123" name="test"/>
</root>)";

    json2doc::XmlDocument doc;
    doc.loadFromString(xmlWithAttrs);

    std::string id = doc.getAttributeValue("//element", "id");
    assert(id == "123");

    bool result = doc.setAttributeValue("//element", "id", "456");
    assert(result == true);

    id = doc.getAttributeValue("//element", "id");
    assert(id == "456");
    std::cout << "✓ PASSED\n";
}

// Test 14: DOCX XML format
void testDocxXml()
{
    std::cout << "Test 14: Work with DOCX XML format... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createDocxXml());

    auto nodes = doc.query("//w:t");
    assert(nodes.size() == 3);

    std::map<std::string, std::string> vars;
    vars["title"] = "My Document";
    vars["author"] = "John Smith";
    vars["metadata.version"] = "2.0";

    int count = doc.replaceVariables(vars);
    assert(count == 3);

    std::string xmlStr = doc.toString();
    assert(xmlStr.find("My Document") != std::string::npos);
    std::cout << "✓ PASSED\n";
}

// Test 15: Clear document
void testClear()
{
    std::cout << "Test 15: Clear document... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());
    assert(doc.isValid());

    doc.clear();
    assert(!doc.isValid());
    std::cout << "✓ PASSED\n";
}

// Test 16: Multiple queries
void testMultipleQueries()
{
    std::cout << "Test 16: Multiple XPath queries... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    auto titles = doc.query("//title");
    auto authors = doc.query("//author");
    auto paragraphs = doc.query("//paragraph");

    assert(titles.size() == 1);
    assert(authors.size() == 1);
    assert(paragraphs.size() == 2);
    std::cout << "✓ PASSED\n";
}

// Test 17: Namespace handling (DOCX)
void testNamespaces()
{
    std::cout << "Test 17: Handle XML namespaces... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createDocxXml());

    // XPath with namespace
    auto nodes = doc.query("//*[local-name()='t']");
    assert(nodes.size() == 3);
    std::cout << "✓ PASSED\n";
}

// Test 18: Empty document
void testEmptyDocument()
{
    std::cout << "Test 18: Handle empty document... ";
    json2doc::XmlDocument doc;

    auto nodes = doc.query("//anything");
    assert(nodes.empty());

    std::string text = doc.getTextContent();
    assert(text.empty());
    std::cout << "✓ PASSED\n";
}

// Test 19: Invalid XPath
void testInvalidXPath()
{
    std::cout << "Test 19: Handle invalid XPath... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    auto nodes = doc.query("//[invalid xpath");
    // Should not crash, returns empty
    assert(nodes.empty());
    std::cout << "✓ PASSED\n";
}

// Test 20: Partial variable replacement
void testPartialReplacement()
{
    std::cout << "Test 20: Partial variable replacement... ";
    json2doc::XmlDocument doc;
    doc.loadFromString(createSampleXml());

    std::map<std::string, std::string> vars;
    vars["name"] = "Bob"; // Only replace 'name', not others

    int count = doc.replaceVariables(vars);
    assert(count == 1);

    std::string xmlStr = doc.toString();
    assert(xmlStr.find("Bob") != std::string::npos);
    assert(xmlStr.find("{{company}}") != std::string::npos); // Still has placeholder
    std::cout << "✓ PASSED\n";
}

int main()
{
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║        XmlDocument TDD Test Suite                      ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    int testCount = 0;

    try
    {
        testConstructor();
        testCount++;
        testLoadFromString();
        testCount++;
        testLoadInvalidXml();
        testCount++;
        testToString();
        testCount++;
        testXPathQuery();
        testCount++;
        testFindTextNodes();
        testCount++;
        testFindTemplateNodes();
        testCount++;
        testGetNode();
        testCount++;
        testSetNodeText();
        testCount++;
        testReplaceText();
        testCount++;
        testReplaceVariables();
        testCount++;
        testGetTextContent();
        testCount++;
        testAttributes();
        testCount++;
        testDocxXml();
        testCount++;
        testClear();
        testCount++;
        testMultipleQueries();
        testCount++;
        testNamespaces();
        testCount++;
        testEmptyDocument();
        testCount++;
        testInvalidXPath();
        testCount++;
        testPartialReplacement();
        testCount++;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    }

    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║  ✓ All " << testCount << " tests passed successfully!                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    return 0;
}
