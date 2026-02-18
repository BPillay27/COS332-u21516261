# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Apache default CGI directory
CGI_DIR = /usr/lib/cgi-bin

# Find all .cpp files in current directory
SOURCES := $(wildcard *.cpp)

# Create a list of executables with .cgi extension
TARGETS := $(SOURCES:.cpp=.cgi)

# Default target: build all executables in current directory
all: $(TARGETS)
	@echo "All .cpp files compiled into .cgi executables."

# Rule to compile each .cpp to .cgi
%.cgi: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Install compiled CGI executables to Apache's CGI dir (run with sudo)
install: all
	cp $(TARGETS) $(CGI_DIR)/
	chmod +x $(CGI_DIR)/*.cgi
	@echo "Installed to $(CGI_DIR)."

# Clean compiled executables in current directory
clean:
	rm -f $(TARGETS)
