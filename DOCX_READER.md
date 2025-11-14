# DocxReader - Documentação

## Visão Geral

A classe `DocxReader` faz parte do projeto `json2doc` e fornece funcionalidade para ler, descomprimir e interpretar arquivos DOCX (Microsoft Word).

## Recursos

- **Abertura de arquivos DOCX**: Valida e abre arquivos DOCX
- **Descompressão ZIP**: Extrai o conteúdo do arquivo DOCX (que é um arquivo ZIP) para um diretório temporário
- **Leitura de XML**: Acessa e lê o arquivo `document.xml` que contém o conteúdo do documento
- **Parsing de XML**: Extrai texto do XML estruturado do Word
- **Gerenciamento automático de recursos**: Limpeza automática de arquivos temporários no destrutor

## Arquivos

```
include/json2doc/docx_reader.h       # Header da classe
src/docx_reader.cpp                   # Implementação da classe
program/test_docx_reader.cpp          # Programa de teste standalone
tests/test_docx_reader.cpp            # Testes unitários TDD (15 testes)
```

## Interface da Classe

### Construtor e Destrutor

```cpp
DocxReader();   // Construtor
~DocxReader();  // Destrutor (limpa arquivos temporários automaticamente)
```

### Métodos Principais

#### `bool open(const std::string& filePath)`
Abre um arquivo DOCX para processamento.

**Parâmetros:**
- `filePath`: Caminho para o arquivo DOCX

**Retorna:** `true` se o arquivo foi aberto com sucesso, `false` caso contrário

**Exemplo:**
```cpp
json2doc::DocxReader reader;
if (reader.open("document.docx")) {
    // Arquivo aberto com sucesso
}
```

#### `bool decompress()`
Descomprime o arquivo DOCX para um diretório temporário.

**Retorna:** `true` se a descompressão foi bem-sucedida, `false` caso contrário

**Exemplo:**
```cpp
if (reader.decompress()) {
    std::cout << "Descomprimido em: " << reader.getTempPath() << std::endl;
}
```

#### `std::string readDocumentXml()`
Lê o conteúdo XML do documento principal (`word/document.xml`).

**Retorna:** String com o conteúdo XML completo

**Exemplo:**
```cpp
std::string xml = reader.readDocumentXml();
```

#### `std::string parseXmlContent()`
Extrai o texto do conteúdo XML (tags `<w:t>`).

**Retorna:** String com o texto extraído do documento

**Exemplo:**
```cpp
std::string text = reader.parseXmlContent();
std::cout << "Texto: " << text << std::endl;
```

#### `void printXml() const`
Imprime o conteúdo XML no stdout para debug.

**Exemplo:**
```cpp
reader.printXml();
```

#### `std::string getTempPath() const`
Retorna o caminho do diretório temporário criado.

**Retorna:** String com o caminho do diretório temporário

#### `std::string getLastError() const`
Retorna a última mensagem de erro.

**Retorna:** String com a mensagem de erro

#### `void cleanup()`
Remove manualmente os arquivos temporários (também é chamado automaticamente no destrutor).

## Exemplo de Uso Completo

```cpp
#include "json2doc/docx_reader.h"
#include <iostream>

int main() {
    json2doc::DocxReader reader;
    
    // 1. Abrir arquivo
    if (!reader.open("meu_documento.docx")) {
        std::cerr << "Erro: " << reader.getLastError() << std::endl;
        return 1;
    }
    
    // 2. Descomprimir
    if (!reader.decompress()) {
        std::cerr << "Erro: " << reader.getLastError() << std::endl;
        return 1;
    }
    
    // 3. Ler XML
    std::string xml = reader.readDocumentXml();
    
    // 4. Exibir XML (debug)
    reader.printXml();
    
    // 5. Extrair texto
    std::string text = reader.parseXmlContent();
    std::cout << "Texto extraído: " << text << std::endl;
    
    // Cleanup automático no destrutor
    return 0;
}
```

## Compilação

### Compilar o programa de teste standalone:
```bash
make test-docx-main
```

### Executar o programa de teste standalone:
```bash
make run-docx-test
# ou
./bin/test_docx_reader_main [arquivo.docx]
```

### Compilar e executar testes unitários TDD:
```bash
make test-docx
```

### Compilar tudo:
```bash
make all
```

## Testes

### Testes Unitários TDD (15 testes)

Os testes cobrem:

1. ✓ Construtor cria objeto válido
2. ✓ Destrutor executa cleanup
3. ✓ Abertura de arquivo inexistente retorna erro
4. ✓ Abertura de arquivo válido funciona
5. ✓ Descompressão sem abrir arquivo retorna erro
6. ✓ Descompressão cria diretório temporário
7. ✓ Leitura de XML sem descompressão retorna vazio
8. ✓ Leitura de XML após descompressão retorna conteúdo
9. ✓ Parsing de XML extrai texto
10. ✓ Cleanup remove diretório temporário
11. ✓ Print XML sem conteúdo não causa crash
12. ✓ Mensagens de erro são descritivas
13. ✓ Múltiplas operações funcionam corretamente
14. ✓ Parsing de documento vazio é tratado
15. ✓ getTempPath retorna caminho correto

### Programa de Teste Standalone

O programa `test_docx_reader.cpp` demonstra o uso completo da classe:
- Cria um arquivo DOCX de exemplo se nenhum for fornecido
- Executa todos os métodos da classe
- Exibe resultados formatados com emojis e separadores
- **Salva automaticamente o XML extraído** em `temp_extracted_xml/<nome_arquivo>_document.xml`

## Dependências

- **C++17**: Requer compilador com suporte a C++17
- **unzip**: Utilitário `unzip` deve estar instalado no sistema
- **zip**: Utilitário `zip` para criar arquivos de teste

```bash
# Ubuntu/Debian
sudo apt-get install unzip zip

# Fedora/RHEL
sudo dnf install unzip zip
```

## Limitações e Notas

1. **Formato XML**: Atualmente extrai apenas tags `<w:t>` (texto simples)
2. **Formatação**: Não preserva formatação (negrito, itálico, etc.)
3. **Estrutura**: Não processa tabelas, imagens ou elementos complexos
4. **Encoding**: Assume UTF-8
5. **Plataforma**: Testado em Linux (usa comandos `unzip`, `mkdtemp`, `system`)

## Estrutura do DOCX

Um arquivo DOCX é essencialmente um arquivo ZIP contendo:

```
documento.docx/
├── [Content_Types].xml
├── _rels/
│   └── .rels
└── word/
    ├── document.xml      # <-- Conteúdo principal (lido pela classe)
    ├── styles.xml
    ├── fontTable.xml
    └── _rels/
        └── document.xml.rels
```

A classe `DocxReader` foca no arquivo `word/document.xml` que contém o texto do documento.

## Tratamento de Erros

A classe usa o método `getLastError()` para fornecer mensagens descritivas:

```cpp
if (!reader.open("arquivo.docx")) {
    std::cerr << "Falha: " << reader.getLastError() << std::endl;
}
```

Exemplos de mensagens de erro:
- "File does not exist: [caminho]"
- "No file is currently open"
- "Failed to create temporary directory"
- "Unzip failed: [detalhes]"
- "Failed to open document.xml at: [caminho]"

## Contribuindo

Para adicionar novos recursos:

1. Adicione métodos ao header (`docx_reader.h`)
2. Implemente no source (`docx_reader.cpp`)
3. Adicione testes unitários em `tests/test_docx_reader.cpp`
4. Atualize este README

## Licença

Este componente segue a mesma licença do projeto json2doc.
