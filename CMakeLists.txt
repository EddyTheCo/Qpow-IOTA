cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
include(local_conf.cmake OPTIONAL)
project(QtIotaPow VERSION 0.1 DESCRIPTION "QT   library for doing Pow on IOTA network " LANGUAGES CXX)


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


add_library(qpow include/pow/qpow.hpp qpow.cpp)

find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(qpow PUBLIC  Qt6::Core )

include(FetchContent)
FetchContent_Declare(
        qb1t6
        GIT_REPOSITORY git@github.com:EddyTheCo/Qb1t6.git
        GIT_TAG main
        )
FetchContent_MakeAvailable(qb1t6)

target_link_libraries(qpow PRIVATE  qb1t6 )
FetchContent_Declare(
        qcurlp81
        GIT_REPOSITORY git@github.com:EddyTheCo/Qcurlp81.git
        GIT_TAG main
        )
FetchContent_MakeAvailable(qcurlp81)

target_link_libraries(qpow PRIVATE  qcurlp81 )

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
	VERSION "0.1.1.0"
	COMPATIBILITY AnyNewerVersion
	)
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/qpowConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/qpowConfigVersion.cmake
	DESTINATION lib/cmake/qpow
	)
export(EXPORT qpowTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/qpowTargets.cmake"
	)

