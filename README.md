# json2doc

Lib to transpose json data to document templates

## Description

json2doc is a C++ library designed to convert JSON data into various document formats using templates.

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
- `test`: Build and run the test suite
- `all`: Build main program and tests
- `run`: Run the main program
- `clean`: Remove all build artifacts

## Usage

### Basic Example

```cpp
#include <json2doc/json2doc.h>
#include <json2doc/converter.h>

int main() {
    json2doc::Json2Doc converter;
    
    std::string jsonData = R"({"title": "My Document"})";
    converter.loadJson(jsonData);
    
    std::string result = converter.convertToDocument("template.docx");
    return 0;
}
```

### Running the Program

After building, you can run the main program:

```bash
make run
# or directly:
./bin/main
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
