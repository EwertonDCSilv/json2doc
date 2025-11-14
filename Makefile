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

# Build all
all: main test

# Run main program
run: main
	$(BINDIR)/main

# Clean build artifacts
clean:
	$(RM) -r $(OBJDIR)/* $(BINDIR)/*

.PHONY: all main test run clean
