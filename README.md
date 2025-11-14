# json2doc

[![CI - Build and Test](https://github.com/EwertonDCSilv/json2doc/actions/workflows/ci.yml/badge.svg)](https://github.com/EwertonDCSilv/json2doc/actions/workflows/ci.yml)

Lib to transpose json data to document templates

## Description

json2doc is a C++ library designed to convert JSON data into various document formats using templates.

### Key Features

- **DocxReader**: Read and parse DOCX files, extract XML content
- **JsonMerge**: Merge JSON data into document templates with `{{variable}}` placeholders
- **Template-based**: Use DOCX templates with variables that are automatically replaced with JSON values
- **Nested JSON Support**: Access nested JSON values with dot notation (e.g., `{{metadata.version}}`)

### Components

1. **DocxReader** - Opens DOCX files, decompresses ZIP, reads and parses XML content
   - See [DOCX_READER.md](DOCX_READER.md) for details
   
2. **JsonMerge** - Finds and replaces `{{variables}}` in templates with JSON data
   - See [JSON_MERGE_README.md](JSON_MERGE_README.md) for details

3. **Integration** - Combine both to create dynamic documents from templates + data

## Building the Library

### Prerequisites

- GNU Make
- C++17 compatible compiler (GCC 7+, Clang 5+)

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
- `test-docx`: Build and run DocxReader tests
- `test-docx-main`: Build DocxReader standalone test program
- `run-docx-test`: Run DocxReader standalone test
- `test-json-merge`: Build and run JsonMerge tests (20 TDD tests)
- `test-json-merge-main`: Build JsonMerge + DocxReader integration test
- `run-json-merge-test`: Run JsonMerge integration test
- `all`: Build main program and all tests
- `run`: Run the main program
- `clean`: Remove all build artifacts

## Usage

### Basic Example

```cpp
#include "json2doc/docx_reader.h"
#include "json2doc/json_merge.h"

int main() {
    // 1. Read DOCX template with {{variables}}
    json2doc::DocxReader reader;
    reader.open("template.docx");
    reader.decompress();
    std::string xmlContent = reader.readDocumentXml();
    
    // 2. Load JSON data
    json2doc::JsonMerge merger;
    merger.loadJson("data.json");
    
    // 3. Merge JSON values into template
    std::string result = merger.replaceVariables(xmlContent);
    
    // 4. Check statistics
    auto stats = merger.getStats();
    std::cout << "Replaced: " << stats["replaced"] << " variables\n";
    
    reader.cleanup();
    return 0;
}
```

### Quick Start

```bash
# Run all tests
make test
make test-docx
make test-json-merge

# Run integration test (DOCX + JSON merge)
make run-json-merge-test
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
