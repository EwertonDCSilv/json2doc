#include <iostream>
#include <cassert>
#include "json2doc/json2doc.h"
#include "json2doc/converter.h"

void testVersion() {
    json2doc::Json2Doc converter;
    std::string version = converter.getVersion();
    assert(!version.empty());
    std::cout << "✓ Version test passed: " << version << "\n";
}

void testLoadJson() {
    json2doc::Json2Doc converter;
    
    // Test with valid JSON
    std::string validJson = R"({"test": "data"})";
    assert(converter.loadJson(validJson));
    std::cout << "✓ Load valid JSON test passed\n";
    
    // Test with empty JSON
    assert(!converter.loadJson(""));
    std::cout << "✓ Load empty JSON test passed\n";
}

void testJsonValidation() {
    // Test valid JSON
    std::string validJson1 = R"({"key": "value"})";
    assert(json2doc::Converter::isValidJson(validJson1));
    std::cout << "✓ Valid JSON object test passed\n";
    
    std::string validJson2 = R"([1, 2, 3])";
    assert(json2doc::Converter::isValidJson(validJson2));
    std::cout << "✓ Valid JSON array test passed\n";
    
    // Test invalid JSON
    assert(!json2doc::Converter::isValidJson(""));
    std::cout << "✓ Empty JSON validation test passed\n";
}

void testConversion() {
    json2doc::Json2Doc converter;
    std::string jsonData = R"({"title": "Test"})";
    
    converter.loadJson(jsonData);
    std::string result = converter.convertToDocument("template.txt");
    assert(!result.empty());
    std::cout << "✓ Conversion test passed\n";
}

int main() {
    std::cout << "Running json2doc tests...\n\n";
    
    try {
        testVersion();
        testLoadJson();
        testJsonValidation();
        testConversion();
        
        std::cout << "\n✓ All tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "✗ Test failed with unknown exception\n";
        return 1;
    }
}
