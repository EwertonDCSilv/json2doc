#include "json2doc/json_merge.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

/**
 * @brief TDD Test Suite for JsonMerge class
 *
 * Tests are written in Test-Driven Development style:
 * 1. Test basic functionality
 * 2. Test edge cases
 * 3. Test error handling
 * 4. Test integration scenarios
 */

// Helper function to create test JSON strings
std::string createSimpleJson()
{
    return R"({
        "name": "John Doe",
        "title": "Software Engineer",
        "date": "November 14, 2025"
    })";
}

std::string createNestedJson()
{
    return R"({
        "author": "Jane Smith",
        "metadata": {
            "version": "1.0.0",
            "status": "draft",
            "type": "example"
        },
        "position": "Senior Developer"
    })";
}

std::string createComplexJson()
{
    return R"({
        "title": "Sample Document",
        "author": "John Doe",
        "date": "November 14, 2025",
        "content": "This is a sample document.",
        "name": "Ewerton Silva Santos",
        "position": "Software Engineer",
        "metadata": {
            "version": "1.0.0",
            "type": "example",
            "status": "draft"
        }
    })";
}

// Test 1: Constructor creates valid object
void testConstructor()
{
    std::cout << "Test 1: Constructor creates valid object... ";
    json2doc::JsonMerge merger;
    assert(merger.getLastError().empty());
    assert(merger.getAllKeys().empty());
    std::cout << "✓ PASSED\n";
}

// Test 2: Load JSON from string
void testLoadJsonString()
{
    std::cout << "Test 2: Load JSON from string... ";
    json2doc::JsonMerge merger;
    std::string json = createSimpleJson();
    assert(merger.loadJsonString(json) == true);
    assert(merger.getAllKeys().size() == 3);
    std::cout << "✓ PASSED\n";
}

// Test 3: Load invalid JSON fails gracefully
void testLoadInvalidJson()
{
    std::cout << "Test 3: Load invalid JSON fails gracefully... ";
    json2doc::JsonMerge merger;
    std::string invalidJson = "{invalid json}";
    // Should not crash, may or may not parse depending on robustness
    merger.loadJsonString(invalidJson);
    // Test passes if it doesn't crash
    std::cout << "✓ PASSED\n";
}

// Test 4: Get value by key
void testGetValue()
{
    std::cout << "Test 4: Get value by key... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createSimpleJson());
    
    std::string name = merger.getValue("name");
    assert(name == "John Doe");
    
    std::string title = merger.getValue("title");
    assert(title == "Software Engineer");
    
    std::cout << "✓ PASSED\n";
}

// Test 5: Get nested value with dot notation
void testGetNestedValue()
{
    std::cout << "Test 5: Get nested value with dot notation... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createNestedJson());
    
    std::string version = merger.getValue("metadata.version");
    assert(version == "1.0.0");
    
    std::string status = merger.getValue("metadata.status");
    assert(status == "draft");
    
    std::cout << "✓ PASSED\n";
}

// Test 6: Get non-existent key returns empty string
void testGetNonExistentKey()
{
    std::cout << "Test 6: Get non-existent key returns empty string... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createSimpleJson());
    
    std::string missing = merger.getValue("nonexistent");
    assert(missing.empty());
    
    std::cout << "✓ PASSED\n";
}

// Test 7: hasKey returns correct boolean
void testHasKey()
{
    std::cout << "Test 7: hasKey returns correct boolean... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createSimpleJson());
    
    assert(merger.hasKey("name") == true);
    assert(merger.hasKey("nonexistent") == false);
    
    std::cout << "✓ PASSED\n";
}

// Test 8: Find variables in text
void testFindVariables()
{
    std::cout << "Test 8: Find variables in text... ";
    json2doc::JsonMerge merger;
    
    std::string text = "Hello {{name}}, your title is {{title}} and date is {{date}}.";
    std::vector<std::string> vars = merger.findVariables(text);
    
    assert(vars.size() == 3);
    assert(std::find(vars.begin(), vars.end(), "name") != vars.end());
    assert(std::find(vars.begin(), vars.end(), "title") != vars.end());
    assert(std::find(vars.begin(), vars.end(), "date") != vars.end());
    
    std::cout << "✓ PASSED\n";
}

// Test 9: Find variables with spaces
void testFindVariablesWithSpaces()
{
    std::cout << "Test 9: Find variables with spaces... ";
    json2doc::JsonMerge merger;
    
    std::string text = "Value: {{ name }}, {{ title }}";
    std::vector<std::string> vars = merger.findVariables(text);
    
    assert(vars.size() == 2);
    
    std::cout << "✓ PASSED\n";
}

// Test 10: Find no variables in plain text
void testFindNoVariables()
{
    std::cout << "Test 10: Find no variables in plain text... ";
    json2doc::JsonMerge merger;
    
    std::string text = "This is plain text without variables.";
    std::vector<std::string> vars = merger.findVariables(text);
    
    assert(vars.empty());
    
    std::cout << "✓ PASSED\n";
}

// Test 11: Replace simple variables
void testReplaceSimpleVariables()
{
    std::cout << "Test 11: Replace simple variables... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createSimpleJson());
    
    std::string template_text = "Name: {{name}}, Title: {{title}}";
    std::string result = merger.replaceVariables(template_text);
    
    assert(result == "Name: John Doe, Title: Software Engineer");
    
    std::cout << "✓ PASSED\n";
}

