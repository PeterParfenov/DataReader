set(CentralityManager_headers_files
  CentralityManager.h
)

set(CentralityManager_cxx_files
  CentralityManager.cxx
)

#---Generate dictionary for CentralityManager
ROOT_GENERATE_DICTIONARY(G__CentralityManager
  ${CentralityManager_headers_files}
LINKDEF CentralityManager.LinkDef.h)

#---Create a shared library with geneated dictionary for CentralityManager class
add_library(CentralityManager SHARED
  ${CentralityManager_cxx_files}
G__CentralityManager.cxx)

target_link_libraries(CentralityManager Event ${ROOT_LIBRARIES})
