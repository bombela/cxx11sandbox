#
# CMakeLists.txt
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
#

find_package(Boost 1.42 REQUIRED system unit_test_framework test_exec_monitor)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

macro(sandbox_add_test src)
	get_filename_component(name ${src} NAME_WE)
	set(name "test_${name}")

	if("${ARGN}" STREQUAL "GCC_ONLY" AND NOT CMAKE_COMPILER_IS_GNUCXX)
		set(IGNORE_REASON "gcc only")
	elseif("${ARGN}" STREQUAL "CLANG_ONLY" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set(IGNORE_REASON "clang only")
	endif()
	if (IGNORE_REASON)
		message("Ignoring test \"${name}\" (${src}) because it compiles with " ${IGNORE_REASON})
	else()
		add_executable(${name} ${src})
		add_test(${name} ${name})
	endif()
endmacro()

sandbox_add_test(range.cpp)
sandbox_add_test(property.cpp CLANG_ONLY)
sandbox_add_test(algo.cpp CLANG_ONLY)
sandbox_add_test(lambda.cpp CLANG_ONLY)
