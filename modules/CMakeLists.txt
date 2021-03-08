aux_source_directory(. MODULE_SOURCE)

add_library(Modules ${MODULE_SOURCE})

if(WIN32)
    target_link_libraries(Modules WS2_32)
elseif(UNIX)
    target_link_libraries(Modules pthread)
endif()