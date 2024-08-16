 
# compile mesh classes
NAME = py5Gstub
ROOT = ../../FRAMEWORK
BOOST_INC := /usr//boost
BOOST_LIB = /usr/lib/x86_64-linux-gnu/
PYTHON_INC := /usr/include/python3.12


LIB_SO =-L$(BIN) -lrtdebug
LIB_SO += -L$(BOOST_LIB) -lboost_thread -lboost_python312 -lpython3.12

OPT_H =-I$(PYTHON_INC) -I$(BOOST_INC) 

include $(ROOT)/Make/build.mk

