 
# compile mesh classes
NAME = THREAD_MGR
ROOT = ../../../FRAMEWORK

BUILD_MODULE = false
#Add support for DPDK library

ifdef DPDK_INC
ifdef DPDK_LIB
BUILD_MODULE = true
endif
endif

ifeq ($(BUILD_MODULE),true)
CFLAGS_ADD := -I$(DPDK_INC)
LIB_SO     =-L$(DPDK_LIB) -lrte_mbuf -lrte_eal -lnuma
include $(ROOT)/Make/build.mk

else
#Avoid build

all: null
ru: null
du: null
clean: null
null:
	@echo $(NAME) built is aborted  
endif
