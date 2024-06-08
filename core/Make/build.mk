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

ifndef architecture
$(error Architecture is not defined. Format: make target architecture=ARM/INTEL. Aborting)
endif
#*****************************************************************

#Configuration of build parameters depending of DU or RU component
#Common headers
# Define the variable to hold the list of all include files
INCLUDE_FILES := $(shell find $(ROOT)/../common_src/Sys_API -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.inc' \))
INCLUDE_FILES := $(INCLUDE_FILES) $(shell find . -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.inc' \))

# Extract unique directories from the list of include files
INCLUDE_DIRS := $(shell echo $(INCLUDE_FILES) | xargs -n 1 dirname | sort -u)

# Convert the directories into -I<directory> format
INCLUDE_FLAGS := $(foreach dir,$(INCLUDE_DIRS),-I$(dir))

SRC_H   := $(SRC_H) $(INCLUDE_FLAGS)

ifeq (ARM, $(architecture))


#@echo "********* Standalone Compile  **********"

ROOT_ARM = /mnt/share/PHY5G/Prototype/NR_5G_SIM

export CPATH=/opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/include/c++/5.5.0/bits:/opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/include/c++/5.5.0:/opt/intel/compilers_and_libraries_2018.1.163/linux/ipp/include:/opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/libc/usr/include:/usr/arm/lib
INC_FLAGS=-I/opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/libc/usr/include -I/usr/local/include

CC  = /opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
CPP = /opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
BIN := $(ROOT_ARM)/bin


#ARCH_DEF   = -DARCH=ARM
ARCH_DEF   = -DARCH_ARM
#CFLAGS_ADD = -std=c++11 --disable-multilib
CFLAGS_ADD := $(CFLAGS_ADD)
#CFLAGS_ADD = 

else ###########################################################   INTEL Architecture

CC  := gcc
CPP := g++
 
#ARCH_DEF = -DARCH=X86
ARCH_DEF = -DARCH_X86
#INC_FLAGS = -msse4.1
CFLAGS_ADD := $(CFLAGS_ADD)  -DINTEL
CFLAGS_ADD := $(CFLAGS_ADD)
BIN := $(ROOT)/../bin

endif # end of IF-ELSE (ARM/INTEL Architecture)


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

# All Target
all: CFLAGS:= $(CFLAGS_ADD) $(DEBFLAGS)
all: lib



SRC_C   := $(shell find . -type f -name "*.c")
SRC_CPP := $(shell find . -type f -name "*.cc")

OBJ := $(obj)


OBJECTS_CPP := $(notdir $(SRC_CPP:.cc=.o))
OBJECTS_C := $(notdir $(patsubst %.c,%.o,$(SRC_C)))


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
 
$(OBJECTS_CPP): $(SRC_CPP) $(INCLUDE_FILES) 
	@echo $(SRC_CPP) $(H_INCLUDE)
	@echo $(OBJECTS_CPP)
	@test -d $(OBJ) || mkdir -p $(OBJ) 
	$(CPP) $(SRC_H) $(VERDEF) $(ARCH_DEF) $(CFLAGS)  -fPIC -c $(SRC_CPP)

$(OBJECTS_C): $(SRC_C) $(INCLUDE_FILES)
	@echo $(SRC_C) $(SRC_H)
	@echo $(OBJECTS_C)
	@test -d $(OBJ) || mkdir -p $(OBJ) 
	$(CC)  $(SRC_H) $(VERDEF) $(ARCH_DEF) $(CFLAGS)  -fPIC -c $(SRC_C)

.PHONY: clean
clean:
	rm *o || true
	rm $(BIN)/$(NAME).so || true
	
test:
	@echo $(CC)  $(SRC_H) $(VERDEF) $(ARCH_DEF) $(CFLAGS)  -fPIC -c $(SRC_CPP)
