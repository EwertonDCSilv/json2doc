NAME := json2doc
CC := g++
SRCDIR := src
TSTDIR := tests
PRGDIR := program
EXAMPLEDIR := examples/programs
OBJDIR := build
BINDIR := bin

MAIN := program/main.cpp

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# -g debug, --coverage para cobertura
CFLAGS := -g -Wall -O3 -std=c++17
INC := -I include/
LIBS := -lpugixml

#################################################################
##  					Build       	  						#
#################################################################

# Build object files from src
$(OBJDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Build main program
main: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(MAIN) $^ $(LIBS) -o $(BINDIR)/main


#################################################################
##  					Run Tests		  						#
#################################################################

# Build and run tests
test: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(TSTDIR)/test_json2doc.cpp $^ $(LIBS) -o $(BINDIR)/test_json2doc
	@echo "Running tests..."
	@$(BINDIR)/test_json2doc

#################################################################
##  					Build examples  						#
#################################################################

# Build and run DocxReader tests
test-docx: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(TSTDIR)/test_docx_reader.cpp $^ $(LIBS) -o $(BINDIR)/test_docx_reader
	@echo "Running DocxReader tests..."
	@$(BINDIR)/test_docx_reader


# Build DocxReader standalone test program
test-docx-main: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(EXAMPLEDIR)/test_docx_reader.cpp $^ $(LIBS) -o $(BINDIR)/test_docx_reader_main


# Build XmlDocument integration demo
test-xml-integration: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(EXAMPLEDIR)/test_xml_integration.cpp $^ $(LIBS) -o $(BINDIR)/test_xml_integration


# Build and run XmlDocument tests
test-xml: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(TSTDIR)/test_xml_document.cpp $^ $(LIBS) -o $(BINDIR)/test_xml_document
	@echo "Running XmlDocument tests..."
	@$(BINDIR)/test_xml_document


# Build and run JsonMerge tests
test-json-merge: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(TSTDIR)/test_json_merge.cpp $^ $(LIBS) -o $(BINDIR)/test_json_merge
	@echo "Running JsonMerge tests..."
	@$(BINDIR)/test_json_merge


# Build JsonMerge + DocxReader integration test program
test-json-merge-main: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(EXAMPLEDIR)/test_json_merge_docx.cpp $^ $(LIBS) -o $(BINDIR)/test_json_merge_docx


# Build example merge program
example-merge: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(EXAMPLEDIR)/example_merge.cpp $^ $(LIBS) -o $(BINDIR)/example_merge


# Build simple merge example
simple-merge: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(EXAMPLEDIR)/simple_merge_example.cpp $^ $(LIBS) -o $(BINDIR)/simple_merge_example


# Build complete DOCX merge (DOCX → XML → Merge → DOCX)
complete-docx-merge: $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INC) $(EXAMPLEDIR)/complete_docx_merge.cpp $^ $(LIBS) -o $(BINDIR)/complete_docx_merge


#################################################################
##  					Runners         						#
#################################################################

# Run DocxReader standalone test
run-docx-test: test-docx-main
	@echo "Running DocxReader standalone test..."
	@$(BINDIR)/test_docx_reader_main


# Run JsonMerge + DocxReader integration test
run-json-merge-test: test-json-merge-main
	@echo "Running JsonMerge + DocxReader integration test..."
	@$(BINDIR)/test_json_merge_docx


# Run XmlDocument integration demo
run-xml-integration: test-xml-integration
	@echo "Running XmlDocument + JsonMerge integration demo..."
	@$(BINDIR)/test_xml_integration	


# Run example merge
run-example: example-merge
	@$(BINDIR)/example_merge


# Run simple merge example
run-simple: simple-merge
	@$(BINDIR)/simple_merge_example


# Run main program
run: main
	$(BINDIR)/main


#################################################################
##  					Aux             						#
#################################################################


# Build all
all: main test test-docx test-json-merge test-xml


# Clean build artifacts
clean:
	$(RM) -r $(OBJDIR)/* $(BINDIR)/*


.PHONY: all main test test-docx test-docx-main run-docx-test test-json-merge test-json-merge-main run-json-merge-test test-xml test-xml-integration run-xml-integration example-merge simple-merge complete-docx-merge run-example run-simple run clean
