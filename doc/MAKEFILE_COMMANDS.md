# Comandos Make para DocxReader

## 📦 Compilação

### Compilar tudo (main + todos os testes)
```bash
make all
```

### Compilar apenas a classe DocxReader
```bash
make  # Compila os objetos automaticamente
```

### Limpar arquivos de compilação
```bash
make clean
```

## 🧪 Testes

### Executar testes unitários TDD (15 testes)
```bash
make test-docx
```
**Saída esperada:** 
```
╔════════════════════════════════════════════════════════╗
║     DocxReader TDD Unit Tests                          ║
╚════════════════════════════════════════════════════════╝

✓ Test 1 passed: Constructor creates valid object
✓ Test 2 passed: Destructor cleanup executed
...
✓ Test 15 passed: getTempPath returns correct path

╔════════════════════════════════════════════════════════╗
║  ✓ All 15 tests passed successfully!                  ║
╚════════════════════════════════════════════════════════╝
```

### Compilar programa de teste standalone
```bash
make test-docx-main
```

### Executar programa de teste standalone
```bash
make run-docx-test
```
**Ou com arquivo personalizado:**
```bash
./bin/test_docx_reader_main meu_arquivo.docx
```

## 🎯 Testes Originais do Projeto

### Executar testes do json2doc
```bash
make test
```

## 🏃 Execução

### Executar programa principal
```bash
make run
```

## 📋 Resumo de Targets

| Target | Descrição |
|--------|-----------|
| `make` ou `make main` | Compila o programa principal |
| `make all` | Compila tudo (main + testes) |
| `make test` | Testes unitários json2doc |
| `make test-docx` | Testes unitários DocxReader (TDD) |
| `make test-docx-main` | Compila programa standalone |
| `make run-docx-test` | Executa programa standalone |
| `make run` | Executa programa principal |
| `make clean` | Remove arquivos compilados |

## 💡 Exemplos de Uso

### Workflow completo de desenvolvimento
```bash
# 1. Limpar e recompilar tudo
make clean && make all

# 2. Executar testes do DocxReader
make test-docx

# 3. Testar com arquivo real
./bin/test_docx_reader_main meu_documento.docx
```

### Debug rápido
```bash
# Compilar apenas os objetos
make

# Compilar e testar
make test-docx
```

### Criar DOCX de teste automaticamente
```bash
# O programa cria um DOCX de exemplo se não fornecer arquivo
./bin/test_docx_reader_main
```

## 🔍 Verificar Compilação

### Ver arquivos compilados
```bash
ls -lh bin/
ls -lh build/
```

### Ver saída detalhada
```bash
make clean
make all 2>&1 | tee build.log
```

## 🐛 Troubleshooting

### Se houver erro de compilação:
```bash
make clean
make test-docx-main VERBOSE=1
```

### Se faltar dependências:
```bash
# Ubuntu/Debian
sudo apt-get install g++ make unzip zip

# Fedora/RHEL  
sudo dnf install gcc-c++ make unzip zip
```

### Verificar versão do compilador:
```bash
g++ --version
# Requerido: g++ com suporte a C++17
```

## 📊 Estrutura de Build

```
build/               # Arquivos objeto (.o)
├── converter.o
├── json2doc.o
├── docx_reader.o   # ← Novo
├── args_parser.o
└── help.o

bin/                 # Executáveis
├── main
├── test_json2doc
├── test_docx_reader        # ← Novo (testes unitários)
└── test_docx_reader_main   # ← Novo (programa standalone)
```

## ⚡ Quick Start

```bash
# Clone e configure (se ainda não fez)
cd /home/ewert/github/json2doc

# Compilar e testar tudo
make clean && make all

# Sucesso! ✨
```
