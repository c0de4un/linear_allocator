# // ------------------------------------------------------------------------------ \\
#
# This is CMake-File for 'libECS' Project.
#
# About:
# "linear_allocator" - sample project to demonstrate usage of custom memory manager (allocation & deallocation)
# using C++ 11.
#
# License: see "LICENSE" file.
# API: C++ 11.
# Platforms: Any with C++ 11 support.
#
# // ------------------------------------------------------------------------------ \\

# =================================================================================
# CMake Meta-Data
# =================================================================================

# CMake-Version
cmake_minimum_required ( VERSION 3.8 FATAL_ERROR)

# =================================================================================
# PROJECT
# =================================================================================

# Project Name
set ( ROOT_PROJECT_NAME "linear_allocator" )

# Project Version
set ( ROOT_PROJECT_VERSION 1.0.0 )

# Configure Project
project ( ${ROOT_PROJECT_NAME} VERSION ${ROOT_PROJECT_VERSION} LANGUAGES C CXX )

# =================================================================================
# CONFIG
# =================================================================================

# DETECT LINUX
if ( UNIX AND NOT DEFINED LINUX AND NOT APPLE )
		
	# LINUX ON
	set ( LINUX ON )
	
endif ( UNIX AND NOT DEFINED LINUX AND NOT APPLE )

# PLATFORM
if ( ANDROID ) # ANDROID

	# Set Directory Prefix
	set ( PLATFORM_DIR "android" )
	
elseif ( WIN32 ) # WINDOWS

	# Set Directory Prefix
	set ( PLATFORM_DIR "win32" )

elseif ( LINUX ) # LINUX
	
	# Set Directory Prefix
	set ( PLATFORM_DIR "linux" )

elseif ( APPLE ) # APPLE

	# Set Directory Prefix
	set ( PLATFORM_DIR "apple" )

else ( ANDROID ) # ?
	
	# ERROR
	message ( SEND_ERROR "${ROOT_PROJECT_NAME} - unknown platform ! Configuration required." )
	
	# Set Directory Prefix
	set ( PLATFORM_DIR "unix" )
	
endif ( ANDROID )

# Build-Type Sub-Dir
if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

	# Add Definition
	add_definitions ( -DDEBUG=1 )
	
	# Set Build-Type Dir
	set ( BUILD_TYPE_DIR "debug" )
	
else ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

	# Set Build-Type Dir
	set ( BUILD_TYPE_DIR "release" )
	
endif ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

# =================================================================================
# CONSTANTS
# =================================================================================

# Set Sources Dir
set ( SOURCES_DIR "${CMAKE_SOURCE_DIR}/src/cpp" )

# Set Project Bin-Output Dir
set ( ROOT_PROJECT_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/${BUILD_TYPE_DIR}/${PLATFORM_DIR}" )

# Sample-Project Multithreading
set ( ROOT_PROJECT_MULTITHREADING_ENABLED ON )
add_definitions ( -D_C0DE4UN_MULTITHREADING_ENABLED_ )

# =================================================================================
# HEADERS
# =================================================================================

# Sample-Project Headers
set ( ROOT_PROJECT_HEADERS "${SOURCES_DIR}/linear_allocator.hpp" )

# =================================================================================
# SOURCES
# =================================================================================

# Sample-Project Sources
set ( ROOT_PROJECT_SOURCES "${SOURCES_DIR}/main.cpp" )

# =================================================================================
# BUILD EXECUTABLE
# =================================================================================

# Create Executable Object
add_executable ( linear_allocator ${ROOT_PROJECT_SOURCES} ${ROOT_PROJECT_HEADERS} )

# Configure Executable Object
set_target_properties ( linear_allocator PROPERTIES
CXX_STANDARD 17
CXX_STANDARD_REQUIRED YES
CXX_EXTENSIONS NO
OUTPUT_NAME ${ROOT_PROJECT_NAME}
RUNTIME_OUTPUT_DIRECTORY ${ROOT_PROJECT_OUTPUT_DIR} )

# Request features
target_compile_features ( linear_allocator PUBLIC cxx_std_17 )