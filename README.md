# json2doc

Lib to transpose json data to document templates

## Description

json2doc is a C++ library designed to convert JSON data into various document formats using templates.

## Building the Library

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Run tests (optional)
ctest

# Install (optional)
sudo cmake --install .
```

### Build Options

- `BUILD_EXAMPLES`: Build example programs (default: ON)
- `BUILD_TESTS`: Build test suite (default: ON)

To disable examples or tests:

```bash
cmake -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF ..
```

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
./build/examples/example_usage
```

## Project Structure

```
json2doc/
├── include/          # Public header files
│   └── json2doc/
├── src/              # Source files
├── examples/         # Example programs
├── tests/            # Test files
├── CMakeLists.txt    # Main build configuration
└── README.md         # This file
```

## License

See LICENSE file for details.
