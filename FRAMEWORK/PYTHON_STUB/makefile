 
# compile mesh classes
NAME = py5Gstub
ROOT = ../../FRAMEWORK
BOOST_INC := /usr/include/boost/
BOOST_LIB = /usr/lib/x86_64-linux-gnu/
PYTHON_INC := /usr/include/python3.12


#LIB_SO =-L$(BIN) -lrtdebug
LIB_SO += -L$(BOOST_LIB) -lboost_python312 -lboost_thread  -lpython3.12 
#-lboost_python312
OPT_H =-I$(PYTHON_INC) -I$(BOOST_INC) 

include $(ROOT)/Make/build.mk

