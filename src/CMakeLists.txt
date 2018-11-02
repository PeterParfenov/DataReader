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
message("Checking UniGen:")
if(DEFINED ENV{UNIGEN})
  message("UniGen found: " $ENV{UNIGEN})

  set(UNIGEN_INCLUDE $ENV{UNIGEN}/base/include)
  set(UNIGEN_LIB $ENV{UNIGEN}/lib)
  set(UNIGEN libUniGen.so)
  link_directories(${UNIGEN_LIB})

else(DEFINED ENV{UNIGEN})
  message("UniGen not found")
endif(DEFINED ENV{UNIGEN})

if(DEFINED ENV{UNIGEN})
  set(INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Converters
    ${CMAKE_SOURCE_DIR}/Event
    ${CMAKE_SOURCE_DIR}/Plotter
    ${ROOT_INCLUDE_DIRS}
    ${UNIGEN_INCLUDE}
  )
else(DEFINED ENV{UNIGEN})
  set(INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Converters
    ${CMAKE_SOURCE_DIR}/Event
    ${CMAKE_SOURCE_DIR}/Plotter
    ${ROOT_INCLUDE_DIRS}
  )
endif(DEFINED ENV{UNIGEN})

include_directories(${INCLUDE_DIRECTORIES}
)
add_definitions(${ROOT_CXX_FLAGS})

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-O2")

add_subdirectory(Event)
add_subdirectory(Plotter)
add_subdirectory(Converters)

#---Generate dictionary for DataReader
ROOT_GENERATE_DICTIONARY(G__DataReader
  DataReader.h
LINKDEF DataReader.LinkDef.h)

#---Create a shared library with geneated dictionary for DataReader class
add_library(DataReader SHARED
  DataReader.cxx
G__DataReader.cxx)

if(DEFINED ENV{UNIGEN})
  target_link_libraries(DataReader Plotter Converters ${UNIGEN} ${ROOT_LIBRARIES})
else(DEFINED ENV{UNIGEN})
  target_link_libraries(DataReader Plotter Converters ${ROOT_LIBRARIES})
endif(DEFINED ENV{UNIGEN})

#---Create  a main program using the library
add_executable(Main DataReaderMain.cxx)
target_link_libraries(Main DataReader)