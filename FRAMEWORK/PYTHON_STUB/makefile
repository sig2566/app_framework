 
# compile mesh classes
NAME = py5Gstub
ROOT := ../../../COMMON

#RULE file
# location of the Python header files
BIN := ../../../bin

ifeq (ARM, $(architecture))
ifeq ($(PLATFORM_NAME), ru_nxp)
ROOT_ARM:=$(OUT_TARGET_DIR)
BIN := $(ROOT_ARM)/lib
LIB_SO =-L$(BIN) -lrtdebug 


## for Marvel Board
#BOOST_INC = /usr/arm/include
#BOOST_LIB = /usr/arm/lib

## for NXP Board - still not working due to linker error: collect2: error: ld
#BOOST_INC = /opt/boost_1.70_arm/boost/arch/nxp/include
#BOOST_LIB = /opt/boost_1.70_arm/boost/arch/nxp/lib

PYTHON_VERSION = 37
#PYTHON_INCLUDE = /usr/arm/include
#PYTHON_INCLUDE = /usr/include/python$(PYTHON_VERSION)
#PYTHON_LIB = /usr/arm/lib  
export SYSROOT_LIB=$(ARM_SYSROOT)/lib
export PYTHON_LIB=$(ARM_SYSROOT)/usr/lib
LIB_SO += -L$(BOOST_LIB) -L$(PYTHON_LIB)  -L$(SYSROOT_LIB) -lboost_thread -lboost_python$(PYTHON_VERSION) 
endif
else
BOOST_INC = /usr/include
BOOST_LIB = /opt/boost_1_69_0_gcc4.8/boost_1_69_0/stage/lib
PYTHON_VERSION = 3.6
PYTHON_INCLUDE = /opt/anaconda3/include/python3.7m
LIB_SO =-L$(BIN) -lrtdebug 
LIB_SO += -L$(BOOST_LIB) -lboost_thread-mt -lboost_python
endif
OPT_H = -I$(PYTHON_INCLUDE)  -I$(BOOST_INC) 

include $(ROOT)/Make/build.mk

