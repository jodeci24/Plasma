include_directories(../../../CoreLib)
include_directories(../../../NucleusLib)
include_directories(../../../NucleusLib/inc)
include_directories(../../../PubUtilLib)
include_directories(../../../FeatureLib)
include_directories(.)

include_directories(${PYTHON_INCLUDE_DIR})

set(pfPython_PyCXX_SOURCES
    CXX/Config.hxx
    CXX/Exception.hxx
    CXX/Extensions.hxx
    CXX/IndirectPythonInterface.hxx
    CXX/Objects.hxx
    CXX/Version.hxx
    CXX/WrapPython.h

    cxxextensions.c
    cxx_extensions.cxx
    cxxsupport.cxx
    IndirectPythonInterface.cxx
    )

set(pfPython_PyCXX2_SOURCES
    CXX/Python2/Config.hxx
    CXX/Python2/CxxDebug.hxx
    CXX/Python2/Exception.hxx
    CXX/Python2/ExtensionModule.hxx
    CXX/Python2/ExtensionOldType.hxx
    CXX/Python2/Extensions.hxx
    CXX/Python2/ExtensionType.hxx
    CXX/Python2/ExtensionTypeBase.hxx
    CXX/Python2/IndirectPythonInterface.hxx
    CXX/Python2/Objects.hxx
    CXX/Python2/PythonType.hxx

    Python2/cxxextensions.c
    Python2/cxx_extensions.cxx
    Python2/cxxsupport.cxx
    )

set(pfPython_API_SOURCES
    pyHelpers.cpp
    pyModule.cpp
    ptKey.cpp
    ptStream.cpp
    )

set(pfPython_API_HEADERS
    pyHelpers.h
    pyModule.h
    ptKey.h
    ptStream.h
    )


SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${Plasma_BINARY_DIR}/bin)
add_library(pfPythonAPI SHARED ${pfPython_PyCXX_SOURCES} ${pfPython_API_HEADERS} ${pfPython_API_SOURCES})

set_target_properties(pfPythonAPI PROPERTIES OUTPUT_NAME "plasma")
set_target_properties(pfPythonAPI PROPERTIES PREFIX "")

target_link_libraries(pfPythonAPI CoreLib)
target_link_libraries(pfPythonAPI plFile)
target_link_libraries(pfPythonAPI ${PYTHON_LIBRARIES})

source_group("PyCXX Files" FILES ${pfPython_PyCXX_SOURCES})
source_group("Source Files" FILES ${pfPython_API_SOURCES})
source_group("Header Files" FILES ${pfPython_API_HEADERS})
