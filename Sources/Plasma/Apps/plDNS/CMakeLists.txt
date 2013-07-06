include_directories("../../CoreLib")
include_directories("../../NucleusLib")
include_directories("../../NucleusLib/inc")
include_directories("../../PubUtilLib/inc")
include_directories("../../PubUtilLib")

include_directories(${CARES_INCLUDE_DIR})

set(plDNS_SOURCES
    main.cpp
)

add_executable(plDNS ${plDNS_SOURCES})
target_link_libraries(plDNS CoreLib plStatusLog pnNetCommon)
target_link_libraries(plDNS ${CARES_LIBRARIES})

if(USE_VLD)
    target_link_libraries(plDNS ${VLD_LIBRARY})
endif()

source_group("Source Files" FILES ${plDNS_SOURCES})
