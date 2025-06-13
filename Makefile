#!/usr/bin/env -S docker run --rm -ti -v ${PWD}:/data --workdir=/data docker.io/devitllc/mingw-w64:latest /usr/bin/env make -f

NAME        := SDL2
TARGET_DIR  := out
TARGET_DLL  := $(TARGET_DIR)/$(NAME).dll
SRC         := src/dllmain.cpp
INSTALL_DIR := $(HOME)/.local/share/Steam/steamapps/common/Rune\ Factory\ 4\ Special

MINHOOK_DIR := extern/minhook
MINHOOK_SRC := \
  $(wildcard $(MINHOOK_DIR)/src/*.c) \
  $(wildcard $(MINHOOK_DIR)/src/hde/*.c)

CXX         := x86_64-w64-mingw32-g++
CXXFLAGS    := -Wall -Wextra -O2 -static -I$(MINHOOK_DIR)/include -mwindows
LDFLAGS     := -shared

CP          := cp -f
GREP        := grep
MKDIR       := mkdir -p
RM          := rm -f
WGET        := wget

DLL_ORIG    := $(INSTALL_DIR)/$(NAME).dll
DLL_REAL    := $(INSTALL_DIR)/$(NAME)_real.dll
DLL_HASH    := 5bb1c75208f6b7c25f4da9190985a2d7
DLL_VERSION := 2.0.16

default: all

.PHONY: all clean install build gendef

all: build

build: $(TARGET_DLL)

clean:
	$(RM) -r $(TARGET_DIR)

gendef: $(NAME).def

install: $(TARGET_DLL) $(DLL_REAL)
	$(CP) $< $(DLL_ORIG)

$(TARGET_DIR):
	$(MKDIR) $@

$(TARGET_DLL): $(SRC) $(MINHOOK_SRC) | $(TARGET_DIR)/$(NAME)_real.def $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(TARGET_DIR)/$(NAME)_real.def $^ -o $@

$(TARGET_DIR)/$(NAME)_real.def: $(TARGET_DIR)
	@printf "EXPORTS\n" > $@
	@$(WGET) -qO - https://raw.githubusercontent.com/libsdl-org/SDL/refs/tags/release-$(DLL_VERSION)/src/dynapi/SDL_dynapi_procs.h | \
		grep -oP 'SDL_DYNAPI_PROC\([^,]+,\s*\K[^,]+' | \
		while read fn; do \
			printf "    %s = SDL2_real.%s\n" "$$fn" "$$fn"; \
		done >> $@

$(DLL_REAL):
	@CUR_HASH=`md5sum $(DLL_ORIG) | cut -d' ' -f1`; \
	if [ "$$CUR_HASH" = "$(DLL_HASH)" ]; then \
	  echo "Renaming $(DLL_ORIG) -> $@"; \
	  mv $(DLL_ORIG) "$@"; \
	else \
	  echo "Hash mismatch: refusing to rename $(DLL_ORIG)"; \
	  exit 1; \
	fi
