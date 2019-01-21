set(Event_header_files
  DataReaderEvent.h
)
set(Event_cxx_files
  DataReaderEvent.cxx
)

#---Generate dictionary for Event
ROOT_GENERATE_DICTIONARY(G__Event
${Event_header_files}
LINKDEF Event.LinkDef.h)

add_library(Event SHARED
${Event_cxx_files}
G__Event.cxx)

target_link_libraries(Event ${ROOT_LIBRARIES})
