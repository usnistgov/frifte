# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.

cmake_minimum_required(VERSION 3.22)

project(frifte_e1n_validation)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(frifte_e1n_validation)
target_sources(frifte_e1n_validation PRIVATE
    frifte_e1n_data.cpp
    frifte_e1n_validation.cpp)
target_include_directories(frifte_e1n_validation PRIVATE .)
target_include_directories(frifte_e1n_validation PUBLIC ../../../include)

# Build libfrifte_e1n first (which itself builds libfrifte)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../libfrifte_e1n ${CMAKE_CURRENT_BINARY_DIR}/libfrifte_e1n)
add_dependencies(frifte_e1n_validation frifte_e1n)
target_link_libraries(frifte_e1n_validation PUBLIC frifte_e1n)

# Find the participant's core library
# Writes CORE_LIB as the name of the library in the parent scope
function(find_core_library)
	set(CORE_LIB_PREFIX "libfrifte_e1n_")
	file(GLOB CORE_LIB_POSSIBILITIES
	    LIST_DIRECTORIES true
	    RELATIVE ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
	    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${CORE_LIB_PREFIX}*_*${CMAKE_SHARED_LIBRARY_SUFFIX})

	foreach (lib ${CORE_LIB_POSSIBILITIES})
		if(CORE_LIB)
			message(FATAL_ERROR "More than one core library found in ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}. Please review naming conventions.")
		endif()

		string(REGEX MATCH "${CORE_LIB_PREFIX}[A-Za-z0-9]+_[0-9A-F][0-9A-F][0-9A-F][0-9A-F]${CMAKE_SHARED_LIBRARY_SUFFIX}" CORE_LIB ${lib})
	endforeach()

	set(CORE_LIB ${CORE_LIB} PARENT_SCOPE)
endfunction()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	message(AUTHOR_WARNING "CMAKE_INSTALL_PREFIX not set, forcing to validation directory")
	set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/.. CACHE PATH "..." FORCE)
endif()

find_core_library()
if(NOT CORE_LIB)
	message(FATAL_ERROR "No core library found in ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}. Please review naming conventions.")
else()
	message(STATUS "Found core library: ${CORE_LIB}")
	target_link_libraries(frifte_e1n_validation PUBLIC ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${CORE_LIB})
endif()

# Extern the version symbols
target_compile_definitions(frifte_e1n_validation PRIVATE NIST_EXTERN_FRIFTE_API_VERSION NIST_EXTERN_FRIFTE_E1N_API_VERSION)

# Enable documentation warnings with clang
target_compile_options(frifte_e1n_validation PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
          -Wdocumentation>)

# Enable all clang warnings
#target_compile_options(frifte_e1n_validation PRIVATE
#     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
#          -Weverything -Wno-c++98-compat -Wno-padded>)

# Turn on common clang/gcc warnings
target_compile_options(frifte_e1n_validation PRIVATE
   -Wall -Wextra -pedantic -Wconversion -Wsign-conversion -Wshadow
   -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused
   -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion
   -Wformat=2 -Wimplicit-fallthrough)

# gcc has some additional useful warnings
target_compile_options(frifte_e1n_validation PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
         -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
         -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion>)

# Set RPATH
set_target_properties(frifte_e1n_validation PROPERTIES
    # Set RPATH to installed version of libs
    INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

# Don't also include local copy of libs in RPATH
set_target_properties(frifte_e1n_validation PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE)

# Use RUNPATH over RPATH
if(NOT APPLE)
	set_target_properties(frifte_e1n_validation PROPERTIES
	    LINK_FLAGS -Wl,--enable-new-dtags)
endif()

install(TARGETS frifte_e1n_validation
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/../${CMAKE_INSTALL_BINDIR})
