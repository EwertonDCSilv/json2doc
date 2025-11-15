#include "json2doc/docx_reader.h"
#include "json2doc/xml_document.h"
#include "json2doc/json_merge.h"
#include <iostream>
#include <fstream>

using namespace json2doc;

int main()
{
    std::cout << "=== Exemplo: Merge JSON → XML (Novo Modelo) ===\n\n";

    // 1. Extrair XML do DOCX
    std::cout << "1. Extraindo XML do template.docx...\n";
    DocxReader reader;
    if (!reader.open("template.docx"))
    {
        std::cerr << "Erro: arquivo template.docx não encontrado\n";
        return 1;
    }
    std::string docxXml = reader.readDocumentXml();
    std::cout << "   ✓ XML extraído (" << docxXml.size() << " bytes)\n\n";

    // 2. Parse XML com XmlDocument
    std::cout << "2. Parseando XML com XPath...\n";
    XmlDocument xmlDoc;
    if (!xmlDoc.loadFromString(docxXml))
    {
        std::cerr << "Erro ao parsear XML\n";
        return 1;
    }
    std::cout << "   ✓ XML parseado com sucesso\n\n";

    // 3. Verificar variáveis no template
    std::cout << "3. Procurando variáveis {{...}} no template...\n";
    auto templateNodes = xmlDoc.findTemplateNodes();
    std::cout << "   ✓ Encontradas " << templateNodes.size() << " ocorrências\n\n";

    // 4. Carregar JSON
    std::cout << "4. Carregando dados do JSON...\n";
    JsonMerge merger;
    if (!merger.loadJson("data.json"))
    {
        std::cerr << "Erro: arquivo data.json não encontrado\n";
        return 1;
    }
    std::cout << "   ✓ JSON carregado\n\n";

    // 5. MERGE! (Novo modelo - trabalha direto com XML parseado)
    std::cout << "5. Executando merge JSON → XML...\n";
    int replaced = merger.mergeIntoXml(xmlDoc); // ← NOVA API!
    std::cout << "   ✓ " << replaced << " variáveis substituídas\n\n";

    // 6. Exportar resultado
    std::cout << "6. Salvando resultado...\n";
    std::string mergedXml = xmlDoc.toString();

    // Salvar em arquivo
    std::ofstream out("output_merged.xml");
    out << mergedXml;
    out.close();

    std::cout << "   ✓ Salvo em output_merged.xml (" << mergedXml.size() << " bytes)\n\n";

    std::cout << "========================================\n";
    std::cout << "✅ Merge concluído com sucesso!\n";
    std::cout << "========================================\n";

    return 0;
}
