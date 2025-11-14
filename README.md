# json2doc

[![CI - Build and Test](https://github.com/EwertonDCSilv/json2doc/actions/workflows/ci.yml/badge.svg)](https://github.com/EwertonDCSilv/json2doc/actions/workflows/ci.yml)

Lib to transpose json data to document templates

## Description

json2doc is a C++ library designed to convert JSON data into various document formats using templates.

### Key Features

- **DocxReader**: Read and parse DOCX files, extract XML content
- **JsonMerge**: Merge JSON data into document templates with `{{variable}}` placeholders
- **XmlDocument**: Robust XML parsing with XPath support using pugixml library
- **Template-based**: Use DOCX templates with variables that are automatically replaced with JSON values
- **Nested JSON Support**: Access nested JSON values with dot notation (e.g., `{{metadata.version}}`)
- **XPath Queries**: Precise node selection and manipulation in XML documents

### Components

1. **DocxReader** - Opens DOCX files, decompresses ZIP, reads and parses XML content
   - See [DOCX_READER.md](DOCX_READER.md) for details
   
2. **JsonMerge** - Finds and replaces `{{variables}}` in templates with JSON data
   - See [JSON_MERGE_README.md](JSON_MERGE_README.md) for details

3. **XmlDocument** - XML parsing and manipulation with XPath support (pugixml)
   - See [XML_DOCUMENT_README.md](XML_DOCUMENT_README.md) for details

4. **Integration** - Combine all three to create dynamic documents from templates + data

## Building the Library

### Prerequisites

- GNU Make
- C++17 compatible compiler (GCC 7+, Clang 5+)
- libpugixml-dev (XML parsing library)

### Install Dependencies

```bash
# Ubuntu/Debian
sudo apt-get install g++ make unzip zip libpugixml-dev

# Arch Linux
sudo pacman -S gcc make unzip zip pugixml

# macOS
brew install gcc make pugixml
```

### Build Instructions

```bash
# Build the main program
make main

# Build and run tests
make test

# Build everything (main program and tests)
make all

# Run the main program
make run

# Clean build artifacts
make clean
```

### Build Targets

- `main`: Build the main program
- `test`: Build and run the json2doc test suite
- `test-docx`: Build and run DocxReader tests (15 tests)
- `test-docx-main`: Build DocxReader standalone test program
- `run-docx-test`: Run DocxReader standalone test
- `test-json-merge`: Build and run JsonMerge tests (20 TDD tests)
- `test-json-merge-main`: Build JsonMerge + DocxReader integration test
- `run-json-merge-test`: Run JsonMerge integration test
- `test-xml`: Build and run XmlDocument tests (20 TDD tests)
- `test-xml-integration`: Build XmlDocument + JsonMerge integration demo
- `run-xml-integration`: Run XmlDocument integration demo
- `all`: Build main program and all tests
- `run`: Run the main program
- `clean`: Remove all build artifacts

## Usage

### Basic Example

```cpp
#include "json2doc/docx_reader.h"
#include "json2doc/xml_document.h"
#include "json2doc/json_merge.h"

int main() {
    // 1. Read DOCX template with {{variables}}
    json2doc::DocxReader reader;
    reader.open("template.docx");
    reader.decompress();
    std::string xmlContent = reader.readDocumentXml();
    
    // 2. Parse XML with XPath support
    json2doc::XmlDocument xmlDoc;
    xmlDoc.loadFromString(xmlContent);
    
    // 3. Query document structure
    auto paragraphs = xmlDoc.query("//*[local-name()='p']");
    std::cout << "Found " << paragraphs.size() << " paragraphs\n";
    
    // 4. Load JSON data
    json2doc::JsonMerge merger;
    merger.loadJson("data.json");
    
    // 5. Merge JSON values into XML using XPath
    int count = merger.mergeIntoXml(xmlDoc);
    std::cout << "Replaced " << count << " variables\n";
    
    // 6. Export merged XML
    std::string result = xmlDoc.toString();
    
    reader.cleanup();
    return 0;
}
```

### Quick Start

```bash
# Install dependencies
sudo apt-get install libpugixml-dev

# Run all tests
make test
make test-docx
make test-json-merge
make test-xml

# Run integration demos
make run-json-merge-test
make run-xml-integration
```

## Project Structure

```
json2doc/
├── include/          # Public header files
│   └── json2doc/
├── src/              # Source files
├── program/          # Main program
│   └── main.cpp
├── tests/            # Test files
├── Makefile          # Build configuration
└── README.md         # This file
```

Build artifacts are placed in:
- `build/` - Object files
- `bin/` - Executable binaries

## License

See LICENSE file for details.
