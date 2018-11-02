set(Converters_header_files
  DRurqmd.h
  DRBase.h
)
set(Converters_cxx_files
  DRurqmd.cxx
  DRBase.cxx
)

#---Generate dictionary for Converters
ROOT_GENERATE_DICTIONARY(G__Converters
${Converters_header_files}
LINKDEF Converters.LinkDef.h)

add_library(Converters SHARED
${Converters_cxx_files}
G__Converters.cxx)

target_link_libraries(Converters Event)