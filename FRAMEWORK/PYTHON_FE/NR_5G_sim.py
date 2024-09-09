import PyStub
ver= PyStub.ver()
print(ver)
#PyStub.init("../COMMON/Projects/FRAMEWORK/sys_configure.xml")   
PyStub.init("../COMMON/Projects/FRAMEWORK/sys_main_5g_handler.xml")      
PyStub.modulenames()
PyStub.areanames()
PyStub.start_logs()
#PyStub.run_test("MAIN_5G_HANDLER","")
PyStub.run_test("HANDLER_TEST_MOD","../Tests/DU/GLUE_LOGIC_TEST/Test_Vec/Test1")
# init: load all shared libraries
#		init all modules
#		restart al modules 
#		define mem areas and allocation storage
#		connection betwenn famework componnents 
#		init log system 
#		init profilers

# prints/get profiling results
PyStub.prof()
# prints/get logs out
buf=PyStub.logs()
print(len(buf))
for st in buf:
    print(st)
    
    

buf=PyStub.logs()
print(len(buf))
for st in buf:
    print(st)  









