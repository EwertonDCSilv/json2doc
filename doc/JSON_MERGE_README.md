# JsonMerge - JSON to DOCX Template Merger

## Visão Geral

A classe `JsonMerge` permite mesclar dados de arquivos JSON em templates DOCX que contêm placeholders no formato `{{variável}}`. Esta funcionalidade é essencial para geração automatizada de documentos a partir de dados estruturados.

## Funcionalidades

### 1. **Carregamento de JSON**
- Carrega dados JSON de arquivo ou string
- Suporta estruturas JSON aninhadas com notação de ponto
- Exemplo: `{{metadata.version}}` acessa valores aninhados

### 2. **Detecção de Variáveis**
- Encontra todos os placeholders `{{variável}}` no texto
- Suporta espaços dentro dos placeholders: `{{ variável }}`
- Retorna lista de todas as variáveis encontradas

### 3. **Substituição de Variáveis**
- Substitui placeholders pelos valores do JSON
- Mantém placeholders que não têm correspondência no JSON
- Fornece estatísticas da operação (encontradas, substituídas, faltantes)

### 4. **Acesso a Dados**
- `getValue(key)`: Obtém valor por chave (suporta notação de ponto)
- `hasKey(key)`: Verifica se chave existe
- `getAllKeys()`: Retorna todas as chaves disponíveis

## Estrutura de Arquivos

```
include/json2doc/json_merge.h          # Interface da classe
src/json_merge.cpp                     # Implementação
tests/test_json_merge.cpp              # 20 testes TDD
program/test_json_merge_docx.cpp       # Teste de integração com DocxReader
```

## Uso Básico

### Exemplo 1: Substituição Simples

```cpp
#include "json2doc/json_merge.h"

json2doc::JsonMerge merger;

// Carregar JSON
merger.loadJson("data.json");

// Template com placeholders
std::string template_text = "Olá {{name}}, seu cargo é {{position}}.";

// Substituir variáveis
std::string result = merger.replaceVariables(template_text);
// Resultado: "Olá Ewerton Silva Santos, seu cargo é Software Engineer."
```

### Exemplo 2: Valores Aninhados

```cpp
// JSON: {"metadata": {"version": "1.0.0", "status": "draft"}}

std::string text = "Versão: {{metadata.version}}, Status: {{metadata.status}}";
std::string result = merger.replaceVariables(text);
// Resultado: "Versão: 1.0.0, Status: draft"
```

### Exemplo 3: Integração com DocxReader

```cpp
#include "json2doc/docx_reader.h"
#include "json2doc/json_merge.h"

// 1. Ler DOCX template
json2doc::DocxReader reader;
reader.open("template.docx");
reader.decompress();
std::string xmlContent = reader.readDocumentXml();

// 2. Carregar dados JSON
json2doc::JsonMerge merger;
merger.loadJson("data.json");

// 3. Mesclar dados no XML
std::string mergedXml = merger.replaceVariables(xmlContent);

// 4. Verificar estatísticas
auto stats = merger.getStats();
std::cout << "Variáveis substituídas: " << stats["replaced"] << "\n";
std::cout << "Variáveis faltantes: " << stats["missing"] << "\n";
```

## API da Classe

### Métodos Principais

#### `bool loadJson(const std::string &jsonFilePath)`
Carrega dados JSON de um arquivo.
- **Retorna**: `true` se sucesso, `false` se falha
- **Erro**: Disponível via `getLastError()`

#### `bool loadJsonString(const std::string &jsonString)`
Carrega dados JSON de uma string.
- **Retorna**: `true` se sucesso, `false` se falha

#### `std::vector<std::string> findVariables(const std::string &text)`
Encontra todos os placeholders `{{variável}}` no texto.
- **Retorna**: Vetor com nomes das variáveis (sem `{{` `}}`)

#### `std::string replaceVariables(const std::string &text)`
Substitui placeholders pelos valores do JSON.
- **Retorna**: Texto com variáveis substituídas
- **Nota**: Placeholders sem correspondência permanecem inalterados

#### `std::string getValue(const std::string &key)`
Obtém valor por chave (suporta notação de ponto para nested).
- **Retorna**: Valor como string, ou string vazia se não encontrado

#### `bool hasKey(const std::string &key)`
Verifica se uma chave existe no JSON.
- **Retorna**: `true` se existe, `false` caso contrário

#### `std::vector<std::string> getAllKeys()`
Retorna todas as chaves disponíveis.
- **Retorna**: Vetor com todas as chaves (incluindo nested com notação de ponto)

#### `std::map<std::string, int> getStats()`
Obtém estatísticas da última operação de substituição.
- **Retorna**: Map com "found", "replaced", "missing"

#### `void clear()`
Limpa todos os dados JSON carregados.

## Formato do JSON

### Exemplo de data.json

```json
{
    "title": "Sample Document",
    "author": "John Doe",
    "date": "November 14, 2025",
    "name": "Ewerton Silva Santos",
    "position": "Software Engineer",
    "metadata": {
        "version": "1.0.0",
        "type": "example",
        "status": "draft"
    }
}
```

### Acessando Dados

- Nível raiz: `{{title}}` → "Sample Document"
- Nested: `{{metadata.version}}` → "1.0.0"
- Nested profundo: `{{metadata.status}}` → "draft"

## Template DOCX

### Estrutura XML com Placeholders

