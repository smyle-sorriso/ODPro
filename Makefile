# =========================================================
# Windows-only Makefile
# raylib + Dear ImGui + rlImGui + Eigen + RapidJSON
# =========================================================

SHELL := cmd.exe
.SHELLFLAGS := /C

.DEFAULT_GOAL := all

# ---------------------------------------------------------
# Project
# ---------------------------------------------------------

PROJECT_NAME ?= $(notdir $(CURDIR))

SRC_DIR      := src
EXTERNAL_DIR := external
OBJ_DIR      := obj
BUILD_DIR    := build

# Web build variables
EMCC            := em++
BUILD_DIR_WEB   := $(BUILD_DIR)/web
TARGET_WEB      := $(BUILD_DIR_WEB)/index.html
SHELL_WEB       := resources/web/custom_shell.html
RAYLIB_LIB_WEB  := $(RAYLIB_PATH)/src/web/libraylib.a
FLAGS_WEB       := -Os -Wall -std=c++17 -DPLATFORM_WEB -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s FORCE_FILESYSTEM=1

# Paths for compiler/linker (forward slashes are fine here)
TARGET       := $(BUILD_DIR)/$(PROJECT_NAME).exe

# Paths for Windows shell commands
TARGET_WIN   := $(BUILD_DIR)\$(PROJECT_NAME).exe
OBJ_DIR_WIN  := $(OBJ_DIR)
BUILD_DIR_WIN:= $(BUILD_DIR)

# ---------------------------------------------------------
# Toolchain
# ---------------------------------------------------------

RAYLIB_PATH   ?= C:/raylib/raylib
COMPILER_PATH ?= C:/raylib/w64devkit/bin

CXX := $(COMPILER_PATH)/g++.exe

# Force GCC to use binutils from the same toolchain
TOOLCHAIN_HINT := -B$(COMPILER_PATH)/

# ---------------------------------------------------------
# Build mode
# ---------------------------------------------------------

BUILD_MODE ?= RELEASE

STD_FLAGS     := -std=c++17
WARNING_FLAGS := -Wall -Wextra -Wpedantic -Wno-missing-braces -Wno-missing-field-initializers

ifeq ($(BUILD_MODE),DEBUG)
OPT_FLAGS := -O0 -g
else
OPT_FLAGS := -O2
endif

CXXFLAGS := $(STD_FLAGS) $(OPT_FLAGS) $(WARNING_FLAGS)

# ---------------------------------------------------------
# Includes / Defines
# ---------------------------------------------------------

INCLUDE_PATHS := -I.
INCLUDE_PATHS += -I$(SRC_DIR)
INCLUDE_PATHS += -I$(EXTERNAL_DIR)
INCLUDE_PATHS += -I$(EXTERNAL_DIR)/imgui
INCLUDE_PATHS += -I$(EXTERNAL_DIR)/implot
INCLUDE_PATHS += -I$(EXTERNAL_DIR)/rlimgui
INCLUDE_PATHS += -I$(EXTERNAL_DIR)/eigen
INCLUDE_PATHS += -I$(RAYLIB_PATH)/src
INCLUDE_PATHS += -I$(RAYLIB_PATH)/src/external

DEFINES := -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33

# Auto dependency generation for headers
DEPFLAGS := -MMD -MP

# ---------------------------------------------------------
# Link
# ---------------------------------------------------------

LDFLAGS := -L$(RAYLIB_PATH)/src
LDLIBS  := -lraylib -lopengl32 -lgdi32 -lwinmm
RES_FILE := $(RAYLIB_PATH)/src/raylib.rc.data

# ---------------------------------------------------------
# Source discovery
# ---------------------------------------------------------

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

SRC := $(call rwildcard,$(SRC_DIR)/,*.cpp)

# Dear ImGui
SRC += $(EXTERNAL_DIR)/imgui/imgui.cpp
SRC += $(EXTERNAL_DIR)/imgui/imgui_draw.cpp
SRC += $(EXTERNAL_DIR)/imgui/imgui_tables.cpp
SRC += $(EXTERNAL_DIR)/imgui/imgui_widgets.cpp
# Optional:
# SRC += $(EXTERNAL_DIR)/imgui/imgui_demo.cpp

# ImPlot
SRC += $(EXTERNAL_DIR)/implot/implot.cpp
SRC += $(EXTERNAL_DIR)/implot/implot_items.cpp
# Optional:
# SRC += $(EXTERNAL_DIR)/implot/implot_demo.cpp

# rlImGui
SRC += $(EXTERNAL_DIR)/rlimgui/rlImGui.cpp

OBJS := $(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))
DEPS := $(OBJS:.o=.d)

-include $(DEPS)

# ---------------------------------------------------------
# Targets
# ---------------------------------------------------------

.PHONY: all clean rebuild run prebuild web serve

all: prebuild $(TARGET)

# Delete stale exe before a new build attempt.
# If compilation fails after this point, the exe should be gone.
prebuild:
	@if not exist "$(BUILD_DIR_WIN)" mkdir "$(BUILD_DIR_WIN)"
	@if exist "$(TARGET_WIN)" del /q "$(TARGET_WIN)"
	@if exist "$(PROJECT_NAME).exe" del /q "$(PROJECT_NAME).exe"

$(TARGET): $(OBJS)
	$(CXX) $(TOOLCHAIN_HINT) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS) $(RES_FILE)

$(OBJ_DIR)/%.o: %.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(TOOLCHAIN_HINT) -c $< -o $@ $(CXXFLAGS) $(DEPFLAGS) $(DEFINES) $(INCLUDE_PATHS)

run: all
	"$(TARGET_WIN)"

clean:
	@if exist "$(OBJ_DIR_WIN)" rmdir /s /q "$(OBJ_DIR_WIN)"
	@if exist "$(BUILD_DIR_WIN)" rmdir /s /q "$(BUILD_DIR_WIN)"
	@if exist "$(PROJECT_NAME).exe" del /q "$(PROJECT_NAME).exe"
	@echo Cleaning done

rebuild: clean all

web:
	@if not exist "$(BUILD_DIR_WEB)" mkdir "$(BUILD_DIR_WEB)"
	$(EMCC) $(SRC) $(INCLUDE_PATHS) $(RAYLIB_LIB_WEB) $(FLAGS_WEB) --shell-file $(SHELL_WEB) --preload-file resources -o $(TARGET_WEB)

serve:
	cmd /c "start http://localhost:8000 && cd build/web && python -m http.server"
