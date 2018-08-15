# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(DataReader)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

#---Link UniGen library
set(UNIGEN_INCLUDE $ENV{UNIGEN}/base/include)
set(UNIGEN_LIB $ENV{UNIGEN}/lib)
set(UNIGEN libUniGen.so)
link_directories(${UNIGEN_LIB})

include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS} ${UNIGEN_INCLUDE})
add_definitions(${ROOT_CXX_FLAGS})

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-O2")

#---Generate dictionary for DataReader
ROOT_GENERATE_DICTIONARY(G__DataReader
DataReader.h
DataReaderPlotter.h
DataReaderEvent.h
LINKDEF DataReader.LinkDef.h)

#---Create a shared library with geneated dictionary for DataReader class
add_library(DataReader SHARED DataReader.cxx
DataReaderEvent.cxx
DataReaderPlotter.cxx
G__DataReader.cxx)
target_link_libraries(DataReader ${UNIGEN} ${ROOT_LIBRARIES})

#---Create  a main program using the library
add_executable(Main DataReaderMain.cxx)
target_link_libraries(Main DataReader)