```xml
<?xml version="1.0" encoding="UTF-8"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:body>
    <w:p>
      <w:r>
        <w:t>Título: {{title}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Autor: {{author}}</w:t>
      </w:r>
    </w:p>
    <w:p>
      <w:r>
        <w:t>Versão: {{metadata.version}}</w:t>
      </w:r>
    </w:p>
  </w:body>
</w:document>
```

Após a mesclagem, os placeholders são substituídos pelos valores reais do JSON.

## Compilação e Testes

### Compilar e Executar Testes Unitários (TDD)

```bash
make test-json-merge
```

**Resultado**: 20 testes executados, validando:
- Carregamento de JSON
- Detecção de variáveis
- Substituição simples e aninhada
- Estatísticas
- Casos extremos

### Compilar Programa de Integração

```bash
make test-json-merge-main
```

### Executar Teste de Integração

```bash
make run-json-merge-test
```

Ou com arquivo DOCX personalizado:

```bash
./bin/test_json_merge_docx meu_template.docx meus_dados.json
```

## Saída do Teste de Integração

O programa `test_json_merge_docx` demonstra o fluxo completo:

1. **Lê template DOCX** com variáveis `{{var}}`
2. **Carrega dados JSON**
3. **Detecta variáveis** no XML do DOCX
4. **Mescla valores** do JSON no XML
5. **Salva resultados** em `temp_extracted_xml/`:
   - `*_original.xml` - XML original com placeholders
   - `*_merged.xml` - XML com valores substituídos
6. **Exibe comparação** lado a lado

### Exemplo de Saída

```
✓ Found 8 variable placeholders:
  ✓ {{title}} → "Sample Document"
  ✓ {{author}} → "John Doe"
  ✓ {{date}} → "November 14, 2025"
  ✓ {{metadata.version}} → "1.0.0"

✓ Merge complete:
  • Variables found: 8
  • Variables replaced: 8
  • Variables missing: 0
```

## Casos de Uso

### 1. Geração de Contratos
```cpp
// Template: "O contratado {{name}} exercerá a função de {{position}}..."
// JSON: {"name": "João Silva", "position": "Desenvolvedor"}
// Resultado: "O contratado João Silva exercerá a função de Desenvolvedor..."
```

### 2. Relatórios Automáticos
```cpp
// Template: "Relatório {{metadata.type}} v{{metadata.version}}"
// JSON: {"metadata": {"type": "Mensal", "version": "2.1"}}
// Resultado: "Relatório Mensal v2.1"
```

### 3. Cartas Personalizadas
```cpp
// Template: "Prezado(a) {{name}}, informamos que em {{date}}..."
// JSON: {"name": "Maria Santos", "date": "15/11/2025"}
```

## Estatísticas de Substituição

```cpp
auto stats = merger.getStats();

std::cout << "Total encontradas: " << stats["found"] << "\n";
std::cout << "Substituídas: " << stats["replaced"] << "\n";
std::cout << "Faltantes no JSON: " << stats["missing"] << "\n";
```

Útil para:
- Validar completude dos dados
- Detectar variáveis faltantes
- Log de operações

## Tratamento de Erros

### Variáveis Não Encontradas

Placeholders sem correspondência no JSON **permanecem inalterados**:

```cpp
// Template: "Nome: {{name}}, Idade: {{age}}"
// JSON: {"name": "João"} (sem "age")
// Resultado: "Nome: João, Idade: {{age}}"
```

### JSON Inválido

```cpp
if (!merger.loadJson("invalid.json")) {
    std::cerr << "Erro: " << merger.getLastError() << "\n";
}
```

## Limitações Conhecidas

1. **Arrays**: Valores de array são preservados como string JSON, não expandidos
2. **Tipos**: Todos os valores são convertidos para string
3. **Recursão**: Nested profundo suporta apenas notação de ponto (não arrays indexados)

## Integração no Pipeline CI/CD

Os testes do JsonMerge estão integrados no GitHub Actions:

```yaml
- name: Run JsonMerge tests
  run: make test-json-merge
```

Executado automaticamente em cada push/PR.

## Estrutura de Testes TDD

### 20 Testes Implementados

1. Constructor cria objeto válido
2. Load JSON de string
3. Load JSON inválido falha graciosamente
4. Get value por chave
5. Get nested value com notação de ponto
6. Get chave inexistente retorna vazio
7. hasKey retorna boolean correto
8. Find variables em texto
9. Find variables com espaços
10. Find no variables em texto puro
11. Replace simple variables
12. Replace nested variables
13. Replace com variáveis faltantes
14. Get statistics após substituição
15. Clear data
16. Load JSON de arquivo
17. Replace em conteúdo XML
18. Get all keys
19. Multiple replacements da mesma variável
20. Empty template

## Próximos Passos

- [ ] Suporte a formatação de valores (números, datas)
- [ ] Expansão de arrays em templates
- [ ] Cache de JSON parseado
- [ ] Validação de schema JSON
- [ ] Suporte a expressões (ex: `{{price * quantity}}`)

## Contribuindo

Para adicionar novos testes ou funcionalidades:

1. Adicione testes TDD em `tests/test_json_merge.cpp`
2. Implemente funcionalidade em `src/json_merge.cpp`
3. Execute `make test-json-merge` para validar
4. Execute `make run-json-merge-test` para teste de integração

## Licença

Mesmo licenciamento do projeto json2doc.
