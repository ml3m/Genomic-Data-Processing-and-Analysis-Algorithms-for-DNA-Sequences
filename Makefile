# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++17 -O2 -I/opt/homebrew/Cellar/opencl-clhpp-headers/2024.10.24_1/include
LDFLAGS = -framework OpenCL

# Target executable
TARGET = gc_content

# Source files
SRC = main.cpp

# Build target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Clean target
clean:
	rm -f $(TARGET)
