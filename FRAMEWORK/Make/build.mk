#*****************************************************************
#*
#* File: build.mk 
#*
#*****************************************************************

SHELL	:= /bin/bash
OSTYPE	:= $(shell uname)
#*********************************************************
# Check Mandatory External variables definitions    
#*****************************************************************
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
SRC_H   := $(SRC_H) $(shell find $(ROOT)/Sys_API -type f -name "*.h")
SRC_INC := $(SRC_INC) $(shell find $(ROOT)/Sys_API -type f -name "*.inc")


CC  := gcc
CPP := g++
 
#ARCH_DEF = -DARCH=X86
ARCH_DEF = -DARCH_X86
#INC_FLAGS = -msse4.1

BIN := $(ROOT)/../bin



BUILD_TIME=$(shell date "+DATE:%d/%m/%y_TIME:%H:%M:%S")


VERDEF = -DTIME_DATE=\"$(BUILD_TIME)\" -DMOD_NAME=\"$(NAME)\" 

ifeq (debug, $(mode))
DEBFLAGS :=-g -O0 -DDEBUG 
endif

#release: CFLAGS=-O3 -g -DDEBUG $(CFLAGS_ADD)
ifeq (release, $(mode))
DEBFLAGS:=-O3 -g -DDEBUG 
endif

ifeq (release_strip, $(mode))
DEBFLAGS:=-O3 
endif

CFLAGS_ADD := $(CFLAGS_ADD)  -D$(rt_mode)


# All Target
all: CFLAGS:= $(CFLAGS_ADD) $(DEBFLAGS) -DCOMMON_PHY
all: lib


SRC_C   := $(shell find . -type f -name "*.c")
SRC_CPP := $(shell find . -type f -name "*.cc")
SRC_H_MODULE   := $(shell find . -type f -name "*.h")

SRC_H := $(SRC_H) $(SRC_H_MODULE)
OBJ := $(obj)


OBJECTS_CPP := $(notdir $(SRC_CPP:.cc=.o))
OBJECTS_C := $(notdir $(patsubst %.c,%.o,$(SRC_C)))

H_DIR = $(dir $(SRC_H_MODULE))
#H_INCLUDE = $(INC_FLAGS) $(patsubst %, -I%, $(H_DIR)) -I$(SYS_API)  -I$(MOD_API) $(OPT_H)
H_INC_DIR := $(H_INC_DIR) $(dir $(SRC_H))
H_INCLUDE = $(OPT_H) $(INC_FLAGS) $(patsubst %, -I%, $(H_INC_DIR))
LIB_TYPE?=DYNAMIC
ifeq (STATIC, $(LIB_TYPE))
BUILD_TARGET:=
LINK_TOOL:=$(AR) -rc $(BIN)/$(NAME).a
else
LINK_TOOL:= $(CPP) -shared -Wl,--export-dynamic
BUILD_TARGET:=-o $(BIN)/$(NAME).so
endif


lib : $(OBJECTS_CPP) $(OBJECTS_C)
	@echo $(CPATH)
	@echo $(OBJECTS_CPP) $(OBJECTS_C)
	@test -d $(BIN) || mkdir -p $(BIN) 
	$(LINK_TOOL) $(OBJECTS_CPP) $(OBJECTS_C) $(LIB_SO)  $(BUILD_TARGET)
 
$(OBJECTS_CPP): $(SRC_CPP) $(SRC_H) $(SRC_INC)
	@echo $(SRC_CPP) $(H_INCLUDE)
	@echo $(OBJECTS_CPP)
	@test -d $(OBJ) || mkdir -p $(OBJ) 
	$(CPP) $(H_INCLUDE) $(VERDEF) $(ARCH_DEF) $(CFLAGS)  -fPIC -c $(SRC_CPP)

$(OBJECTS_C): $(SRC_C) $(SRC_H) $(SRC_INC)
	@echo $(SRC_C) $(SRC_H)
	@echo $(OBJECTS_C)
	@test -d $(OBJ) || mkdir -p $(OBJ) 
	$(CC)  $(H_INCLUDE) $(VERDEF) $(ARCH_DEF) $(CFLAGS)  -fPIC -c $(SRC_C)

.PHONY: clean
clean:
	rm *o || true
	rm $(BIN)/$(NAME).so || true
