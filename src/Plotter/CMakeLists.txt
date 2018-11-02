set(Plotter_header_files
  DataReaderPlotter.h
)
set(Plotter_cxx_files
  DataReaderPlotter.cxx
)

#---Generate dictionary for Plotter
ROOT_GENERATE_DICTIONARY(G__Plotter
${Plotter_header_files}
LINKDEF Plotter.LinkDef.h)

add_library(Plotter SHARED
${Plotter_cxx_files}
G__Plotter.cxx)

target_link_libraries(Plotter Event)