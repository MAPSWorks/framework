cmake_minimum_required(VERSION 3.2.2)

project(framework_template)
set(exe_name framework_template)

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 ) # Used by YouCompleteMe Required to export .json compile file

SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(OUTPUT_LIB_DIR ${PROJECT_BINARY_DIR}/lib)
set(OUTPUT_BIN_DIR ${PROJECT_BINARY_DIR}/bin)
make_directory(${OUTPUT_LIB_DIR})
make_directory(${OUTPUT_BIN_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_LIB_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_BIN_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_LIB_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

################################################################################
####          3rd Party Header Only Libraries              #####################
################################################################################
add_subdirectory(pkgs/glm)

################################################################################
####           SOURCE CODE                                 #####################
################################################################################
add_subdirectory(src)
