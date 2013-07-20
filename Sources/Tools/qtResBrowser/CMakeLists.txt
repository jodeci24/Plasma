include_directories("../../Plasma/Apps")
include_directories("../../Plasma/CoreLib")
include_directories("../../Plasma/FeatureLib/inc")
include_directories("../../Plasma/FeatureLib")
include_directories("../../Plasma/NucleusLib/inc")
include_directories("../../Plasma/NucleusLib")
include_directories("../../Plasma/PubUtilLib/inc")
include_directories("../../Plasma/PubUtilLib")

include_directories(${OPENAL_INCLUDE_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(OpenGL REQUIRED)

set(qtResBrowser_HEADERS
    qtResBrowser.h
    qtTreeItem.h
    types/qtCreatable.h
    types/qtMipmap.h
)

set(qtResBrowser_SOURCES
    plAllCreatables.cpp
    qtResBrowser.cpp
    qtTreeItem.cpp
    types/qtCreatable.cpp
    types/qtMipmap.cpp
)

if(WIN32)
    set(qtResBrowser_SOURCES ${qtResBrowser_SOURCES} qtResBrowser.rc)
endif()

# generate rules for building source files from the resources
qt5_add_resources(qtResBrowser_RCC images.qrc shaders.qrc)


if(APPLE)
    set(MACOSX_BUNDLE true)
    set(MACOSX_BUNDLE_NAME "qtResBrowser")
    set(MACOSX_BUNDLE_ICON_FILE "icons/qtResBrowser.icns")
endif()

add_executable(qtResBrowser WIN32 MACOSX_BUNDLE ${qtResBrowser_SOURCES} ${qtResBrowser_RCC})

qt5_use_modules(qtResBrowser Widgets Svg OpenGL)

target_link_libraries(qtResBrowser CoreLib)
target_link_libraries(qtResBrowser pnDispatch)
target_link_libraries(qtResBrowser pnFactory)
target_link_libraries(qtResBrowser pnKeyedObject)
target_link_libraries(qtResBrowser pnMessage)
target_link_libraries(qtResBrowser pnNetCommon)
target_link_libraries(qtResBrowser pnNucleusInc)
target_link_libraries(qtResBrowser pnSceneObject)
target_link_libraries(qtResBrowser pnTimer)
target_link_libraries(qtResBrowser pnUtils)
target_link_libraries(qtResBrowser pnUUID)
#target_link_libraries(qtResBrowser plAudible)
#target_link_libraries(qtResBrowser plAudioCore)
#target_link_libraries(qtResBrowser plAudio)
#target_link_libraries(qtResBrowser plDrawable)
target_link_libraries(qtResBrowser plFile)
target_link_libraries(qtResBrowser plGImage)
target_link_libraries(qtResBrowser plMessage)
#target_link_libraries(qtResBrowser plModifier)
target_link_libraries(qtResBrowser plResMgr)
target_link_libraries(qtResBrowser plScene)
target_link_libraries(qtResBrowser plStatusLog)
#target_link_libraries(qtResBrowser plSurface)
target_link_libraries(qtResBrowser plUnifiedTime)

target_link_libraries(qtResBrowser ${OPENGL_LIBRARIES})
target_link_libraries(qtResBrowser ${JPEG_LIBRARY})
target_link_libraries(qtResBrowser ${PNG_LIBRARY})
#target_link_libraries(qtResBrowser ${OPENAL_LIBRARY})
#target_link_libraries(qtResBrowser ${Speex_LIBRARY})
#target_link_libraries(qtResBrowser ${Ogg_LIBRARIES})
#target_link_libraries(qtResBrowser ${Vorbis_LIBRARIES})

if (WIN32)
    target_link_libraries(qtResBrowser Rpcrt4)
    target_link_libraries(qtResBrowser Version)
    target_link_libraries(qtResBrowser Vfw32)
    target_link_libraries(qtResBrowser Ws2_32)
    target_link_libraries(qtResBrowser winmm)
    target_link_libraries(qtResBrowser strmiids)
    target_link_libraries(qtResBrowser comctl32)
    target_link_libraries(qtResBrowser shlwapi)
endif(WIN32)

if(USE_VLD)
    target_link_libraries(qtResBrowser ${VLD_LIBRARY})
endif()

source_group("Source Files" FILES ${qtResBrowser_SOURCES})
source_group("Header Files" FILES ${qtResBrowser_HEADERS})
