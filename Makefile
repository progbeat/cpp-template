.PHONY: build submit.cpp launch

TARGET_CPP      := $(FILE:.submit.cpp=.cpp)
SUBMIT_CPP      := $(TARGET_CPP:.cpp=.submit.cpp)

ifeq ($(TARGET_CPP), $(SUBMIT_CPP))
$(error invalid target $(FILE))
endif

MAKEFILE_PATH   := $(realpath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR        := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
BIN_DIR         := $(ROOT_DIR)/.bin
INCLUDE_DIR     := $(ROOT_DIR)/.include
PROJECT_DIR     := $(realpath $(dir $(TARGET_CPP)))
PROJECT_NAME    := $(basename $(notdir $(TARGET_CPP)))
CXX             ?= c++
DEBUG           ?= 0

ifeq ($(DEBUG), 1)
	OPTIMIZATION_LEVEL = -g -O0
	DEFINITIONS += -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls
else
	OPTIMIZATION_LEVEL = -Ofast
	DEFINITIONS += -DNDEBUG
endif

INCLUDES = -I$(INCLUDE_DIR)
TEMPLATE_HPP = $(INCLUDE_DIR)/template.hpp

CXXSTANDARD = -std=c++14
CXXFLAGS += -march=native $(CXXSTANDARD) -Wall -fno-fast-math $(OPTIMIZATION_LEVEL) $(DEFINITIONS) $(INCLUDES)

COMPILE = $(CXX) -include $(TEMPLATE_HPP) $(1) -o $(BIN_DIR)/$(2) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

RUN_CXX_PREPROCESSOR = $(CXX) $(TARGET_CPP) $(CXXSTANDARD) -I.include -E -C -P
REMOVE_EXTRA_EMPTY_LINES = cat -s
MAKE_SUBMIT_CPP = { cat $(TEMPLATE_HPP); $(RUN_CXX_PREPROCESSOR); } | $(REMOVE_EXTRA_EMPTY_LINES)

$(BIN_DIR):
	@-mkdir -p $@

$(BIN_DIR)/$(PROJECT_NAME): $(BIN_DIR) $(TARGET_CPP)
	@clear
	@-echo Compiling "$(TARGET_CPP)"...
	@-rm $(SUBMIT_CPP) 2> /dev/null || true
	@$(CXX) -include $(TEMPLATE_HPP) $(TARGET_CPP) -o $@ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

$(SUBMIT_CPP): $(TARGET_CPP) $(BIN_DIR)/$(PROJECT_NAME)
	@$(MAKE_SUBMIT_CPP) > $@

build: $(BIN_DIR)/$(PROJECT_NAME) $(SUBMIT_CPP)
	@echo > /dev/null

submit.cpp: $(SUBMIT_CPP)
	@echo > /dev/null

launch: build
	@clear
	@cd $(BIN_DIR) && time $(BIN_DIR)/$(PROJECT_NAME) $(LANCH_ARGS)

