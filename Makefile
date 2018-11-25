.PHONY: build submit.cpp launch

TARGET_CPP      := $(FILE:.submit.cpp=.cpp)
SUBMIT_CPP      := $(TARGET_CPP:.cpp=.submit.cpp)

ifeq ($(TARGET_CPP), $(SUBMIT_CPP))
$(error invalid target $(FILE))
endif

MAKEFILE_PATH   := $(realpath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR        := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
BIN_DIR         := $(ROOT_DIR)/.bin
SRC_DIR         := $(ROOT_DIR)/.src
INCLUDE_DIR     := $(ROOT_DIR)/.include
PROBLEM_DIR     := $(realpath $(dir $(TARGET_CPP)))
PROBLEM_NAME    := $(basename $(notdir $(TARGET_CPP)))
INPUT_FILE      := $(PROBLEM_DIR)/$(PROBLEM_NAME).in
DO_NOTHING      := @echo > /dev/null
EMPTY_CPP       := -x c++ /dev/null

CXX             ?= c++
DEBUG           ?= 0

ifeq ($(DEBUG), 1)
	OPTIMIZATION_LEVEL = -g -O0
	DEFINITIONS += -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls
else
	OPTIMIZATION_LEVEL = -Ofast
	DEFINITIONS += -DNDEBUG
endif

# If input file exists redirect input to it.
ifneq ($(wildcard $(INPUT_FILE)),) 
	LANCH_ARGS = < $(INPUT_FILE)
endif 

ifneq ($(patsubst .%,.,$(notdir $(realpath $(dir $(PROBLEM_DIR))))),.)
	FOR_SUBMIT ?= 1
endif

ifneq ($(FOR_SUBMIT),1)
$(shell touch $(TARGET_CPP))
endif

INCLUDES = -I$(INCLUDE_DIR)
TEMPLATE_HPP = $(INCLUDE_DIR)/cp/template.hpp

CXXSTANDARD = -std=c++14
CXXFLAGS += -march=native $(CXXSTANDARD) -Wall -fno-fast-math $(OPTIMIZATION_LEVEL) $(DEFINITIONS) $(INCLUDES)

SOURCE_FILES = $(addprefix -include ,$(wildcard $(SRC_DIR)/cp/*.cpp)) $(EMPTY_CPP)

ifeq ($(FOR_SUBMIT),1)
	RUN_CXX_PREPROCESSOR = $(CXX) $(TARGET_CPP) $(CXXSTANDARD) -DONLINE_JUDGE -I.include -E -C -P
	REMOVE_EXTRA_EMPTY_LINES = cat -s
	MAKE_SUBMIT_CPP = { cat $(TEMPLATE_HPP); $(RUN_CXX_PREPROCESSOR); } | $(REMOVE_EXTRA_EMPTY_LINES)
	MAKE_SUBMIT_CPP += > $(SUBMIT_CPP)
else
	MAKE_SUBMIT_CPP = $(DO_NOTHING)
endif

$(BIN_DIR):
	@-mkdir -p $@

$(BIN_DIR)/$(PROBLEM_NAME): $(BIN_DIR) $(TARGET_CPP)
	@clear
	@-echo Compiling "$(TARGET_CPP)"...
	@-rm $(SUBMIT_CPP) 2> /dev/null || true
	@$(CXX) -include $(TEMPLATE_HPP) -include $(TARGET_CPP) $(SOURCE_FILES) -o $@ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

$(SUBMIT_CPP): $(TARGET_CPP) $(BIN_DIR)/$(PROBLEM_NAME)
	@$(MAKE_SUBMIT_CPP)

build: $(BIN_DIR)/$(PROBLEM_NAME) $(SUBMIT_CPP)
	@$(DO_NOTHING)

submit.cpp: $(SUBMIT_CPP)
	@$(DO_NOTHING)

launch: build
	@clear
	@cd $(BIN_DIR) && time $(BIN_DIR)/$(PROBLEM_NAME) $(LANCH_ARGS)
