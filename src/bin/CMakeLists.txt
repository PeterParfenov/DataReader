#---Create  a main executable program for DataReader
add_executable(Main DataReaderMain.cxx)
target_link_libraries(Main DataReader)

#---Create an executable file CentraltyManager
add_executable(CMMain CentralityManagerMain.cpp)
target_link_libraries(CMMain CentralityManager)

#---Create  a main executable program for GetMult
add_executable(GetMult GetMult.cxx)
target_link_libraries(GetMult DataReader)

#---Create  a main executable program for GetRes
add_executable(GetRes GetRes.cxx)
target_link_libraries(GetRes DataReader)

#---Create  a main executable program for CalcRes
add_executable(CalcRes CalcRes.cxx)
target_link_libraries(CalcRes EPManager)