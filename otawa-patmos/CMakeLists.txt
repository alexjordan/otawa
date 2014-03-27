cmake_minimum_required(VERSION 2.8)

set(CMAKE_INSTALL_RPATH "\\$ORIGIN/../../")

# otawa dependency
find_program(OTAWA_CONFIG otawa-config DOC "OTAWA configuration file")
if(NOT OTAWA_CONFIG)
	message(FATAL_ERROR "FATAL: OTAWA not found but required !")
endif()
execute_process(COMMAND "${OTAWA_CONFIG}" --cflags OUTPUT_VARIABLE OTAWA_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "${OTAWA_CONFIG}" --libs OUTPUT_VARIABLE OTAWA_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "${OTAWA_CONFIG}" --prefix OUTPUT_VARIABLE OTAWA_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)

# internals
set(ARCH		"patmos")
set(TARGET		"patmos")
set(TARGET_PATH		"${CMAKE_SOURCE_DIR}/../${TARGET}/" CACHE PATH "patmos GLISS2 directory")
set(GLISS_PATH		"${CMAKE_SOURCE_DIR}/../gliss2" CACHE PATH "GLISS v2 source directory")
set(ELF_NUM		"48875")
set(TARGET_LIB		"${TARGET_PATH}/src/lib${TARGET}.a")
set(TARGET_INC		"${TARGET_PATH}/include")
set(TARGET_IRG		"${TARGET_PATH}/${TARGET}.irg")
set(GLISS_ATTR		"${GLISS_PATH}/gep/gliss-attr")
set(OTAWA_KIND 		"${PROJECT_BINARY_DIR}/otawa_kind.h")
set(OTAWA_TARGET 	"${PROJECT_BINARY_DIR}/otawa_target.h")
set(OTAWA_DELAYED 	"${PROJECT_BINARY_DIR}/otawa_delayed.h")
set(OTAWA_USED_REGS     "${PROJECT_BINARY_DIR}/otawa_used_regs.h")
set(OTAWA_SEM		"${PROJECT_BINARY_DIR}/otawa_sem.h")
message(STATUS "GLISS_ATTR = ${GLISS_ATTR}")

set(SOURCES
	"${ARCH}.cpp"
	"${OTAWA_KIND}"
	"${OTAWA_PRED}"
	"${OTAWA_TARGET}"
	"${OTAWA_DELAYED}"
	"${OTAWA_SEM}"
)

# build of GLISS-derived sources
if(NOT MINGW_WIN)
	add_custom_command(
		OUTPUT ${OTAWA_KIND} DEPENDS "kind.tpl" COMMAND ${GLISS_ATTR}
		ARGS ${TARGET_IRG} -o ${OTAWA_KIND} -a otawa_kind -f -t "${CMAKE_SOURCE_DIR}/kind.tpl" -d "return 0\\;" -e ${CMAKE_SOURCE_DIR}/kind.nmp
		DEPENDS ${TARGET_IRG}
	)
	add_custom_command(
		OUTPUT ${OTAWA_TARGET} DEPENDS "target.tpl" COMMAND ${GLISS_ATTR}
		ARGS ${TARGET_IRG} -o ${OTAWA_TARGET} -a otawa_target -f -t "${CMAKE_SOURCE_DIR}/target.tpl" -d "return 0\\;" -e ${CMAKE_SOURCE_DIR}/target.nmp
		DEPENDS ${TARGET_IRG}
	)
	add_custom_command(
		OUTPUT ${OTAWA_DELAYED} DEPENDS "delayed.tpl" COMMAND ${GLISS_ATTR}
		ARGS ${TARGET_IRG} -o ${OTAWA_DELAYED} -a otawa_delayed -f -t "${CMAKE_SOURCE_DIR}/delayed.tpl" -d "return 0\\;" -e ${CMAKE_SOURCE_DIR}/delayed.nmp
		DEPENDS ${TARGET_IRG}
	)
	add_custom_command(
		OUTPUT ${OTAWA_SEM} DEPENDS "sem.tpl" "sem.nmp" COMMAND ${GLISS_ATTR}
		ARGS ${TARGET_IRG} -o ${OTAWA_SEM} -a otawa_sem -p -t "${CMAKE_SOURCE_DIR}/sem.tpl" -d "';'" -e ${CMAKE_SOURCE_DIR}/sem.nmp
		DEPENDS ${TARGET_IRG}
	)
endif()

# build the library
include_directories("${TARGET_INC}")
include_directories("${PROJECT_BINARY_DIR}")
add_library(${ARCH} SHARED ${SOURCES})
set_property(TARGET ${ARCH} PROPERTY PREFIX "")
set_property(TARGET ${ARCH} PROPERTY COMPILE_FLAGS ${OTAWA_CXXFLAGS})
target_link_libraries(${ARCH} "${TARGET_LIB}" "${OTAWA_LDFLAGS} -lgel_dwarf")

# installation
if(NOT PREFIX)
	set(PREFIX "${OTAWA_PREFIX}")
endif()
install(TARGETS ${ARCH} DESTINATION "${PREFIX}/lib/otawa/loader")
if(WIN32)
	file(WRITE elf_${ELF_NUM}.link "${ARCH}.dll")
	install(FILES elf_${ELF_NUM}.link DESTINATION "${PREFIX}/lib/otawa/loader")
elseif(UNIX OR APPLE)
	add_custom_command(OUTPUT "elf_${ELF_NUM}${CMAKE_SHARED_LIBRARY_SUFFIX}" COMMAND rm -rf "elf_${ELF_NUM}${CMAKE_SHARED_LIBRARY_SUFFIX}" COMMAND ln -s "${ARCH}${CMAKE_SHARED_LIBRARY_SUFFIX}" "elf_${ELF_NUM}${CMAKE_SHARED_LIBRARY_SUFFIX}")
	add_custom_target(ELF_LINK ALL DEPENDS "elf_${ELF_NUM}${CMAKE_SHARED_LIBRARY_SUFFIX}")
	install(FILES "${PROJECT_BINARY_DIR}/elf_${ELF_NUM}${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION "${PREFIX}/lib/otawa/loader")
endif()
install(FILES "patmos.h" DESTINATION "${PREFIX}/include/otawa")


