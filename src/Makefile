# Compiler
CXX := g++

# Compiler Flags
CXXFLAGS := -Wall -Wextra -pedantic -std=c++17 -O2

# Folder Paths
INCLUDES := Includes
OBJECTS  := Objects
SOURCES  := Sources

# Source and object files
CPP_SOURCES := $(wildcard $(SOURCES)/*.cpp)
OBJ := $(patsubst $(SOURCES)/%.cpp,$(OBJECTS)/%.o,$(CPP_SOURCES))

# Include directory
CXXFLAGS += -I$(INCLUDES)

# Executable output inside src folder
TARGET := chip-8.exe

# Default target: build and run
all: $(TARGET)
	@echo Building and running $(TARGET)...
	@./$(TARGET)

# Link object files into executable
$(TARGET): $(OBJ)
	@echo Linking object files into $(TARGET)...
	$(CXX) $(OBJ) -lgdiplus -lwinmm -mwindows -o $(TARGET)

# Compile .cpp files into .o object files
$(OBJECTS)/%.o: $(SOURCES)/%.cpp | $(OBJECTS)
	@echo Compiling $< into $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Create Objects directory if it doesn't exist
$(OBJECTS):
	@echo Creating directory $(OBJECTS)...
	@mkdir $(OBJECTS)

# Clean up generated files
clean:
	@echo Cleaning up object files and $(TARGET)...
	@del /Q $(OBJECTS)\*.o $(TARGET) 2> NUL || exit 0

.PHONY: all clean