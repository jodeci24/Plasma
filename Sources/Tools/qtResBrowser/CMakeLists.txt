include_directories("../../Plasma/Apps")
include_directories("../../Plasma/CoreLib")
include_directories("../../Plasma/FeatureLib/inc")
include_directories("../../Plasma/FeatureLib")
include_directories("../../Plasma/NucleusLib/inc")
include_directories("../../Plasma/NucleusLib")
include_directories("../../Plasma/PubUtilLib/inc")
include_directories("../../Plasma/PubUtilLib")

find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui QtSvg QtMain)
include(${QT_USE_FILE})

set(qtResBrowser_MOC_Headers
    qtResBrowser.h
    qtTreeItem.h
)

set(qtResBrowser_Sources
    qtResBrowser.cpp
    qtTreeItem.cpp
)

if(WIN32)
    set(qtResBrowser_Sources ${qtResBrowser_Sources} qtResBrowser.rc)
endif()

# generate rules for building source files from the resources
qt4_add_resources(qtResBrowser_RCC images.qrc)
# generate rules for building source files that moc generates
qt4_wrap_cpp(qtResBrowser_MOC ${qtResBrowser_MOC_Headers})

if(APPLE)
    set(MACOSX_BUNDLE true)
    set(MACOSX_BUNDLE_NAME "qtResBrowser")
    set(MACOSX_BUNDLE_ICON_FILE "icons/qtResBrowser.icns")
endif()

add_executable(qtResBrowser WIN32 MACOSX_BUNDLE ${qtResBrowser_Sources} ${qtResBrowser_MOC} ${qtResBrowser_RCC})

target_link_libraries(qtResBrowser ${QT_LIBRARIES})
target_link_libraries(qtResBrowser CoreLib)
target_link_libraries(qtResBrowser plFile)
target_link_libraries(qtResBrowser plMessage)
target_link_libraries(qtResBrowser plResMgr)
target_link_libraries(qtResBrowser plScene)
target_link_libraries(qtResBrowser plStatusLog)
target_link_libraries(qtResBrowser plUnifiedTime)
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
