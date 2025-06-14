#!/usr/bin/env -S docker run --rm -ti -v ${PWD}:/data --workdir=/data docker.io/devitllc/mingw-w64:latest /usr/bin/env make -f

NAME        := SDL2
TARGET_DIR  := out
TARGET_DLL  := $(TARGET_DIR)/$(NAME).dll
INSTALL_DIR := $(HOME)/.local/share/Steam/steamapps/common/Rune\ Factory\ 4\ Special

# Source files
CORE_SRC := \
  src/core/logger.cpp \
  src/core/hook_manager.cpp \
  src/core/config_manager.cpp \
  src/core/scriptloader.cpp

API_SRC := \
  src/api/rf4s_api.cpp

MOD_SRC := \
  src/mods/everyone_is_bi/bi_mod.cpp

MAIN_SRC := src/dllmain.cpp

ALL_SRC := $(MAIN_SRC) $(CORE_SRC) $(API_SRC) $(MOD_SRC)

# External dependencies
MINHOOK_DIR := extern/minhook
MINHOOK_SRC := \
  $(wildcard $(MINHOOK_DIR)/src/*.c) \
  $(wildcard $(MINHOOK_DIR)/src/hde/*.c)

TOML_DIR    := extern/tomlplusplus

# Compiler settings
CXX         := x86_64-w64-mingw32-g++
CXXFLAGS    := -Wall -Wextra -O2 -static -std=c++17 \
               -I$(MINHOOK_DIR)/include \
               -I$(TOML_DIR)/include \
               -Isrc \
               -mwindows
LDFLAGS     := -shared

# Tools
CP          := cp -f
GREP        := grep
MKDIR       := mkdir -p
RM          := rm -f
WGET        := wget

# SDL2 DLL management
DLL_ORIG    := $(INSTALL_DIR)/$(NAME).dll
DLL_REAL    := $(INSTALL_DIR)/$(NAME)_real.dll
DLL_HASH    := 5bb1c75208f6b7c25f4da9190985a2d7
DLL_VERSION := 2.0.16

# Object files
OBJ_DIR     := $(TARGET_DIR)/obj
CORE_OBJ    := $(CORE_SRC:src/%.cpp=$(OBJ_DIR)/%.o)
API_OBJ     := $(API_SRC:src/%.cpp=$(OBJ_DIR)/%.o)
MOD_OBJ     := $(MOD_SRC:src/%.cpp=$(OBJ_DIR)/%.o)
MAIN_OBJ    := $(MAIN_SRC:src/%.cpp=$(OBJ_DIR)/%.o)
MINHOOK_OBJ := $(MINHOOK_SRC:$(MINHOOK_DIR)/%.c=$(OBJ_DIR)/minhook/%.o)

ALL_OBJ     := $(MAIN_OBJ) $(CORE_OBJ) $(API_OBJ) $(MOD_OBJ) $(MINHOOK_OBJ)

default: all

.PHONY: all clean install build gendef debug

all: build

build: $(TARGET_DLL)

clean:
	$(RM) -r $(TARGET_DIR)

gendef: $(NAME).def

install: $(TARGET_DLL) $(DLL_REAL)
	$(CP) $< $(DLL_ORIG)

debug:
	@echo "Source files:"
	@echo "  MAIN_SRC: $(MAIN_SRC)"
	@echo "  CORE_SRC: $(CORE_SRC)"
	@echo "  API_SRC: $(API_SRC)"
	@echo "  MOD_SRC: $(MOD_SRC)"
	@echo "  MINHOOK_SRC: $(MINHOOK_SRC)"
	@echo ""
	@echo "Object files:"
	@echo "  ALL_OBJ: $(ALL_OBJ)"

# Create directories
$(TARGET_DIR):
	$(MKDIR) $@

$(OBJ_DIR):
	$(MKDIR) $@

$(OBJ_DIR)/core:
	$(MKDIR) $@

$(OBJ_DIR)/api:
	$(MKDIR) $@

$(OBJ_DIR)/mods/everyone_is_bi:
	$(MKDIR) $@

$(OBJ_DIR)/minhook/src:
	$(MKDIR) $@

$(OBJ_DIR)/minhook/src/hde:
	$(MKDIR) $@

# Compile C++ source files
$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR) $(OBJ_DIR)/core $(OBJ_DIR)/api $(OBJ_DIR)/mods/everyone_is_bi
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile MinHook C source files
$(OBJ_DIR)/minhook/%.o: $(MINHOOK_DIR)/%.c | $(OBJ_DIR)/minhook/src $(OBJ_DIR)/minhook/src/hde
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the final DLL
$(TARGET_DLL): $(ALL_OBJ) $(TARGET_DIR)/$(NAME)_real.def | $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(TARGET_DIR)/$(NAME)_real.def $(ALL_OBJ) -o $@

# Generate SDL2 export definitions
$(TARGET_DIR)/$(NAME)_real.def: $(TARGET_DIR)
	@printf "EXPORTS\n" > $@
	@$(WGET) -qO - https://raw.githubusercontent.com/libsdl-org/SDL/refs/tags/release-$(DLL_VERSION)/src/dynapi/SDL_dynapi_procs.h | \
		grep -oP 'SDL_DYNAPI_PROC\([^,]+,\s*\K[^,]+' | \
		while read fn; do \
			printf "    %s = SDL2_real.%s\n" "$$fn" "$$fn"; \
		done >> $@

# Rename original SDL2.dll to SDL2_real.dll
$(DLL_REAL):
	@CUR_HASH=`md5sum $(DLL_ORIG) | cut -d' ' -f1`; \
	if [ "$$CUR_HASH" = "$(DLL_HASH)" ]; then \
	  echo "Renaming $(DLL_ORIG) -> $@"; \
	  mv $(DLL_ORIG) "$@"; \
	else \
	  echo "Hash mismatch: refusing to rename $(DLL_ORIG)"; \
	  exit 1; \
	fi

# Dependencies (simplified - in a real project you'd use automatic dependency generation)
$(OBJ_DIR)/dllmain.o: src/dllmain.cpp src/core/scriptloader.h src/mods/everyone_is_bi/bi_mod.h
$(OBJ_DIR)/core/logger.o: src/core/logger.cpp src/core/logger.h
$(OBJ_DIR)/core/hook_manager.o: src/core/hook_manager.cpp src/core/hook_manager.h src/core/logger.h
$(OBJ_DIR)/core/config_manager.o: src/core/config_manager.cpp src/core/config_manager.h src/core/logger.h
$(OBJ_DIR)/core/scriptloader.o: src/core/scriptloader.cpp src/core/scriptloader.h src/core/logger.h src/core/hook_manager.h src/core/config_manager.h
$(OBJ_DIR)/api/rf4s_api.o: src/api/rf4s_api.cpp src/api/rf4s_api.h src/api/rf4s_types.h src/core/scriptloader.h
$(OBJ_DIR)/mods/everyone_is_bi/bi_mod.o: src/mods/everyone_is_bi/bi_mod.cpp src/mods/everyone_is_bi/bi_mod.h src/api/rf4s_api.h
