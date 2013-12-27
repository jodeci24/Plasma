include_directories("../../Apps")
include_directories("../../CoreLib")
include_directories("../../FeatureLib/inc")
include_directories("../../FeatureLib")
include_directories("../../NucleusLib/inc")
include_directories("../../NucleusLib")
include_directories("../../PubUtilLib/inc")
include_directories("../../PubUtilLib")

set(plSocketTest_SOURCES
    main.cpp
)

add_executable(plSocketTest ${plSocketTest_SOURCES})
target_link_libraries(plSocketTest CoreLib plSockets pnNetCommon)

if(USE_VLD)
    target_link_libraries(plSocketTest ${VLD_LIBRARY})
endif()

source_group("Source Files" FILES ${plSocketTest_SOURCES})

