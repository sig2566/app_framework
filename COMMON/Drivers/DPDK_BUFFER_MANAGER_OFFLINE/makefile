 
# compile mesh classes
NAME = DPDK_BUFFER_MANAGER_OFFLINE
ROOT = ../../../COMMON


ifdef DPDK_INC
ifdef DPDK_LIB
BUILD_MODULE = true
endif
endif

ifeq ($(BUILD_MODULE),true)
CFLAGS_ADD := -I$(DPDK_INC)
#LIB_SO     =-L$(DPDK_LIB) -lrte_mbuf
#LIB_SO     =-L$(DPDK_LIB) -lrte_mbuf -lrte_mempool -lrte_eal -lnuma -lrte_ethdev
LIB_SO     =-L$(DPDK_LIB) -ldpdk
#LIB_SO     =-L$(DPDK_LIB) -lrte_mbuf -lrte_mempool  -lrte_eal -lnuma -lrte_ethdev -Wl,--whole-archive -ldpdk -Wl,--no-whole-archive
#LIB_SO     =-L$(DPDK_LIB) -lrte_mbuf -lrte_mempool -lrte_eal -lnuma -lrte_ethdev -Wl,--whole-archive -ldpdk -Wl,--no-whole-archive

include $(ROOT)/Make/build.mk

else
#Avoid build

all: null
ru: null
du: null
clean: null
null:
	@echo DPDK_INC or/and DPDK_LIB are not defined. $(NAME) cannot link with DPDK SDK
	@echo $(NAME) built is aborted  
endif
