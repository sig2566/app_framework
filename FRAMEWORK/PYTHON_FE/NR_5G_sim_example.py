#import sys
#sys.path.append('../../../bin')
import PyStub
ver= PyStub.ver()
print(ver)
# init: load all shared libraries
#		init all modules
#		restart al modules 
#		define mem areas and allocation storage
#		connection betwenn famework componnents 
#		init log system 
#		init profilers
PyStub.init("../COMMON/Projects/FRAMEWORK/sys_configure.xml")      
# prints/get list of modules names
PyStub.modulenames()
# prints/get list of mem areas names
PyStub.areanames()
# call module stand alone 
PyStub.callmod("SAMPLE_MODULE")
# prints/get profiling results
PyStub.stop()
PyStub.prof()

# prints/get logs out
PyStub.logs()


areas= PyStub.areanames()
print (areas)
module_names =  PyStub.modulenames()
print(module_names)

#/////////////////////////////
#// testing memories access
#/////////////////////////////

# in_mem -> TEST_MEM_AREA_1 buf
in_mem = PyStub.getmem('TEST_MEM_AREA_1')

out_r = [0,1,2,3,4,5,6,7,8]
print(out_r)
# wr buf out_r[] into class 'in_mem' fifo according wr_ptr
in_mem.pushu32(out_r)
in_r = in_mem.popu32()
print(in_r)

out_f = [1.1,2.1, 3.4, 4.5, 7.8, 102.12]
print(out_f)
in_mem.pushfloat(out_f)
in_f = in_mem.popfloat()
print(in_f)

##Simple unit test


out_r = [9,8,4,0,2,100,200]
print(out_r)
in_mem.pushu32(out_r)
PyStub.callmod("SAMPLE_MODULE")		# call SAMPLE_MODULE.ICall
PyStub.prof()
PyStub.logs()

# out_mem -> TEST_MEM_AREA_2 buf
out_mem = PyStub.getmem('TEST_MEM_AREA_2')

out_tst= out_mem.popu32()
print(out_tst)
bool(set(out_r).intersection(out_tst)) #Compare two buffers

PyStub.run(20000)
PyStub.prof()









