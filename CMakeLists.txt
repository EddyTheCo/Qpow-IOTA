cmake_minimum_required(VERSION 3.24)
include(local_conf.cmake OPTIONAL)

include(FetchContent)
FetchContent_Declare(
	ccommon
	GIT_REPOSITORY https://github.com/EddyTheCo/Common.git
	GIT_TAG main
	)
FetchContent_MakeAvailable(ccommon)

version_from_git(
	LOG       OFF
	TIMESTAMP "%Y%m%d%H%M%S"
	)

project(QtIotaPow VERSION ${VERSION} DESCRIPTION "QT library for doing Pow on IOTA" LANGUAGES CXX)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(GNUInstallDirs)

find_package(Qt6 COMPONENTS Core)
if (Qt6_FOUND)
	qt_standard_project_setup()
endif(Qt6_FOUND)

option(USE_THREADS "On or off" OFF)
message(STATUS "Use multithread: '${USE_THREADS}' ")
add_library(qpow include/pow/qpow.hpp qpow.cpp)
target_compile_definitions(qpow PRIVATE WINDOWS_QPOW)

FetchContent_Declare(
	qb1t6
	GIT_REPOSITORY https://github.com/EddyTheCo/Qb1t6.git
	GIT_TAG v0.0.2
	FIND_PACKAGE_ARGS 0.0 CONFIG
	)
FetchContent_MakeAvailable(qb1t6)
FetchContent_Declare(
	qcurlp81
	GIT_REPOSITORY https://github.com/EddyTheCo/Qcurlp81.git
	GIT_TAG v0.0.2
	FIND_PACKAGE_ARGS 0.0 CONFIG
	)
FetchContent_MakeAvailable(qcurlp81)

if (Qt6_FOUND)
	target_link_libraries(qpow PUBLIC  Qt6::Core )
endif(Qt6_FOUND)

if(USE_THREADS)
	target_compile_definitions(qpow PRIVATE USE_THREADS)
endif(USE_THREADS)


target_link_libraries(qpow PRIVATE  qb1t6 qcurlp81 )

target_include_directories(qpow PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/qpow>")

install(TARGETS qpow EXPORT qpowTargets DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qpow)

install(EXPORT qpowTargets
	FILE qpowTargets.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/qpow
	)
include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/qpowConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/qpow"
	)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/qpowConfigVersion.cmake"
	VERSION ${VERSION}
	COMPATIBILITY SameMajorVersion
	)
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/qpowConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/qpowConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/qpow
	)
export(EXPORT qpowTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/qpowTargets.cmake"
	)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CPACK_PACKAGE_CONTACT "estervtech")
	include(CTest)
	set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-v${SEMVER}-${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
	if(NOT BUILD_SHARED_LIBS)
		set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-static")
	endif(NOT BUILD_SHARED_LIBS)
	if(USE_THREADS)
		set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-MThread")
	endif(USE_THREADS)
	include(CPack)
endif()

if(BUILD_DOCS)
	get_target_property(build_docs cmake_build_docs SOURCES)
	include(${build_docs})
endif()
