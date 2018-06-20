################################################################################
#
# Header
#
################################################################################
# Set CMake minimum version
cmake_minimum_required(VERSION 3.9)

# Describe project
project(memalloc VERSION 1.0.0 DESCRIPTION "Memalloc library.")
set (CMAKE_CXX_STANDARD 14)

################################################################################
#
# Build libraries
#
################################################################################
# Create a variable with all source files files
AUX_SOURCE_DIRECTORY(src libsrc)

# This is the "object library" target: compiles the sources only once
add_library(objlib OBJECT ${libsrc})
target_include_directories(objlib PRIVATE .) # Set main dir as search path

# Shared libraries need PIC
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

# Add dynamic library
add_library(memalloc SHARED $<TARGET_OBJECTS:objlib>)
set_target_properties(memalloc PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(memalloc PROPERTIES SOVERSION 1)
set_target_properties(memalloc PROPERTIES PUBLIC_HEADER 
"\
include/memalloc.hpp;\
include/allocatable.hpp;\
include/allocate_smart.hpp;\
include/allocator.hpp;\
include/allocator_traits.hpp;\
include/mempool_allocator.hpp;\
include/mempool_alloc_policy.hpp;\
include/mempool.hpp;\
include/object_traits.hpp;\
include/standard_alloc_policy.hpp;\
include/timer.hpp\
")

# Add static library 
add_library(memalloc_static STATIC $<TARGET_OBJECTS:objlib>)
set_target_properties(memalloc_static PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(memalloc_static PROPERTIES SOVERSION 1)

################################################################################
#
# Install setup
#
################################################################################
# Install part
include(GNUInstallDirs)

install(TARGETS memalloc
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

install(TARGETS memalloc_static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
