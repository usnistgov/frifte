# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.

cmake_minimum_required(VERSION 3.22)

project(frif_e1n_library)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(frif_e1n SHARED)
target_sources(frif_e1n PRIVATE
    libfrif_e1n.cpp)
target_include_directories(frif_e1n PRIVATE ${PROJECT_SOURCE_DIR}/../../include)

# Extern the version symbols
target_compile_definitions(frif_e1n PRIVATE NIST_EXTERN_FRIF_E1N_API_VERSION)

# Require libfrif
add_subdirectory(${PROJECT_SOURCE_DIR}/../../libfrif ${CMAKE_CURRENT_BINARY_DIR}/libfrif)
add_dependencies(frif_e1n frif)
target_link_libraries(frif_e1n PUBLIC frif)

# Set RPATH to $ORIGIN
set_target_properties(frif_e1n PROPERTIES
    BUILD_RPATH_USE_ORIGIN TRUE)
if(UNIX)
	set_target_properties(frif_e1n PROPERTIES
	    INSTALL_RPATH $ORIGIN)
endif()

# Use RUNPATH over RPATH
if(NOT APPLE)
	set_target_properties(frif_e1n PROPERTIES
	    LINK_FLAGS -Wl,--enable-new-dtags)
endif()

# Enable documentation warnings with clang
target_compile_options(frif_e1n PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
          -Wdocumentation>)

# Enable all clang warnings
#target_compile_options(frif_e1n PRIVATE
#     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
#          -Weverything -Wno-c++98-compat -Wno-padded>)

# Turn on common clang/gcc warnings
target_compile_options(frif_e1n PRIVATE
   -Wall -Wextra -pedantic -Wconversion -Wsign-conversion -Wshadow
   -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused
   -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion
   -Wformat=2 -Wimplicit-fallthrough)

# gcc has some additional useful warnings
target_compile_options(frif_e1n PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
         -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
         -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion>)

#set_target_properties(frif_e1n PROPERTIES
#  PUBLIC_HEADER
#       ${PROJECT_SOURCE_DIR}/../../include/frif/e1n.h)

include(GNUInstallDirs)
install(TARGETS frif_e1n
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
