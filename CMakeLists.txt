cmake_minimum_required(VERSION 3.24)
include(${CMAKE_CURRENT_BINARY_DIR}/local_conf.cmake OPTIONAL)

include(FetchContent)
FetchContent_Declare(
  ccommon
  GIT_REPOSITORY https://github.com/EddyTheCo/Common.git
  GIT_TAG v0.4.0)
FetchContent_MakeAvailable(ccommon)

version_from_git(LOG OFF TIMESTAMP "%Y%m%d%H%M%S")

project(
  QtIotaPow
  VERSION ${VERSION}
  DESCRIPTION "Methods for doing Pow on IOTA"
  LANGUAGES CXX)

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
  GIT_TAG v0.1.1
  FIND_PACKAGE_ARGS 0.1 CONFIG)
FetchContent_MakeAvailable(Qtb1t6)
FetchContent_Declare(
  Qtcurlp81
  GIT_REPOSITORY https://github.com/EddyTheCo/Qcurlp81.git
  GIT_TAG v0.1.1
  FIND_PACKAGE_ARGS 0.1 CONFIG)
FetchContent_MakeAvailable(Qtcurlp81)

find_package(Qt6 COMPONENTS Core)

if(Qt6_FOUND)
  qt_standard_project_setup()

  add_library(qpow include/pow/qpow.hpp qpow.cpp)
  add_library(${PROJECT_NAME}::qpow ALIAS qpow)
  set_target_properties(qpow PROPERTIES VERSION ${VERSION} SOVERSION
                                                           ${VERSION_MAJOR})
  target_compile_definitions(qpow PRIVATE WINDOWS_QPOW)

  target_link_libraries(qpow PUBLIC Qt6::Core)

  if(USE_THREADS)
    target_compile_definitions(qpow PRIVATE USE_THREADS)
  endif(USE_THREADS)

  target_link_libraries(qpow PRIVATE Qtb1t6::qb1t6 Qtcurlp81::qcurlp81)

  target_include_directories(
    qpow PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Esterv/>")

  install(
    TARGETS qpow
    EXPORT ${PROJECT_NAME}-config
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT pow
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT pow)

  install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Esterv
    COMPONENT ${PROJECT_NAME})

  build_exports()
  if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    build_cpack()
  endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

endif(Qt6_FOUND)

if(BUILD_DOCS)
  build_docs()
endif(BUILD_DOCS)
