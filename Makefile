# Project name
PROJECT := objviewer


# Compiler
LIB := -ldl -lGL -lglfw
FLAGS = -Wall -Wextra
CCFLAGS = -std=c11 $(FLAGS)
CXXFLAGS = -std=c++11 $(FLAGS)


# Folder management
MKDIR := mkdir -pv
RM := rm -rfv
CP := cp -ruv


# Directories
SRC := src
BUILD := build
BIN := bin


# Main target
TARGET := $(BIN)/$(PROJECT)

# Targets
.PHONY: release debug clean

release: $(TARGET)

debug: FLAGS += -g3
debug: $(TARGET)

clean:
	$(RM) $(BUILD) $(BIN)


# Directories creation
.PRECIOUS: $(BUILD)/ $(BUILD)%/ $(BIN)/
.SECONDEXPANSION:

$(BUILD)/:
	$(MKDIR) $@

$(BUILD)%/:
	$(MKDIR) $@

$(BIN)/:
	$(MKDIR) $@


# C files
CCSOURCES := $(shell find $(SRC) -type f -name *.c)
CCOBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(CCSOURCES:.c=.o))

# CXX files
CXXSOURCES := $(shell find $(SRC) -type f -name *.cpp)
CCOBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(CXXSOURCES:.cpp=.o))


# Compilation
$(TARGET): $(CCOBJECTS) $(CCOBJECTS) | $$(@D)/
	$(CXX) $^ -o $@ $(LIB)

$(BUILD)/%.o: $(SRC)/%.c | $$(@D)/
	$(CC) $(CCFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(SRC)/%.cpp | $$(@D)/
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD)/main.o: $(SRC)/main.cpp | $$(@D)/
	$(CXX) $(CXXFLAGS) -c -o $@ $<
