cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
include(local_conf.cmake OPTIONAL)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(cmake/VersionFromGit.cmake)

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
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 QUIET COMPONENTS Core)


if(NOT Qt6_FOUND AND NOT TARGET Qt6::Core)

set(QT_BUILD_TESTS OFF)
set(QT_BUILD_EXAMPLES OFF)
set(QT_FEATURE_sql OFF)
set(QT_FEATURE_network OFF)
set(QT_FEATURE_gui OFF)
#set(QT_FEATURE_regularexpression OFF)
set(QT_FEATURE_concurrent OFF)
set(QT_FEATURE_xml OFF)
set(QT_FEATURE_dbus OFF)
set(QT_FEATURE_opengl OFF)
set(QT_FEATURE_widgets OFF)
set(QT_FEATURE_testlib OFF)
set(QT_FEATURE_printsupport OFF)
#set(QT_FEATURE_commandlineparser OFF)
set(QT_FEATURE_androiddeployqt OFF)
set(QT_FEATURE_qmake OFF)
set(QT_SUPERBUILD OFF)
set(ANDROID OFF)
set(WASM OFF)
#set(QT_WILL_BUILD_TOOLS OFF)

include(FetchContent)
FetchContent_Declare(
        qtbase
        GIT_REPOSITORY git://code.qt.io/qt/qtbase.git
        GIT_TAG 6.5.0 
        )
FetchContent_GetProperties(qtbase)
if(NOT qtbase_POPULATED)
  FetchContent_Populate(qtbase)
  add_subdirectory(${qtbase_SOURCE_DIR} ${qtbase_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
endif(NOT Qt6_FOUND AND NOT TARGET Qt6::Core)
set(CMAKE_AUTOMOC ON)

option(USE_THREADS "On or off" OFF)
add_library(qpow include/pow/qpow.hpp qpow.cpp)

include(FetchContent)
if(NOT qb1t6_FOUND AND NOT TARGET qb1t6)

FetchContent_Declare(
        qb1t6
        GIT_REPOSITORY https://github.com/EddyTheCo/Qb1t6.git
        GIT_TAG v0.0.1
        )
FetchContent_MakeAvailable(qb1t6)
endif(NOT qb1t6_FOUND AND NOT TARGET qb1t6)
if(NOT qcurlp81_FOUND AND NOT TARGET qcurlp81)

FetchContent_Declare(
        qcurlp81
        GIT_REPOSITORY https://github.com/EddyTheCo/Qcurlp81.git
        GIT_TAG v0.0.1
        )
FetchContent_MakeAvailable(qcurlp81)
endif(NOT qcurlp81_FOUND AND NOT TARGET qcurlp81)



target_link_libraries(qpow PUBLIC  Qt6::Core )

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

