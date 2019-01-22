set(EPManager_header_files
  DRQvector.h
	DRQvCalc.h
	DRResCalc.h
)
set(EPManager_cxx_files
	DRQvector.cxx
	DRQvCalc.cxx
	DRResCalc.cxx
)

#---Generate dictionary for EPManager
ROOT_GENERATE_DICTIONARY(G__EPManager
${EPManager_header_files}
LINKDEF EPManager.LinkDef.h)

add_library(EPManager SHARED
${EPManager_cxx_files}
G__EPManager.cxx)

target_link_libraries(EPManager Event)
