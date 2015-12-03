cmake_minimum_required(VERSION 3.2.2)

# Boost
set(Boost_USE_STATIC_LIBS  ON)
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# ASSIMP for model loading
find_package(PkgConfig REQUIRED)
pkg_search_module(ASSIMP REQUIRED assimp)
include_directories( ${ASSIMP_INLUDE_DIRS} )

# Point Cloud Library
find_package(PCL REQUIRED common kdtree search)
include_directories(${PCL_INCLUDE_DIRS})
include_directories(${FLANN_INCLUDE_DIRS})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIR} )

# GLM for OpenGL linear algebra (Already included as pkgs/glm)

# ASSIMP for model loading
#pkg_search_module(ASSIMP REQUIRED assimp)
#include_directories( ${ASSIMP_INLUDE_DIRS} )

# Qt
set(CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
set(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
    # Set Qt files and resources here
file(GLOB uis resources/*.ui)
file(GLOB resources resources/*.qrc)
QT5_WRAP_UI(ui_srcs ${uis})
QT5_ADD_RESOURCES(resource_srcs ${resources})

# Google Proto Buffers
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

# Osmium
find_package(Osmium REQUIRED COMPONENTS io proj)
include_directories(${OSMIUM_INCLUDE_DIRS})

#################################################
#   Project Files
#################################################
file(GLOB core_source core/*.cpp core/*.cc)
file(GLOB core_header core/*.h)
file(GLOB util_source util/*.cpp)
file(GLOB util_header util/*.h)
file(GLOB ui_source ui/*.cpp)
file(GLOB ui_header ui/*.h)
file(GLOB exec_source exec/*.cpp)
set(srcs ${core_source}
         ${util_source}
         ${ui_source}
         ${exec_source})
set(incs ${core_header}
         ${util_header}
         ${ui_header})
include_directories(core ui util)
SET_SOURCE_FILES_PROPERTIES(${srcs} PROPERTIES OBJECT_DEPENDS "${ui_srcs}")

add_executable(${exe_name} ${srcs})
target_link_libraries(${exe_name}
                      ${CGAL_LIBRARIES}
                      ${Boost_SYSTEM_LIBRARY} 
                      ${Boost_FILESYSTEM_LIBRARY}
                      assimp ${ASSIMP_LIBRARIES}
                      ${PCL_COMMON_LIBRARY} 
                      ${PCL_KDTREE_LIBRARY}
                      ${PCL_SEARCH_LIBRARY}
                      ${OPENGL_LIBRARIES}
                      Qt5::Core Qt5::Widgets Qt5::OpenGL
                      ${OSMIUM_LIBRARIES}
                      ${PROTOBUF_LIBRARIES}
)

set_target_properties(${exe_name} PROPERTIES DEBUG_POSTFIX _debug)
set_target_properties(${exe_name} PROPERTIES RELEASE_POSTFIX _release)
