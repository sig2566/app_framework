import PyStub
str = PyStub.ver()

print("version="+str)
# init: load all shared libraries
#        init all modules
#        restart al modules 
#        define mem areas and allocation storage
#        connection betwenn famework componnents 
#        init log system 
#        init profilers
PyStub.init("../FRAMEWORK/RUN_ENV/sys_configure.xml")      
# prints/get list of modules names
print(PyStub.modulenames())
# prints/get list of mem areas names
print(PyStub.areanames())
# call module stand alone 
PyStub.callmod("SAMPLE_MODULE")
# prints/get profiling results
PyStub.stop()
PyStub.prof()

# prints/get logs out
PyStub.logs()


print("Finish")


