CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
TARGET = RylanMoore_lab10
SOURCES = RylanMoore_lab10.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean

