#include "json2doc/docx_reader.h"
#include "json2doc/xml_document.h"
#include "json2doc/json_merge.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace json2doc;

int main(int argc, char *argv[])
{
    // Verificar argumentos
    if (argc < 4)
    {
        std::cerr << "Uso: " << argv[0] << " <template.docx> <dados.json> <output.docx>\n";
        std::cerr << "\nExemplo:\n";
        std::cerr << "  " << argv[0] << " template.docx dados.json output.docx\n";
        return 1;
    }

    std::string inputDocx = argv[1];
    std::string inputJson = argv[2];
    std::string outputDocx = argv[3];

    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                           ║\n";
    std::cout << "║     📄 Merge JSON → DOCX (Fluxo Completo)                ║\n";
    std::cout << "║                                                           ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    // ========================================================================
    // ETAPA 1: Abrir e descomprimir DOCX
    // ========================================================================
    std::cout << "1️⃣  Abrindo template DOCX...\n";
    DocxReader reader;
    if (!reader.open(inputDocx))
    {
        std::cerr << "❌ Erro ao abrir: " << reader.getLastError() << "\n";
        return 1;
    }
    std::cout << "   ✓ Arquivo aberto: " << inputDocx << "\n\n";

    std::cout << "2️⃣  Descomprimindo DOCX...\n";
    if (!reader.decompress())
    {
        std::cerr << "❌ Erro ao descomprimir: " << reader.getLastError() << "\n";
        return 1;
    }
    std::cout << "   ✓ Extraído para: " << reader.getTempPath() << "\n\n";

    // ========================================================================
    // ETAPA 2: Ler XML do DOCX
    // ========================================================================
    std::cout << "3️⃣  Lendo XML do documento...\n";
    std::string docxXml = reader.readDocumentXml();
    if (docxXml.empty())
    {
        std::cerr << "❌ Erro ao ler XML: " << reader.getLastError() << "\n";
        return 1;
    }
    std::cout << "   ✓ XML extraído (" << docxXml.size() << " bytes)\n\n";

    // ========================================================================
    // ETAPA 3: Parse XML com XmlDocument
    // ========================================================================
    std::cout << "4️⃣  Parseando XML com XPath...\n";
    XmlDocument xmlDoc;
    if (!xmlDoc.loadFromString(docxXml))
    {
        std::cerr << "❌ Erro ao parsear XML\n";
        return 1;
    }
    std::cout << "   ✓ XML parseado com sucesso\n\n";

    // ========================================================================
    // ETAPA 4: Encontrar variáveis
    // ========================================================================
    std::cout << "5️⃣  Procurando variáveis {{...}} no template...\n";
    auto templateNodes = xmlDoc.findTemplateNodes();
    std::cout << "   ✓ Encontradas " << templateNodes.size() << " ocorrências\n\n";

    // ========================================================================
    // ETAPA 5: Carregar JSON
    // ========================================================================
    std::cout << "6️⃣  Carregando dados do JSON...\n";
    JsonMerge merger;
    if (!merger.loadJson(inputJson))
    {
        std::cerr << "❌ Erro ao carregar JSON\n";
        return 1;
    }
    std::cout << "   ✓ JSON carregado: " << inputJson << "\n\n";

    // ========================================================================
    // ETAPA 6: Processar tabelas (se houver listas no JSON)
    // ========================================================================
    std::cout << "7️⃣  Processando tabelas com listas...\n";
    int rowsCreated = merger.processTablesInXml(xmlDoc);
    if (rowsCreated > 0)
    {
        std::cout << "   ✅ " << rowsCreated << " linhas de tabela criadas!\n\n";
    }
    else
    {
        std::cout << "   ℹ️  Nenhuma tabela com listas encontrada\n\n";
    }

    // ========================================================================
    // ETAPA 7: MERGE! (Substituir variáveis simples)
    // ========================================================================
    std::cout << "8️⃣  Executando merge de variáveis simples...\n";
    std::cout << "   🔄 Substituindo variáveis...\n";
    int replaced = merger.mergeIntoXml(xmlDoc);
    std::cout << "   ✅ " << replaced << " variáveis substituídas!\n\n";

    // ========================================================================
    // ETAPA 8: Escrever XML modificado de volta
    // ========================================================================
    std::cout << "9️⃣  Salvando XML modificado...\n";
    std::string mergedXml = xmlDoc.toString();
    if (!reader.writeDocumentXml(mergedXml))
    {
        std::cerr << "❌ Erro ao escrever XML: " << reader.getLastError() << "\n";
        return 1;
    }
    std::cout << "   ✓ XML atualizado no temp directory\n\n";

    // ========================================================================
    // ETAPA 9: Recomprimir em DOCX
    // ========================================================================
    std::cout << "🔟 Recomprimindo em DOCX...\n";
    if (!reader.recompress(outputDocx))
    {
        std::cerr << "❌ Erro ao recomprimir: " << reader.getLastError() << "\n";
        return 1;
    }
    std::cout << "   ✓ DOCX gerado: " << outputDocx << "\n\n";

    // ========================================================================
    // RESUMO FINAL
    // ========================================================================
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                           ║\n";
    std::cout << "║         ✅ DOCX gerado com sucesso! ✅                   ║\n";
    std::cout << "║                                                           ║\n";
    std::cout << "║  Fluxo executado:                                         ║\n";
    std::cout << "║  1. " << inputDocx << " (template)                        \n";
    std::cout << "║  2. Extrair XML do DOCX                                   ║\n";
    std::cout << "║  3. Parse com XPath (pugixml)                             ║\n";
    std::cout << "║  4. Processar tabelas: " << rowsCreated << " linhas criadas           \n";
    std::cout << "║  5. Merge com " << inputJson << "                         \n";
    std::cout << "║  6. " << replaced << " variáveis substituídas             \n";
    std::cout << "║  7. Escrever XML de volta                                 ║\n";
    std::cout << "║  8. Recomprimir em DOCX                                   ║\n";
    std::cout << "║  9. ✨ " << outputDocx << " ✨                            \n";
    std::cout << "║                                                           ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    return 0;
}
