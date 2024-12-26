# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++11

# Target executable
TARGET = needleman

# Source files
SRC = needleman_wunsch.cpp

# Build target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Clean target
clean:
	rm -f $(TARGET)
