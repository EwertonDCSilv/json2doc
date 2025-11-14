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
# Build the library
make lib

# Build and run tests
make test

# Build examples
make example

# Build everything (library, tests, and examples)
make all

# Run the example program
make run

# Clean build artifacts
make clean
```

### Build Targets

- `lib`: Build the static library (libjson2doc.a)
- `test`: Build and run the test suite
- `example`: Build the example programs
- `all`: Build library, tests, and examples
- `run`: Run the example program
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

### Running Examples

After building, you can run the example program:

```bash
make run
# or directly:
./bin/example_usage
```

## Project Structure

```
json2doc/
├── include/          # Public header files
│   └── json2doc/
├── src/              # Source files
├── examples/         # Example programs
├── tests/            # Test files
├── Makefile          # Build configuration
└── README.md         # This file
```

Build artifacts are placed in:
- `build/` - Object files
- `lib/` - Compiled library (libjson2doc.a)
- `bin/` - Executable binaries

## License

See LICENSE file for details.
