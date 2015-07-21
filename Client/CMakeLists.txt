cmake_minimum_required(VERSION 2.8)
project ("XALPlayerClient")

# VERSION
################################################################################
set( XALPLAYERCLIENT_MAJOR_VERSION 0 )
set( XALPLAYERCLIENT_MINOR_VERSION 1 )
################################################################################

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# REQUIRED LIBRARIES
################################################################################
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake_modules" ${CMAKE_MODULE_PATH})

# SFML
find_package(SFML 2.3 REQUIRED network graphics window system)
find_package(TGUI 0.7)
find_package(sfeMovie)
################################################################################

set( INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include" )
set( SOURCE_PATH "${PROJECT_SOURCE_DIR}/src" )

file(
        GLOB_RECURSE
        SOURCES
        ${SOURCE_PATH}/*
)

include_directories( ${INCLUDE_PATH}
	${SFML_INCLUDE_DIR}
	${TGUI_INCLUDE_DIR}
	${SFEMOVIE_INCLUDE_DIR} )

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
  add_compile_options(-std=c++14 -pedantic -Wall)
endif()
################################################################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_executable(
        "XALPlayerClient"
        ${SOURCES}
)

target_link_libraries(XALPlayerClient ${SFML_LIBRARIES} ${TGUI_LIBRARY} ${SFEMOVIE_LIBRARY} )
