# compile mesh classes
NAME = DPDK_ETH_MAC
ROOT = ../../../COMMON

ifdef DPDK_INC
ifdef DPDK_LIB
BUILD_MODULE = true
endif
endif
endif

ifeq ($(BUILD_MODULE),true)

#CFLAGS_ADD := -I$(RTE_SDK)/$(RTE_TARGET)/include -I$(ROOT)/$(INFRA_INTERFACES_PATH) -DAS_INFRA_NULL=0
CFLAGS_ADD := -I$(DPDK_INC) -I$(INFRA_INTERFACES_PATH) -DAS_INFRA_NULL=0
#LIB_SO     =-L$(RTE_SDK)/$(RTE_TARGET)/lib/ -ldpdk -L/usr/lib/x86_64-linux-gnu/ -fPIC
LIB_SO     =-L$(DPDK_LIB) -ldpdk -L/usr/lib/x86_64-linux-gnu/ -fPIC
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

