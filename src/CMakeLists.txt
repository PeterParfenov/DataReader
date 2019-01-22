# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(DataReader)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net EG MathMore)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

#---Link UniGen library
message("Checking UniGen:")
if(DEFINED ENV{UNIGEN})
  message("UniGen found: " $ENV{UNIGEN})

  set(UNIGEN_ENV $ENV{UNIGEN})
  set(UNIGEN_INCLUDE ${UNIGEN_ENV}/base/include)
  set(UNIGEN_LIB_DIR ${UNIGEN_ENV}/lib)
  set(UNIGEN libUniGen.so)
  #link_directories(${UNIGEN_LIB_DIR})
  find_library(UNIGEN_LIB REQUIRED
    NAMES ${UNIGEN}
    HINTS ${UNIGEN_LIB_DIR} ${UNIGEN_ENV})

else(DEFINED ENV{UNIGEN})
  message(FATAL_ERROR "UniGen not found")
endif(DEFINED ENV{UNIGEN})

message("checking vsdt:")
if(DEFINED ENV{VSDT})
  message("vsdt found: " $ENV{VSDT})
  set(VSDT_ENV $ENV{VSDT})
  set(VSDT_LIB libvsdt.so)
  #link_directories(${VSDT_ENV})
  find_library(VSDT REQUIRED
    NAMES ${VSDT_LIB}
    HINTS ${VSDT_ENV} $ENV{PATH})
else(DEFINED ENV{VSDT})
  message(FATAL_ERROR "vsdt is not found")
endif(DEFINED ENV{VSDT})

set(INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Converters
    ${CMAKE_SOURCE_DIR}/Event
    ${CMAKE_SOURCE_DIR}/Plotter
    ${CMAKE_SOURCE_DIR}/CentralityManager
    ${CMAKE_SOURCE_DIR}/EPManager
    ${CMAKE_SOURCE_DIR}/bin
    ${ROOT_INCLUDE_DIRS}
    ${UNIGEN_INCLUDE}
    ${VSDT_ENV}
)

include_directories(${INCLUDE_DIRECTORIES}
)
add_definitions(${ROOT_CXX_FLAGS})

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-O2")

add_subdirectory(Event)
add_subdirectory(Plotter)
add_subdirectory(Converters)
add_subdirectory(CentralityManager)
add_subdirectory(EPManager)

#---Generate dictionary for DataReader
ROOT_GENERATE_DICTIONARY(G__DataReader
  DataReader.h
LINKDEF DataReader.LinkDef.h)

#---Create a shared library with geneated dictionary for DataReader class
add_library(DataReader SHARED
  DataReader.cxx
G__DataReader.cxx)

target_link_libraries(DataReader Converters)

add_subdirectory(bin)