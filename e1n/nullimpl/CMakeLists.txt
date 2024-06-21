# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.

cmake_minimum_required(VERSION 3.22)

project(frif_e1n_nullimpl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Discover name and version
# Sets LIBRARY_NAME and LIBRARY_VERSION in parent context
function(find_name_and_version)
	# Read each line of a file into a list
	file(READ frif_e1n_nullimpl.h HEADER_CONTENTS)
	string(REGEX REPLACE ";" "\\\\;" HEADER_CONTENTS "${HEADER_CONTENTS}")
	string(REGEX REPLACE "\n" ";" HEADER_CONTENTS "${HEADER_CONTENTS}")

	foreach(line ${HEADER_CONTENTS})
		# Find line with version number
		string(REGEX MATCH ".*libraryVersionNumber{0x(....)}.*" VERSION_LINE ${line})
		if(VERSION_LINE AND NOT LIBRARY_VERSION)
			string(TOUPPER ${CMAKE_MATCH_1} LIBRARY_VERSION)
			set(LIBRARY_VERSION ${LIBRARY_VERSION} PARENT_SCOPE)
		endif()

		# Find line with library name
		string(REGEX MATCH ".*libraryIdentifier\\[\\]{\"(.+)\"}.*" NAME_LINE ${line})
		if(NAME_LINE AND NOT LIBRARY_NAME)
			set(LIBRARY_NAME ${CMAKE_MATCH_1} PARENT_SCOPE)
		endif()
	endforeach()
endfunction()

find_name_and_version()
if(NOT LIBRARY_NAME OR NOT LIBRARY_VERSION)
	message(FATAL_ERROR "Could not discover library name or version")
endif()
set(LIB_NAME "frif_e1n_${LIBRARY_NAME}_${LIBRARY_VERSION}")

add_library(${LIB_NAME} SHARED)
target_sources(${LIB_NAME} PRIVATE frif_e1n_nullimpl.cpp)
target_include_directories(${LIB_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../../include)
target_include_directories(${LIB_NAME} PRIVATE ${PROJECT_SOURCE_DIR})

# Depend on libfrif_e1n
add_subdirectory(${PROJECT_SOURCE_DIR}/../libfrif_e1n ${CMAKE_CURRENT_BINARY_DIR}/libfrif_e1n)
add_dependencies(${LIB_NAME} frif_e1n)
target_link_libraries(${LIB_NAME} PUBLIC frif_e1n)

# Depend on libfrif
add_subdirectory(${PROJECT_SOURCE_DIR}/../../libfrif ${CMAKE_CURRENT_BINARY_DIR}/libfrif)
add_dependencies(${LIB_NAME} frif)
target_link_libraries(${LIB_NAME} PUBLIC frif)

# Enable documentation warnings with clang
target_compile_options(${LIB_NAME} PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
          -Wdocumentation>)

# Enable all clang warnings
#target_compile_options(${LIB_NAME} PRIVATE
#     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
#          -Weverything -Wno-c++98-compat -Wno-padded>)

# Turn on common clang/gcc warnings
target_compile_options(${LIB_NAME} PRIVATE
   -Wall -Wextra -pedantic -Wconversion -Wsign-conversion -Wshadow
   -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused
   -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion
   -Wformat=2 -Wimplicit-fallthrough)

# gcc has some additional useful warnings
target_compile_options(${LIB_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
         -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
         -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion>)

# Turn off unused variable warning
target_compile_options(${LIB_NAME} PRIVATE
    -Wno-unused-parameter)

# Set RPATH to $ORIGIN
set_target_properties(${LIB_NAME} PROPERTIES
    BUILD_RPATH_USE_ORIGIN TRUE)
if(UNIX)
	set_target_properties(${LIB_NAME} PROPERTIES
	    INSTALL_RPATH $ORIGIN)
endif()

# Use RUNPATH over RPATH
if(NOT APPLE)
	set_target_properties(${LIB_NAME} PROPERTIES
	    LINK_FLAGS -Wl,--enable-new-dtags)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../validation CACHE PATH "..." FORCE)
endif()

include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
