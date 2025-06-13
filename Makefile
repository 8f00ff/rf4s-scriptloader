#!/usr/bin/env -S docker run --rm -ti -v ${PWD}:/data --workdir=/data docker.io/devitllc/mingw-w64:latest /usr/bin/env make -f

NAME        := SDL2
TARGET_DIR  := out
TARGET_DLL  := $(TARGET_DIR)/$(NAME).dll
SRC         := src/dllmain.cpp
INSTALL_DIR := $(HOME)/.local/share/Steam/steamapps/common/Rune Factory 4 Special

CXX         := x86_64-w64-mingw32-g++
CXXFLAGS    := -Wall -Wextra -O2 -static -mwindows
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

gendef: $(NAME).def

build: $(TARGET_DLL)

check-real-dll:
	@if [ -f "$(DLL_REAL)" ]; then \
	  echo "$(NAME) already renamed."; \
	elif [ -f "$(DLL_ORIG)" ]; then \
	  CUR_HASH=`md5sum "$(DLL_ORIG)" | cut -d' ' -f1`; \
	  if [ "$$CUR_HASH" = "$(DLL_HASH)" ]; then \
	    echo "Renaming original $(NAME).dll -> $(NAME)_real.dll"; \
	    mv "$(DLL_ORIG)" "$(DLL_REAL)"; \
	  else \
	    echo "$(NAME).dll hash mismatch, skipping rename."; \
	  fi \
	else \
	  echo "$(NAME).dll not found."; \
	fi

$(TARGET_DIR)/:
	$(MKDIR) $@

$(TARGET_DLL): $(SRC) $(TARGET_DIR)/$(NAME).def | $(TARGET_DIR)/
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -Wl,--output-def,$(TARGET_DIR)/$(NAME).def -o $@ $^

install: $(TARGET_DLL) | check-real-dll
	$(CP) $< "$(DLL_ORIG)"

clean:
	$(RM) -r $(TARGET_DIR)

$(TARGET_DIR)/$(NAME).def: $(TARGET_DIR)/
	@printf "EXPORTS\n" > $@
	@$(WGET) -qO - https://raw.githubusercontent.com/libsdl-org/SDL/refs/tags/release-$(DLL_VERSION)/src/dynapi/SDL_dynapi_procs.h | \
		grep -oP 'SDL_DYNAPI_PROC\([^,]+,\s*\K[^,]+' | \
		while read fn; do \
			printf "    %s = SDL2_real.%s\n" "$$fn" "$$fn"; \
		done >> $@
