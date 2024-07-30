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
SRC_H   := $(SRC_H) $(shell find $(ROOT)/Sys_API -type f -name "*.h")
SRC_INC := $(SRC_INC) $(shell find $(ROOT)/Sys_API -type f -name "*.inc")

#By default DU is compiled with ICC and COMMON and RU with GCC
DU_MARK := $(shell pwd | grep /DU/)
RU_MARK := $(shell pwd | grep /RU/)
ifeq ($(DU_MARK),)
#ifeq ($(COMP),)
COMP ?= GCC
#endif
ifneq ($(RU_MARK),)
#RU component
SRC_H   := $(SRC_H) $(shell find $(ROOT)/../RU/Sys_API -type f -name "*.h")
SRC_INC   := $(SRC_INC) $(shell find $(ROOT)/../RU/Sys_API -type f -name "*.inc")
endif
else
#DU component
#ifeq ($(COMP),)
COMP ?= ICC
#endif
SRC_H   := $(shell find $(ROOT)/../DU/Sys_API -type f -name "*.h") $(SRC_H)
SRC_INC   := $(shell find $(ROOT)/../DU/Sys_API -type f -name "*.inc") $(SRC_INC)
endif

ifeq (ARM, $(architecture))

ifeq ($(PLATFORM_NAME), ru_nxp)

export ARM_SYSROOT=$(SW_PACKAGER_SYS_ROOT_PATH)

export PYTHON_VERSION = 37
export BOOST_INC=$(ARM_SYSROOT)/usr/include/
export BOOST_LIB=$(ARM_SYSROOT)/usr/lib
export SYSROOT_LIB=$(ARM_SYSROOT)/lib
#export CPATH=$(PATH)
CC=$(CROSS_COMPILE)gcc 
CPP=$(CROSS_COMPILE)g++ 
LIB_SO += --sysroot=$(ARM_SYSROOT)
ROOT_ARM:=$(OUT_TARGET_DIR)

CFLAGS_ADD+=-I$(INFRA_LIB_ROOT)/interfaces \
	 	-I$(ARM_SYSROOT)/usr/include/ \
	 	-I$(ARM_SYSROOT)/usr/include/python3.7m/
# boost library from Micha: -I/p/Development/Builds/5G/Infra/1_3_3_0/opt/boost/arch/nxp/include/
BIN := $(ROOT_ARM)/lib

else   ################################################################# ! ARM ru_nxp
#@echo "********* Standalone Compile  **********"

ROOT_ARM = /mnt/share/PHY5G/Prototype/NR_5G_SIM

export CPATH=/opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/include/c++/5.5.0/bits:/opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/include/c++/5.5.0:/opt/intel/compilers_and_libraries_2018.1.163/linux/ipp/include:/opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/libc/usr/include:/usr/arm/lib
INC_FLAGS=-I/opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/libc/usr/include -I/usr/local/include

CC  = /opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
CPP = /opt/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
BIN := $(ROOT_ARM)/bin
endif


#ARCH_DEF   = -DARCH=ARM
ARCH_DEF   = -DARCH_ARM
#CFLAGS_ADD = -std=c++11 --disable-multilib
CFLAGS_ADD := $(CFLAGS_ADD) -std=c++11
#CFLAGS_ADD = 

else ###########################################################   INTEL Architecture
#CFLAGS_CPP := -std=c++11
#Select gcc or icc compilers
ifeq (ICC, $(COMP))

CC := icc
CPP := icc
AR :=ar
else # GCC or G++ compilers

ifeq (GCC, $(COMP))
CC  := gcc
CPP := g++
else
$(error Wrong compiler is selected. Options are GCC or ICC)
endif

endif #end of ifeq (ICC)
 
#ARCH_DEF = -DARCH=X86
ARCH_DEF = -DARCH_X86
INC_FLAGS = -msse4.1
CFLAGS_ADD := $(CFLAGS_ADD)  -DINTEL
CFLAGS_ADD := $(CFLAGS_ADD) -std=c++11
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
# Set antena mode: options:_1x1_ _2x2_ _4x4_ _8x32_ 
ant_mode:= _8x32_
CFLAGS_ADD := $(CFLAGS_ADD)  -D$(ant_mode)

#Set RU offline or RT modes
rt_mode:= _RU_OFFLINE_
#other option is _RU_OFFLINE_
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
