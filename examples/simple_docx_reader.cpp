// Exemplo de uso simples da classe DocxReader
// Compile com: make test-docx-main && ./bin/test_docx_reader_main seu_arquivo.docx

#include <iostream>
#include "json2doc/docx_reader.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Uso: " << argv[0] << " <arquivo.docx>\n";
        return 1;
    }

    json2doc::DocxReader reader;

    // Workflow completo
    if (!reader.open(argv[1]))
    {
        std::cerr << "❌ " << reader.getLastError() << "\n";
        return 1;
    }

    if (!reader.decompress())
    {
        std::cerr << "❌ " << reader.getLastError() << "\n";
        return 1;
    }

    std::string text = reader.parseXmlContent();
    std::cout << "📄 Texto extraído:\n"
              << text << "\n";

    return 0;
}
