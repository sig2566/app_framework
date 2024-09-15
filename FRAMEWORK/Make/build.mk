#*****************************************************************
#*
#* File: build.mk 
#*
#*****************************************************************

SHELL	:= /bin/bash
OSTYPE	:= $(shell uname)
#*********************************************************
# Check Mandatory External variables definitions    
#**************************************************************LIB_SO***
ifndef ROOT
$(error ROOT is not defined. Aborting)
endif

ifndef NAME
$(error NAME is not defined. Aborting)
endif

#ifndef architecture
#$(error Architecture is not defined. Format: make target architecture=ARM/INTEL. Aborting)
#endif
#*****************************************************************


#Configuration of build parameters depending of DU or RU component
#Common headers
SRC_H   := $(SRC_H) $(shell find $(ROOT)/Sys_API -type f \( -name '*.h' -o -name '*.inc' -o -name '*.hpp' \))
SRC_H_MODULE   := $(shell find . -type f \( -name '*.h' -o -name '*.inc' -o -name '*.hpp' \))

PROFILE_CPP_API :=$(ROOT)/../../rt_debug_lib/rt_debug/rt_debug_cpp/api
PROFILE_DEF_API := $(ROOT)/../../rt_debug_lib/rt_debug/api

HEADERS := $(SRC_H) $(SRC_H_MODULE) 
# Generate a list of unique directories containing header files
INCLUDE_DIRS := $(sort $(dir $(HEADERS)))

# Convert the list of directories into -I include parameters for GCC
INCLUDE_FLAGS := $(foreach dir, $(INCLUDE_DIRS), -I$(dir)) $(OPT_H)
#Add path to profiler header files
INCLUDE_FLAGS := $(INCLUDE_FLAGS) -I$(PROFILE_CPP_API) -I$(PROFILE_DEF_API)
CC  := gcc
CPP := g++
 
#ARCH_DEF = -DARCH=X86
ARCH_DEF = -D ARCH_X86
#INC_FLAGS = -msse4.1

BIN := $(ROOT)/../bin


BUILD_TIME=$(shell date "+DATE:%d/%m/%y_TIME:%H:%M:%S")

VERDEF = -D TIME_DATE=\"$(BUILD_TIME)\" -D MOD_NAME=\"$(NAME)\" -DMOD_MACRO=$(NAME) -DAPI_CONNECT_FUNC=$(NAME)_IGetConnectAPI

ifeq (debug, $(mode))
DEBFLAGS :=-g -O0 -D DEBUG 
endif

#release: CFLAGS=-O3 -g -DDEBUG $(CFLAGS_ADD)
ifeq (release, $(mode))
DEBFLAGS:=-O3 -g -D DEBUG 
endif

ifeq (release_strip, $(mode))
DEBFLAGS:=-O3 
endif

CFLAGS_ADD := $(CFLAGS_ADD)


# All Target
all: CFLAGS:= $(CFLAGS_ADD) $(DEBFLAGS) 
all: lib


SRC_C   := $(shell find . -type f -name "*.c")
SRC_CPP := $(shell find . -type f -name "*.cc")

OBJ := $(obj)


OBJECTS_CPP := $(notdir $(SRC_CPP:.cc=.o))
OBJECTS_C := $(notdir $(patsubst %.c,%.o,$(SRC_C)))

H_DIR = $(dir $(SRC_H_MODULE))
H_INC_DIR := $(H_INC_DIR) $(dir $(SRC_H))

LIB_TYPE?=DYNAMIC
ifeq (STATIC, $(LIB_TYPE))
BUILD_TARGET:=
LINK_TOOL:=$(AR) -rc $(BIN)/$(NAME).a
else
LINK_TOOL:= $(CPP) -shared -Wl,--export-dynamic
BUILD_TARGET:=-o $(BIN)/$(NAME).so
endif
LIB_SO += -L$(BIN) -lrtdebug

lib : $(OBJECTS_CPP) $(OBJECTS_C)
	@echo $(CPATH)
	@echo $(OBJECTS_CPP) $(OBJECTS_C)
	@test -d $(BIN) || mkdir -p $(BIN) 
	$(LINK_TOOL) $(DEBFLAGS) $(OBJECTS_CPP) $(OBJECTS_C) $(LIB_SO)  $(BUILD_TARGET)
 
$(OBJECTS_CPP): $(SRC_CPP) $(SRC_H) $(SRC_INC)
	@echo $(SRC_CPP) $(INCLUDE_FLAGS) 
	@echo $(OBJECTS_CPP)
	@test -d $(OBJ) || mkdir -p $(OBJ) 
	$(CPP) $(INCLUDE_FLAGS) $(VERDEF) $(ARCH_DEF) $(CFLAGS)  -fPIC -c $(SRC_CPP)

$(OBJECTS_C): $(SRC_C) $(SRC_H) $(SRC_INC)
	@echo $(SRC_C) $(SRC_H)
	@echo $(OBJECTS_C)
	@test -d $(OBJ) || mkdir -p $(OBJ) 
	$(CC)   -fPIC -c $(SRC_C) $(INCLUDE_FLAGS) $(VERDEF) $(ARCH_DEF) $(CFLAGS)

.PHONY: clean
clean:
	rm *o || true
	rm $(BIN)/$(NAME).so || true
