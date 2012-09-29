#
# CMakeLists.txt
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
#

find_package(Boost 1.42 REQUIRED system unit_test_framework test_exec_monitor)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

add_custom_target(bench)

add_custom_target(cleanbench
	COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/bench.log
	VERBATIM
	)

add_custom_target(plotbench
	${CMAKE_CURRENT_SOURCE_DIR}/plotbench.py ${CMAKE_CURRENT_BINARY_DIR}/bench.log
	)

macro(sandbox_add_bench src)
	get_filename_component(name ${src} NAME_WE)
	set(name "bench_${name}")

	if("${ARGN}" STREQUAL "GCC_ONLY" AND NOT CMAKE_COMPILER_IS_GNUCXX)
		set(IGNORE_REASON "gcc only")
	elseif("${ARGN}" STREQUAL "CLANG_ONLY" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set(IGNORE_REASON "clang only")
	endif()
	if (IGNORE_REASON)
		message("Ignoring benchmark \"${name}\" (${src}) because it compiles with " ${IGNORE_REASON})
	else()
		add_executable(${name} ${src})
		target_link_libraries(${name} rt)
		set(runname "${name}.run")
		add_custom_target(${runname}
			echo "Benchmarking ${name}..."
			COMMAND ${name}
			VERBATIM
			)
		add_dependencies(bench ${runname})
	endif()
endmacro()

sandbox_add_bench(property.cpp CLANG_ONLY)