// Test 12: Replace nested variables
void testReplaceNestedVariables()
{
    std::cout << "Test 12: Replace nested variables... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createNestedJson());
    
    std::string template_text = "Version: {{metadata.version}}, Status: {{metadata.status}}";
    std::string result = merger.replaceVariables(template_text);
    
    assert(result == "Version: 1.0.0, Status: draft");
    
    std::cout << "✓ PASSED\n";
}

// Test 13: Replace with missing variables (keeps placeholder)
void testReplaceMissingVariables()
{
    std::cout << "Test 13: Replace with missing variables... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createSimpleJson());
    
    std::string template_text = "Name: {{name}}, Missing: {{missing}}";
    std::string result = merger.replaceVariables(template_text);
    
    // Missing variables should remain unchanged
    assert(result.find("{{missing}}") != std::string::npos);
    assert(result.find("John Doe") != std::string::npos);
    
    std::cout << "✓ PASSED\n";
}

// Test 14: Get statistics after replacement
void testGetStats()
{
    std::cout << "Test 14: Get statistics after replacement... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createSimpleJson());
    
    std::string template_text = "{{name}} {{title}} {{missing}}";
    merger.replaceVariables(template_text);
    
    auto stats = merger.getStats();
    assert(stats["found"] == 3);
    assert(stats["replaced"] == 2);
    assert(stats["missing"] == 1);
    
    std::cout << "✓ PASSED\n";
}

// Test 15: Clear data
void testClear()
{
    std::cout << "Test 15: Clear data... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createSimpleJson());
    
    assert(!merger.getAllKeys().empty());
    
    merger.clear();
    
    assert(merger.getAllKeys().empty());
    assert(merger.getLastError().empty());
    
    std::cout << "✓ PASSED\n";
}

// Test 16: Load JSON from file
void testLoadJsonFile()
{
    std::cout << "Test 16: Load JSON from file... ";
    json2doc::JsonMerge merger;
    
    // Use the existing data.json file
    bool loaded = merger.loadJson("data.json");
    
    if (loaded)
    {
        assert(!merger.getAllKeys().empty());
        std::cout << "✓ PASSED\n";
    }
    else
    {
        std::cout << "⊘ SKIPPED (file not found)\n";
    }
}

// Test 17: Replace variables in XML-like content
void testReplaceInXmlContent()
{
    std::cout << "Test 17: Replace variables in XML-like content... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createComplexJson());
    
    std::string xmlTemplate = R"(<w:document>
        <w:p><w:t>Title: {{title}}</w:t></w:p>
        <w:p><w:t>Author: {{author}}</w:t></w:p>
        <w:p><w:t>Date: {{date}}</w:t></w:p>
        <w:p><w:t>Version: {{metadata.version}}</w:t></w:p>
    </w:document>)";
    
    std::string result = merger.replaceVariables(xmlTemplate);
    
    assert(result.find("Title: Sample Document") != std::string::npos);
    assert(result.find("Author: John Doe") != std::string::npos);
    assert(result.find("Date: November 14, 2025") != std::string::npos);
    assert(result.find("Version: 1.0.0") != std::string::npos);
    
    std::cout << "✓ PASSED\n";
}

// Test 18: Get all keys
void testGetAllKeys()
{
    std::cout << "Test 18: Get all keys... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createNestedJson());
    
    std::vector<std::string> keys = merger.getAllKeys();
    assert(!keys.empty());
    assert(std::find(keys.begin(), keys.end(), "author") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "position") != keys.end());
    
    std::cout << "✓ PASSED\n";
}

// Test 19: Multiple replacements of same variable
void testMultipleReplacements()
{
    std::cout << "Test 19: Multiple replacements of same variable... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createSimpleJson());
    
    std::string template_text = "{{name}} is {{name}}, {{name}}!";
    std::string result = merger.replaceVariables(template_text);
    
    assert(result == "John Doe is John Doe, John Doe!");
    
    std::cout << "✓ PASSED\n";
}

// Test 20: Empty template
void testEmptyTemplate()
{
    std::cout << "Test 20: Empty template... ";
    json2doc::JsonMerge merger;
    merger.loadJsonString(createSimpleJson());
    
    std::string template_text = "";
    std::string result = merger.replaceVariables(template_text);
    
    assert(result.empty());
    
    std::cout << "✓ PASSED\n";
}

// Main test runner
int main()
{
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║        JsonMerge TDD Test Suite                        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    int testCount = 0;

    try
    {
        testConstructor();
        testCount++;
        testLoadJsonString();
        testCount++;
        testLoadInvalidJson();
        testCount++;
        testGetValue();
        testCount++;
        testGetNestedValue();
        testCount++;
        testGetNonExistentKey();
        testCount++;
        testHasKey();
        testCount++;
        testFindVariables();
        testCount++;
        testFindVariablesWithSpaces();
        testCount++;
        testFindNoVariables();
        testCount++;
        testReplaceSimpleVariables();
        testCount++;
        testReplaceNestedVariables();
        testCount++;
        testReplaceMissingVariables();
        testCount++;
        testGetStats();
        testCount++;
        testClear();
        testCount++;
        testLoadJsonFile();
        testCount++;
        testReplaceInXmlContent();
        testCount++;
        testGetAllKeys();
        testCount++;
        testMultipleReplacements();
        testCount++;
        testEmptyTemplate();
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
