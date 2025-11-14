# CI/CD Pipeline - GitHub Actions

## 📋 Visão Geral

Este repositório possui pipeline de Integração Contínua (CI) configurado com **GitHub Actions** que executa automaticamente em cada push ou pull request.

## 🔄 Quando é Executado

O pipeline é acionado em:
- ✅ Push para branch `main`
- ✅ Push para branch `develop`
- ✅ Pull Requests para `main` ou `develop`

## 🧪 Testes Executados

O pipeline executa os seguintes testes:

### 1. Testes do json2doc
```bash
make test
```
- Testa a classe Json2Doc
- Valida conversão JSON
- Verifica validação de JSON

### 2. Testes do DocxReader (TDD)
```bash
make test-docx
```
- Executa 15 testes unitários
- Testa abertura de arquivos
- Valida descompressão
- Verifica parsing de XML
- Testa tratamento de erros

### 3. Teste Standalone DocxReader
```bash
make test-docx-main
./bin/test_docx_reader_main
```
- Cria DOCX de exemplo
- Testa workflow completo
- Salva XMLs extraídos

## 📦 Artefatos

O pipeline gera artefatos que ficam disponíveis por **7 dias**:
- 📁 `bin/` - Executáveis compilados
- 📄 `temp_extracted_xml/` - XMLs extraídos nos testes

## 🛠️ Dependências Instaladas

O ambiente de CI instala automaticamente:
- `g++` - Compilador C++
- `make` - Sistema de build
- `unzip` - Para descomprimir DOCX
- `zip` - Para criar arquivos de teste

## ✅ Status do Build

Após configurar, você verá badges de status:
- ✅ Verde = Todos os testes passaram
- ❌ Vermelho = Falha em algum teste
- 🟡 Amarelo = Build em progresso

## 📊 Visualizar Resultados

1. Vá para a aba **Actions** no GitHub
2. Selecione o workflow **"CI - Build and Test"**
3. Clique em um run específico para ver detalhes
4. Baixe os artefatos se necessário

## 🔧 Arquivo de Configuração

O pipeline está definido em:
```
.github/workflows/ci.yml
```

## 🚀 Executar Localmente

Para reproduzir o CI localmente:

```bash
# Instalar dependências (Ubuntu/Debian)
sudo apt-get install -y g++ make unzip zip

# Compilar tudo
make all

# Executar testes
make test
make test-docx

# Teste standalone
make run-docx-test
```

## 📝 Logs

Cada etapa do pipeline gera logs detalhados:
- Instalação de dependências
- Compilação
- Execução de testes
- Upload de artefatos

## 🔒 Segurança

- Pipeline usa versões específicas de actions (@v4)
- Executa em ambiente isolado Ubuntu latest
- Não requer secrets ou credenciais

## 💡 Dicas

- Se um teste falhar, verifique os logs no GitHub Actions
- Artefatos podem ser baixados para debug local
- Pipeline também valida se o código compila corretamente
