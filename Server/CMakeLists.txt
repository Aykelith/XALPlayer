cmake_minimum_required(VERSION 2.8)
project ("XALPlayerServer")

# VERSION
################################################################################
set( XALPLAYERSERVER_MAJOR_VERSION 0 )
set( XALPLAYERSERVER_MINOR_VERSION 1 )
################################################################################

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# REQUIRED LIBRARIES
################################################################################
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake_modules" ${CMAKE_MODULE_PATH})

# SFML
find_package(SFML 2.3 REQUIRED network system)
################################################################################

set( INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include" )
set( SOURCE_PATH "${PROJECT_SOURCE_DIR}/src" )

file(
        GLOB_RECURSE
        SOURCES
        ${SOURCE_PATH}/*
)

include_directories( ${INCLUDE_PATH}
	${SFML_INCLUDE_DIR} )

# Compiler settings
################################################################################
# Detect compiler
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(USING_GCC TRUE)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(USING_CLANG TRUE)
endif()

# For GCC and Clang, enable C++11 support and add some other flags
if(USING_GCC OR USING_CLANG)
  add_compile_options(-std=c++11 -pedantic -Wall)
endif()
################################################################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_executable(
        "XALPlayerServer"
        ${SOURCES}
)

target_link_libraries(XALPlayerServer ${SFML_LIBRARIES} )
