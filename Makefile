CC := g++
SRCDIR := src
TSTDIR := tests
OBJDIR := build
BINDIR := bin
LIBDIR := lib
EXMDIR := examples

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Test files
TESTS := $(shell find $(TSTDIR) -type f -name *.$(SRCEXT))
TESTOBJS := $(patsubst $(TSTDIR)/%,$(OBJDIR)/$(TSTDIR)/%,$(TESTS:.$(SRCEXT)=.o))

# Example files
EXAMPLES := $(shell find $(EXMDIR) -type f -name *.$(SRCEXT))
EXAMPLEOBJS := $(patsubst $(EXMDIR)/%,$(OBJDIR)/$(EXMDIR)/%,$(EXAMPLES:.$(SRCEXT)=.o))

# -g debug, --coverage para cobertura
CFLAGS := -g -Wall -O3 -std=c++17
INC := -I include/

# Library name
LIBNAME := libjson2doc.a

# Build object files from src
$(OBJDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Build object files from tests
$(OBJDIR)/$(TSTDIR)/%.o: $(TSTDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Build object files from examples
$(OBJDIR)/$(EXMDIR)/%.o: $(EXMDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Build static library
lib: $(OBJECTS)
	@mkdir -p $(LIBDIR)
	ar rcs $(LIBDIR)/$(LIBNAME) $^
	@echo "Library built: $(LIBDIR)/$(LIBNAME)"

# Build tests
test: lib $(TESTOBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(OBJDIR)/$(TSTDIR)/test_json2doc.o $(LIBDIR)/$(LIBNAME) -o $(BINDIR)/test_json2doc
	@echo "Running tests..."
	@$(BINDIR)/test_json2doc

# Build examples
example: lib $(EXAMPLEOBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(OBJDIR)/$(EXMDIR)/example_usage.o $(LIBDIR)/$(LIBNAME) -o $(BINDIR)/example_usage
	@echo "Example built: $(BINDIR)/example_usage"

# Build all
all: lib test example

# Run example
run: example
	$(BINDIR)/example_usage

# Clean build artifacts
clean:
	$(RM) -r $(OBJDIR)/* $(BINDIR)/* $(LIBDIR)/*

.PHONY: all lib test example run clean
