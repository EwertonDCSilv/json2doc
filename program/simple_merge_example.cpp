#include "json2doc/xml_document.h"
#include "json2doc/json_merge.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace json2doc;

int main()
{
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                      ║\n";
    std::cout << "║     📝 Exemplo: Merge JSON → XML (Novo Modelo)      ║\n";
    std::cout << "║                                                      ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    // 1. Carregar template XML
    std::cout << "1️⃣  Carregando template XML...\n";
    std::ifstream templateFile("temp_extracted_xml/template.xml");
    if (!templateFile)
    {
        std::cerr << "❌ Erro: arquivo temp_extracted_xml/template.xml não encontrado\n";
        return 1;
    }
    std::stringstream buffer;
    buffer << templateFile.rdbuf();
    std::string xmlContent = buffer.str();
    templateFile.close();
    std::cout << "   ✓ Template carregado (" << xmlContent.size() << " bytes)\n\n";

    // 2. Parse XML com XmlDocument
    std::cout << "2️⃣  Parseando XML com XPath...\n";
    XmlDocument xmlDoc;
    if (!xmlDoc.loadFromString(xmlContent))
    {
        std::cerr << "❌ Erro ao parsear XML\n";
        return 1;
    }
    std::cout << "   ✓ XML parseado com sucesso\n\n";

    // 3. Verificar variáveis no template
    std::cout << "3️⃣  Procurando variáveis {{...}} no template...\n";
    auto templateNodes = xmlDoc.findTemplateNodes();
    std::cout << "   ✓ Encontradas " << templateNodes.size() << " ocorrências de variáveis\n\n";

    // 4. Carregar JSON
    std::cout << "4️⃣  Carregando dados do JSON...\n";
    JsonMerge merger;
    if (!merger.loadJson("dados.json"))
    {
        std::cerr << "❌ Erro: arquivo dados.json não encontrado\n";
        return 1;
    }
    std::cout << "   ✓ JSON carregado\n\n";

    // 5. MERGE! (Novo modelo - trabalha direto com XML parseado)
    std::cout << "5️⃣  Executando merge JSON → XML...\n";
    std::cout << "   🔄 Substituindo variáveis...\n";
    int replaced = merger.mergeIntoXml(xmlDoc); // ← NOVA API!
    std::cout << "   ✅ " << replaced << " variáveis substituídas com sucesso!\n\n";

    // 6. Exportar resultado
    std::cout << "6️⃣  Salvando resultado...\n";
    std::string mergedXml = xmlDoc.toString();

    // Salvar em arquivo
    std::ofstream out("temp_extracted_xml/output_merged.xml");
    out << mergedXml;
    out.close();

    std::cout << "   ✓ Salvo em temp_extracted_xml/output_merged.xml (" << mergedXml.size() << " bytes)\n\n";

    // 7. Preview do resultado
    std::cout << "7️⃣  Preview (primeiras linhas):\n";
    std::cout << "   ┌─────────────────────────────────────────────────┐\n";
    std::istringstream previewStream(mergedXml);
    std::string line;
    int lineCount = 0;
    while (std::getline(previewStream, line) && lineCount < 8)
    {
        std::cout << "   │ " << line << "\n";
        lineCount++;
    }
    std::cout << "   └─────────────────────────────────────────────────┘\n\n";

    std::cout << "╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                      ║\n";
    std::cout << "║         ✅ Merge concluído com sucesso! ✅          ║\n";
    std::cout << "║                                                      ║\n";
    std::cout << "║  Arquivos:                                           ║\n";
    std::cout << "║  • temp_extracted_xml/template.xml                   ║\n";
    std::cout << "║    → Template com {{variáveis}}                      ║\n";
    std::cout << "║  • dados.json                                        ║\n";
    std::cout << "║    → Valores para substituir                         ║\n";
    std::cout << "║  • temp_extracted_xml/output_merged.xml              ║\n";
    std::cout << "║    → Resultado final! 🎉                            ║\n";
    std::cout << "║                                                      ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    return 0;
}
