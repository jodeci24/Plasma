include_directories("../../Apps")
include_directories("../../CoreLib")
include_directories("../../FeatureLib/inc")
include_directories("../../FeatureLib")
include_directories("../../NucleusLib/inc")
include_directories("../../NucleusLib")
include_directories("../../PubUtilLib/inc")
include_directories("../../PubUtilLib")

set(passtest_SOURCES
    main.cpp
)

add_executable(passtest ${passtest_SOURCES})
target_link_libraries(passtest CoreLib pfPasswordStore)

if(USE_VLD)
    target_link_libraries(passtest ${VLD_LIBRARY})
endif()

source_group("Source Files" FILES ${passtest_SOURCES})

