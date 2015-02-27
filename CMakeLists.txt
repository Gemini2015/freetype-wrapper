cmake_minimum_required(VERSION 2.8)

project(freetype-wrapper)

set(FREETYPE_WRAPPER_INC ${PROJECT_SOURCE_DIR}/freetype-wrapper)
set(FREETYPE_INC ${PROJECT_SOURCE_DIR}/deps/freetype/include)
set(FREETYPE_LIB ${PROJECT_SOURCE_DIR}/deps/freetype/lib/freetype.lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

set(CMAKE_BUILD_TYPE Debug)

link_directories(${PROJECT_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})

include_directories(${FREETYPE_WRAPPER_INC})
include_directories(${FREETYPE_INC})

add_subdirectory(freetype-wrapper)
add_subdirectory(examples)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/media 
	DESTINATION ${PROJECT_BINARY_DIR})