CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS =

TARGET = smtp_proxy
ZIPNAME = u24594522_u21516261.zip

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

zip: clean
	zip $(ZIPNAME) *.cpp *.h Makefile .env

rz:
	rm -f $(ZIPNAME)

hash: $(TARGET)
	md5sum $(TARGET)

.PHONY: all run clean zip rz hash