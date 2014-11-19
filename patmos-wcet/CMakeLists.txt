# To use this template, complete the configuration section.
# This file must be saved with name "CMakeLists.txt".
#
# The created plugin will be named ${SCRIPT}.so and the contained classes
# must be put in the C++ namespace ${NAMESPACE}::${SCRIPT}. The installed
# OSX file must be called ${SCRIPT}.osx and put in the same directory
# than this file.
#
# Script files like pipeline, cache and memory description must be set
# in the FILES variable.
#
# Sources must be set in the SOURCES variable.
#
# To compile it, otawa-config must in the path:
#	cmake .
# Or passed explicetely:
#	cmake . -DOTAWA_CONFIG=path/to/otawa-config

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# configuration
set(SCRIPT      "patmos_wcet")	# machine model
set(NAMESPACE   "tcrest")	# namespace (classes must be contained in the namespace otawa::model)
set(FILES	memory.xml	# cache, pipeline, memory
	      	pipeline.xml
	      	caches.xml)
set(MODULES	)		# used modules (to pass to otawa-config=
set(SOURCES 	hook.cpp	# sources of the plugin
		BBTimer.cpp
		MethodCacheContributer.cpp
		)		



# script
project(${SCRIPT})


# look for OTAWA
if(NOT OTAWA_CONFIG)
	find_program(OTAWA_CONFIG otawa-config DOC "path to otawa-config")
	if(NOT OTAWA_CONFIG)
		message(FATAL_ERROR "ERROR: otawa-config is required !")
	endif()
endif()
message(STATUS "otawa-config at ${OTAWA_CONFIG}")
execute_process(COMMAND "${OTAWA_CONFIG}" --cflags ${MODULES} OUTPUT_VARIABLE OTAWA_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "${OTAWA_CONFIG}" --libs ${MODULES}  OUTPUT_VARIABLE OTAWA_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "${OTAWA_CONFIG}" --prefix OUTPUT_VARIABLE OTAWA_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)


# plugin definition
set(CMAKE_INSTALL_RPATH "\\$ORIGIN/../../../:\\$ORIGIN/../../loader")
include_directories("${CMAKE_SOURCE_DIR}" ".")
# depend on otawa et al. headers to trigger rebuild of plugin
include_directories("${OTAWA_PREFIX}/include")
add_library(${SCRIPT} SHARED ${SOURCES})
set_property(TARGET ${SCRIPT} PROPERTY PREFIX "")
set_property(TARGET ${SCRIPT} PROPERTY COMPILE_FLAGS "${OTAWA_CFLAGS}")
target_link_libraries(${SCRIPT} "${OTAWA_LDFLAGS} ${CMAKE_SOURCE_DIR}/../build/otawa-patmos/patmos.so")

# installation
if(NOT PREFIX)
	set(PREFIX "${OTAWA_PREFIX}")
endif()
set(MODULE_PATH "${PREFIX}/lib/otawa/proc/${NAMESPACE}")
set(SCRIPT_PATH "${PREFIX}/share/Otawa/scripts/")

if(WIN32 OR WIN64)
	install(TARGETS ${SCRIPT} RUNTIME DESTINATION ${MODULE_PATH})
else()
	install(TARGETS ${SCRIPT} LIBRARY DESTINATION ${MODULE_PATH})
endif()
install(FILES ${SCRIPT}.osx DESTINATION ${SCRIPT_PATH})
install(FILES ${SCRIPT}_dcache.osx DESTINATION ${SCRIPT_PATH})
foreach(FILE ${FILES})
	install(FILES ${SCRIPT}/${FILE} DESTINATION ${SCRIPT_PATH}/${SCRIPT})
endforeach()

