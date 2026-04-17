# =============================================================================
# Universal TUI Game Engine - Cross-Platform Makefile
# =============================================================================
# 
# This Makefile builds the engine and demo game on:
#   - Linux (gcc)
#   - macOS (clang/gcc)
#   - Windows (via MinGW-w64 or MSYS2)
#
# Usage:
#   make              - Build everything
#   make engine_demo  - Build just the engine demo
#   make potion_game  - Build the Potion Master demo game
#   make clean        - Remove all build artifacts
#   make run          - Build and run the Potion Master game
# =============================================================================

# Platform detection
UNAME := $(shell uname -s)

# Compiler selection
CC = gcc
CXX = g++

# C standard (use C11 for best compatibility)
CFLAGS = -std=c11 -Wall -Wextra -pedantic
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

# Include paths
INCLUDES = -IEngine -IEngine/utils -IGames/PotionGame

# Library flags
LDFLAGS = -lm

# Platform-specific flags
ifeq ($(UNAME), Linux)
    CFLAGS += -DPLATFORM_LINUX
    CXXFLAGS += -DPLATFORM_LINUX
endif

ifeq ($(UNAME), Darwin)
    CFLAGS += -DPLATFORM_MACOS
    CXXFLAGS += -DPLATFORM_MACOS
endif

# Windows detection (via MinGW/MSYS2)
ifneq ($(findstring MINGW,$(shell uname)),)
    CFLAGS += -DPLATFORM_WINDOWS
    CXXFLAGS += -DPLATFORM_WINDOWS
endif

ifneq ($(findstring MSYS,$(shell uname)),)
    CFLAGS += -DPLATFORM_WINDOWS
    CXXFLAGS += -DPLATFORM_WINDOWS
endif

# Source files for the engine
ENGINE_SRC = \
    Engine/engine.cpp \
    Engine/render.cpp \
    Engine/ui.cpp \
    Engine/component.cpp \
    Engine/entity.cpp \
    Engine/audio.cpp \
    Engine/utils/mathutils.cpp \
    Engine/utils/stringutils.cpp \
    Engine/utils/arrayutils.cpp

# Source files for the Potion Master demo game
GAME_SRC = \
    Games/PotionGame/GameMain.cpp

# Object file directories
OBJ_DIR = build/obj
BIN_DIR = build/bin

# Object files
ENGINE_OBJ = $(patsubst Engine/%.cpp,$(OBJ_DIR)/%.o,$(ENGINE_SRC))
GAME_OBJ = $(patsubst Games/%.cpp,$(OBJ_DIR)/game_%.o,$(GAME_SRC))

# Output binaries
ENGINE_DEMO = $(BIN_DIR)/engine_demo
POTION_GAME = $(BIN_DIR)/potion_game

# Default target: build everything
all: potion_game

# =============================================================================
# ENGINE DEMO (just the engine, no game)
# =============================================================================
engine_demo: $(ENGINE_OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(ENGINE_OBJ) $(LDFLAGS)

# =============================================================================
# POTION MASTER GAME
# =============================================================================
potion_game: $(ENGINE_OBJ) $(GAME_OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(ENGINE_OBJ) $(GAME_OBJ) $(LDFLAGS)

# =============================================================================
# COMPILATION RULES
# =============================================================================

# Create object directories
$(OBJ_DIR)/%.o: Engine/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/game_%.o: Games/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Create directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/Engine/utils
	mkdir -p $(OBJ_DIR)/Games/PotionGame

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# =============================================================================
# CLEANUP
# =============================================================================
clean:
	rm -rf build/
	@echo "Build artifacts removed."

# =============================================================================
# RUN
# =============================================================================
run: potion_game
	./$(POTION_GAME)

# =============================================================================
# INSTALL (optional, for system-wide installation)
# =============================================================================
install: potion_game
	install -d /usr/local/bin
	install $(POTION_GAME) /usr/local/bin/potion-game
	@echo "Installed to /usr/local/bin/potion-game"

uninstall:
	rm -f /usr/local/bin/potion-game
	@echo "Uninstalled potion-game"

# =============================================================================
# INFO
# =============================================================================
info:
	@echo "=== Build Configuration ==="
	@echo "Platform: $(UNAME)"
	@echo "Compiler: $(CC) / $(CXX)"
	@echo "C Flags:  $(CFLAGS)"
	@echo "C++ Flags:$(CXXFLAGS)"
	@echo "Libs:     $(LDFLAGS)"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build everything (Potion Master game)"
	@echo "  make engine_demo - Build just the engine demo"
	@echo "  make potion_game - Build the Potion Master game"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make run      - Build and run the game"

# =============================================================================
# PHONY TARGETS
# =============================================================================
.PHONY: all engine_demo potion_game clean run install uninstall info
