CC := g++
SRCDIR := src
TSTDIR := tests
OBJDIR := build
BINDIR := bin

MAIN := program/main.cpp

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# -g debug, --coverage para cobertura
CFLAGS := -g -Wall -O3 -std=c++17
INC := -I include/

# Build object files from src
$(OBJDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Build main program
main: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(MAIN) $^ -o $(BINDIR)/main

# Build and run tests
test: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(TSTDIR)/test_json2doc.cpp $^ -o $(BINDIR)/test_json2doc
	@echo "Running tests..."
	@$(BINDIR)/test_json2doc

# Build and run DocxReader tests
test-docx: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(TSTDIR)/test_docx_reader.cpp $^ -o $(BINDIR)/test_docx_reader
	@echo "Running DocxReader tests..."
	@$(BINDIR)/test_docx_reader

# Build DocxReader standalone test program
test-docx-main: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) program/test_docx_reader.cpp $^ -o $(BINDIR)/test_docx_reader_main

# Run DocxReader standalone test
run-docx-test: test-docx-main
	@echo "Running DocxReader standalone test..."
	@$(BINDIR)/test_docx_reader_main

# Build all
all: main test test-docx

# Run main program
run: main
	$(BINDIR)/main

# Clean build artifacts
clean:
	$(RM) -r $(OBJDIR)/* $(BINDIR)/*

.PHONY: all main test test-docx test-docx-main run-docx-test run clean
