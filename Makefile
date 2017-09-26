.PHONY: build submit.cpp launch

CXX             ?= c++

TARGET_CPP      := $(patsubst %.submit.cpp,%.cpp,$(FILE))
SUBMIT_CPP      := $(TARGET_CPP:.cpp=.submit.cpp)
MAKEFILE_PATH   := $(realpath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR        := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
BIN_DIR         ?= $(ROOT_DIR)/.bin
INCLUDE_DIR     := $(ROOT_DIR)/.include
PROJECT_DIR     := $(realpath $(dir $(TARGET_CPP)))
PROJECT_NAME    := $(basename $(notdir $(TARGET_CPP)))

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

COMPILE = $(CXX) -include $(TEMPLATE_HPP) $(1) -o $(PREFIX)/$(2) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

RUN_CXX_PREPROCESSOR = $(CXX) $(TARGET_CPP) $(CXXSTANDARD) -I.include -E -C -P
REMOVE_EXTRA_EMPTY_LINES = cat -s
MAKE_SUBMIT_CPP = { cat $(TEMPLATE_HPP); $(RUN_CXX_PREPROCESSOR); } | $(REMOVE_EXTRA_EMPTY_LINES)

$(PREFIX):
	@-mkdir -p $@

$(PREFIX)/$(PROJECT_NAME): $(PREFIX) $(TARGET_CPP)
	@-echo Compiling "$(TARGET_CPP)"...
	@-rm $(SUBMIT_CPP) 2>/dev/null || true
	@$(CXX) -include $(TEMPLATE_HPP) $(TARGET_CPP) -o $(PREFIX)/$(PROJECT_NAME) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

$(SUBMIT_CPP): $(TARGET_CPP) $(PREFIX)/$(PROJECT_NAME)
	@$(MAKE_SUBMIT_CPP) > $(SUBMIT_CPP)

build: $(PREFIX)/$(PROJECT_NAME) $(SUBMIT_CPP)
	@echo >/dev/null

submit.cpp: $(SUBMIT_CPP)
	@echo >/dev/null

launch: build
	@clear
	@cd $(PREFIX) && time $(PREFIX)/$(PROJECT_NAME) $(LANCH_ARGS)

