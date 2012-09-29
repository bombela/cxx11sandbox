#
# CMakeLists.txt
# Copyright © 2012 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
#

cmake_minimum_required(VERSION 2.8)
project(sandbox CXX)

###############################################################################
# COMPILER FLAGS
###############################################################################

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

	# You do not really want to disable that, but sometimes it is usefull when debugging.
	set(PROFILING false CACHE BOOL "Enable profiling")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")

	set(PROFILING false CACHE BOOL "Enable profiling")
	if (PROFILING)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
	endif()

	set(FORCEDBGINFO false CACHE BOOL "Force debug info (even in release mode)")
	if (FORCEDBGINFO)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
	endif()
	
	set(NATIVECODE false CACHE BOOL "Generate native code (on the current host)")
	if (NATIVECODE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	endif()

	set(NOELIDECTOR false CACHE BOOL "Disable elide contructors optimization (debug)")
	if (NOELIDECTOR)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-elide-constructors")
	endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#	set(OMIT_FRAME_POINTER true CACHE BOOL "Omit frame pointer")
#	if (OMIT_FRAME_POINTER)
#		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer")
#	endif()

	# ugly temparary hack
	#include_directories(
	#	"/usr/include/c++/4.6/"
	#	"/usr/include/c++/4.6/x86_64-linux-gnu/"
	#	"/usr/include/x86_64-linux-gnu/"
	#	"/usr/include/i386-linux-gnu/"
	#	)
endif()

###############################################################################
# GLOBAL BOOST CONFIGURATION
###############################################################################

set(Boost_USE_STATIC_LIBS   OFF)
set(Boost_USE_MULTITHREADED ON)
set(BOOST_INCLUDEDIR "/usr/local/include")
set(Boost_ADDITIONAL_VERSIONS 1.51 1.50 1.49 1.48 1.47 1.46 1.44 1.43 1.42)
add_definitions(-DBOOST_ALL_NO_LIB)

###############################################################################
# RANDOM TWEAKS for debugging purposes
###############################################################################

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SANDBOX_COROUTINE_LINUX_2SWAPSITE true CACHE BOOL
	"Coroutine, Linux x86_64, duplicated code for context switching that supposedly help the CPU prediction")
if (NOT SANDBOX_COROUTINE_LINUX_2SWAPSITE)
	add_definitions(-DSANDBOX_COROUTINE_LINUX_2SWAPSITE=0)
endif()

###############################################################################

enable_testing()
add_subdirectory(test)
add_subdirectory(bench)
