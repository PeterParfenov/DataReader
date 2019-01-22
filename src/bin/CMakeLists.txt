#---Create  a main executable program for DataReader
add_executable(Main DataReaderMain.cxx)
target_link_libraries(Main DataReader)

#---Create an executable file CentraltyManager
add_executable(CMMain CentralityManagerMain.cpp)
target_link_libraries(CMMain CentralityManager)