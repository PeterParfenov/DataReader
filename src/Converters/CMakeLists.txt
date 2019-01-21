set(Converters_header_files
  DRurqmd.h
  DRlaqgsm.h
  DRphsd.h
  DRunigen.h
  DRvsdt.h
  DRBase.h
  DRManager.h
)
set(Converters_cxx_files
  DRurqmd.cxx
  DRlaqgsm.cxx
  DRphsd.cxx
  DRunigen.cxx
  DRvsdt.cxx
  DRBase.cxx
  DRManager.cxx
)

#---Generate dictionary for Converters
ROOT_GENERATE_DICTIONARY(G__Converters
${Converters_header_files}
LINKDEF Converters.LinkDef.h)

add_library(Converters SHARED
${Converters_cxx_files}
G__Converters.cxx)

target_link_libraries(Converters ${UNIGEN_LIB} ${VSDT} Plotter)
