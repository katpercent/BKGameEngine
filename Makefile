# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -DCL_TARGET_OPENCL_VERSION=100 -g

# Directories
SRC_DIR := ./src
INC_DIR := ./include
BUILD_DIR := ./build

CORE_DIR := $(SRC_DIR)/core
UTILS_DIR := $(SRC_DIR)/utils
MATH_DIR := $(SRC_DIR)/math
RENDER_DIR := $(SRC_DIR)/render

# Sources & Objects
CORE_SOURCES := $(wildcard $(CORE_DIR)/*.cpp)
UTILS_SOURCES := $(wildcard $(UTILS_DIR)/*.cpp)
MATH_SOURCES := $(wildcard $(MATH_DIR)/*.cpp)
RENDER_SOURCES := $(wildcard $(RENDER_DIR)/*.cpp)
MAIN_SOURCE := $(SRC_DIR)/main.cpp

SOURCES := $(CORE_SOURCES) $(UTILS_SOURCES) $(MATH_SOURCES) $(RENDER_SOURCES) $(MAIN_SOURCE)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Executable
TARGET := $(BUILD_DIR)/main

# Libraries (pkg-config for SDL2 + SDL2_ttf)
LIBS := $(shell pkg-config --libs sdl2 SDL2_ttf) -lOpenCL -lm
INCLUDES := $(shell pkg-config --cflags sdl2 SDL2_ttf) -I$(SRC_DIR) -I$(INC_DIR)

# Default rule
all: $(TARGET)

# Link final binary
$(TARGET): $(OBJECTS)
	@echo "🔗 Linking: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Compile sources into objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "⚙️ Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)/main $(OBJECTS)

# Show variables (debug makefile)
print-%:
	@echo '$*=$($*)'

# Phony targets
.PHONY: all clean print-%

