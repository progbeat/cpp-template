.PHONY: build launch

CXX             ?= c++

MAKEFILE_PATH   := $(realpath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR        := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
BIN_DIR         ?= $(ROOT_DIR)/.bin
INCLUDE_DIR		:= $(ROOT_DIR)/.include
PROJECT_DIR     := $(realpath $(dir $(TARGET)))
PROJECT_NAME    := $(basename $(notdir $(TARGET)))

DEBUG           ?= 0

ifeq ($(DEBUG), 1)
	OPTIMIZATION_LEVEL = -g -O0
	DEFINITIONS += -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls
	CONFIGURATION = Debug
else
	OPTIMIZATION_LEVEL = -Ofast
	DEFINITIONS += -DNDEBUG
	CONFIGURATION = Release
endif

INCLUDES = -I$(INCLUDE_DIR)
TEMPLATE_HPP = $(INCLUDE_DIR)/template.hpp

CXXSTANDARD = -std=c++14
CXXFLAGS += -march=native $(CXXSTANDARD) -Wall -fno-fast-math $(OPTIMIZATION_LEVEL) $(DEFINITIONS) $(INCLUDES)

PREFIX = $(BIN_DIR)/$(CONFIGURATION)

define preprocess
	@$(CXX) $(1) $(CXXSTANDARD) -I.include -E -C -P > $(PROJECT_DIR)/submit.cpp
endef

define compile
	@-echo Compiling...
	@$(CXX) -include $(TEMPLATE_HPP) $(1) -o $(PREFIX)/$(2) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)
	@-clear
endef

define launch
	@cd $(PREFIX) && time $(PREFIX)/$(1) $(2)
endef

$(BIN_DIR):
	@-mkdir -p $@

$(BIN_DIR)/$(CONFIGURATION): $(BIN_DIR)
	@-mkdir -p $@

build: $(BIN_DIR)/$(CONFIGURATION)
	$(call compile,$(TARGET),$(PROJECT_NAME))

launch:
	$(call launch,$(PROJECT_NAME),$(LANCH_ARGS))