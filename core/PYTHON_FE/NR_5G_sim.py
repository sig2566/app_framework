import py5Gstub
ver= py5Gstub.ver()
print(ver)
#py5Gstub.init("../COMMON/Projects/FRAMEWORK/sys_configure.xml")   
py5Gstub.init("../COMMON/Projects/FRAMEWORK/sys_main_5g_handler.xml")      
py5Gstub.modulenames()
py5Gstub.areanames()
py5Gstub.start_logs()
#py5Gstub.run_test("MAIN_5G_HANDLER","")
py5Gstub.run_test("HANDLER_TEST_MOD","../Tests/DU/GLUE_LOGIC_TEST/Test_Vec/Test1")
# init: load all shared libraries
#		init all modules
#		restart al modules 
#		define mem areas and allocation storage
#		connection betwenn famework componnents 
#		init log system 
#		init profilers

# prints/get profiling results
py5Gstub.prof()
# prints/get logs out
buf=py5Gstub.logs()
print(len(buf))
for st in buf:
    print(st)
    
    

buf=py5Gstub.logs()
print(len(buf))
for st in buf:
    print(st)  









