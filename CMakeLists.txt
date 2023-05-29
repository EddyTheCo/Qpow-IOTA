cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
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


set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
		STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(Qt6 COMPONENTS Core)
if (Qt6_FOUND)
	qt_standard_project_setup()
endif(Qt6_FOUND)

option(USE_THREADS "On or off" OFF)
add_library(qpow include/pow/qpow.hpp qpow.cpp)

if(BUILD_SHARED_LIBS)
	$target_compile_definitions(qpow PRIVATE "$<BUILD_INTERFACE:QEXPORT_IMPORT=Q_DECL_EXPORT>"
         "$<INSTALL_INTERFACE:QEXPORT_IMPORT=Q_DECL_IMPORT>")
endif(BUILD_SHARED_LIBS)

find_package(qb1t6 0 QUIET )
if(NOT qb1t6_FOUND AND NOT TARGET qb1t6)
	FetchContent_Declare(
		qb1t6
		GIT_REPOSITORY https://github.com/EddyTheCo/Qb1t6.git
		GIT_TAG v0.0.1
		)
	FetchContent_MakeAvailable(qb1t6)
endif(NOT qb1t6_FOUND AND NOT TARGET qb1t6)
find_package(qcurlp81 0 QUIET )
if(NOT qcurlp81_FOUND AND NOT TARGET qcurlp81)
	FetchContent_Declare(
		qcurlp81
		GIT_REPOSITORY https://github.com/EddyTheCo/Qcurlp81.git
		GIT_TAG v0.0.1
		)
	FetchContent_MakeAvailable(qcurlp81)
endif(NOT qcurlp81_FOUND AND NOT TARGET qcurlp81)

if (Qt6_FOUND)
	target_link_libraries(qpow PUBLIC  Qt6::Core )
endif(Qt6_FOUND)

if(USE_THREADS)
	target_compile_definitions(qpow PRIVATE USE_THREADS)
endif(USE_THREADS)


target_link_libraries(qpow PRIVATE  qb1t6 qcurlp81 )

target_include_directories(qpow PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	"$<INSTALL_INTERFACE:include>")

install(TARGETS qpow EXPORT qpowTargets DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/)

install(EXPORT qpowTargets
	FILE qpowTargets.cmake
	DESTINATION lib/cmake/qpow
	)
include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/qpowConfig.cmake"
	INSTALL_DESTINATION "lib/cmake/qpow"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
	)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/qpowConfigVersion.cmake"
	VERSION ${VERSION}
	COMPATIBILITY SameMajorVersion
	)
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/qpowConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/qpowConfigVersion.cmake
	DESTINATION lib/cmake/qpow
	)
export(EXPORT qpowTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/qpowTargets.cmake"
	)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
	set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-v${SEMVER}-${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
	if(NOT BUILD_SHARED_LIBS)
		set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-static")
	endif(NOT BUILD_SHARED_LIBS)
	include(CPack)
endif()

if(BUILD_DOCS)
	get_target_property(build_docs cmake_build_docs SOURCES)
	include(${build_docs})
endif()
