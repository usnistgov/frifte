# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.

cmake_minimum_required(VERSION 3.22)

project(frifte_e1n_library)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(frifte_e1n SHARED)
target_sources(frifte_e1n PRIVATE
    libfrifte_e1n.cpp)
target_include_directories(frifte_e1n PRIVATE ${PROJECT_SOURCE_DIR}/../../include)

# Extern the version symbols
target_compile_definitions(frifte_e1n PRIVATE NIST_EXTERN_FRIFTE_E1N_API_VERSION)

# Require libfrifte
add_subdirectory(${PROJECT_SOURCE_DIR}/../../libfrifte ${CMAKE_CURRENT_BINARY_DIR}/libfrifte)
add_dependencies(frifte_e1n frifte)
target_link_libraries(frifte_e1n PUBLIC frifte)

# Set RPATH to $ORIGIN
set_target_properties(frifte_e1n PROPERTIES
    BUILD_RPATH_USE_ORIGIN TRUE)
if(UNIX)
	set_target_properties(frifte_e1n PROPERTIES
	    INSTALL_RPATH $ORIGIN)
endif()

# Use RUNPATH over RPATH
if(NOT APPLE)
	set_target_properties(frifte_e1n PROPERTIES
	    LINK_FLAGS -Wl,--enable-new-dtags)
endif()

# Enable documentation warnings with clang
target_compile_options(frifte_e1n PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
          -Wdocumentation>)

# Enable all clang warnings
#target_compile_options(frifte_e1n PRIVATE
#     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
#          -Weverything -Wno-c++98-compat -Wno-padded>)

# Turn on common clang/gcc warnings
target_compile_options(frifte_e1n PRIVATE
   -Wall -Wextra -pedantic -Wconversion -Wsign-conversion -Wshadow
   -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused
   -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion
   -Wformat=2 -Wimplicit-fallthrough)

# gcc has some additional useful warnings
target_compile_options(frifte_e1n PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
         -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
         -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion>)

#set_target_properties(frifte_e1n PROPERTIES
#  PUBLIC_HEADER
#       ${PROJECT_SOURCE_DIR}/../../include/frifte/e1n.h)

include(GNUInstallDirs)
install(TARGETS frifte_e1n
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
