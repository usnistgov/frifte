# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.

cmake_minimum_required(VERSION 3.22)

project(frif_library)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(frif SHARED)
target_sources(frif PRIVATE
    libfrif_common.cpp
    libfrif_efs.cpp
    libfrif_io.cpp)
target_include_directories(frif PRIVATE ${PROJECT_SOURCE_DIR}/../include)

#if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#	set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../validation CACHE PATH "..." FORCE)
#endif()

# Extern the version symbols
target_compile_definitions(frif PRIVATE NIST_EXTERN_FRIF_API_VERSION)

# Enable documentation warnings with clang
target_compile_options(frif PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
          -Wdocumentation>)

# Enable all clang warnings
#target_compile_options(frif PRIVATE
#     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
#          -Weverything -Wno-c++98-compat -Wno-padded>)

# Turn on common clang/gcc warnings
target_compile_options(frif PRIVATE
   -Wall -Wextra -pedantic -Wconversion -Wsign-conversion -Wshadow
   -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused
   -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion
   -Wformat=2 -Wimplicit-fallthrough)

# gcc has some additional useful warnings
target_compile_options(frif PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
         -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
         -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion>)

#set_target_properties(frif PROPERTIES
#     PUBLIC_HEADER ${PROJECT_SOURCE_DIR}/../include/frif/frif.h)

include(GNUInstallDirs)
install(TARGETS frif
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
