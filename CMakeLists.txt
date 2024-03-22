cmake_minimum_required(VERSION 3.24)
include(${CMAKE_CURRENT_BINARY_DIR}/local_conf.cmake OPTIONAL)

include(FetchContent)
FetchContent_Declare(
	ccommon
	GIT_REPOSITORY https://github.com/EddyTheCo/Common.git
	GIT_TAG use_IFW
	)
FetchContent_MakeAvailable(ccommon)

version_from_git(
	LOG       OFF
	TIMESTAMP "%Y%m%d%H%M%S"
	)

project(QtIotaPow VERSION ${VERSION} DESCRIPTION "Methods for doing Pow on IOTA" LANGUAGES CXX)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(GNUInstallDirs)

option(USE_THREADS "On or off" OFF)
message(STATUS "Use multithread: '${USE_THREADS}' ")


FetchContent_Declare(
	Qtb1t6
	GIT_REPOSITORY https://github.com/EddyTheCo/Qb1t6.git
	GIT_TAG use_IFW
	FIND_PACKAGE_ARGS 0.0 CONFIG
	)
FetchContent_MakeAvailable(Qb1t6)
FetchContent_Declare(
	Qtcurlp81
	GIT_REPOSITORY https://github.com/EddyTheCo/Qcurlp81.git
	GIT_TAG use_IFW
	FIND_PACKAGE_ARGS 0.0 CONFIG
	)
FetchContent_MakeAvailable(Qtcurlp81)

find_package(Qt6 COMPONENTS Core)

if (Qt6_FOUND)
	qt_standard_project_setup()
	
	add_library(qpow include/pow/qpow.hpp qpow.cpp)
	add_library(${PROJECT_NAME}::qpow ALIAS qpow)
	set_target_properties(qpow PROPERTIES VERSION ${VERSION} SOVERSION ${VERSION_MAJOR})
	target_compile_definitions(qpow PRIVATE WINDOWS_QPOW)

	target_link_libraries(qpow PUBLIC  Qt6::Core )

	if(USE_THREADS)
		target_compile_definitions(qpow PRIVATE USE_THREADS)
	endif(USE_THREADS)


	target_link_libraries(qpow PRIVATE  Qtb1t6::qb1t6 Qtcurlp81::qcurlp81 )

	target_include_directories(qpow PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Esterv/>")

	install(TARGETS qpow 
		EXPORT ${PROJECT_NAME}-config
	      	DESTINATION ${CMAKE_INSTALL_LIBDIR}
		COMPONENT pow
		)

	install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/ 
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Esterv
		COMPONENT pow
		)

	install(EXPORT ${PROJECT_NAME}-config
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
		NAMESPACE ${PROJECT_NAME}::
		COMPONENT ${PROJECT_NAME}
		)
	include(CMakePackageConfigHelpers)
	configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
		"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
		INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
		)
	write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
		VERSION ${VERSION}
		COMPATIBILITY SameMajorVersion
		)
	install(FILES ${CMAKE_SOURCE_DIR}/LICENSE
		DESTINATION ${CMAKE_INSTALL_DATADIR}/Esterv/${PROJECT_NAME}
		COMPONENT pow
		)
	install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
		COMPONENT pow 
		)
	export(EXPORT ${PROJECT_NAME}-config
		FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
		)
	if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
		set(cpack_file_name "${PROJECT_NAME}-v${SEMVER}-${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
		if(NOT BUILD_SHARED_LIBS)
			set(cpack_file_name "${cpack_file_name}-static")
		endif(NOT BUILD_SHARED_LIBS)
		if(USE_THREADS)
			set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-MThread")
		endif(USE_THREADS)
		set(CPACK_PACKAGE_FILE_NAME ${cpack_file_name})
		include(CPack)
		include(CPackIFW)
		cpack_add_component(pow)
		cpack_ifw_configure_component(pow LICENSES "License" ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
		cpack_ifw_add_repository("Repo" URL "${REPO_URL}/packages/${CMAKE_SYSTEM_NAME}/" )
	endif()

endif(Qt6_FOUND)

if(BUILD_DOCS)
	build_docs()
endif()
