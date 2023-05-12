
find_package(Qt6 QUIET COMPONENTS Core)


if(NOT Qt6_FOUND AND NOT TARGET Qt6::Core)

set(QT_BUILD_TESTS OFF PARENT_SCOPE)
set(QT_BUILD_EXAMPLES OFF PARENT_SCOPE)
set(QT_FEATURE_sql OFF PARENT_SCOPE)
set(QT_FEATURE_network OFF PARENT_SCOPE)
set(QT_FEATURE_gui OFF PARENT_SCOPE)
#set(QT_FEATURE_regularexpression OFF PARENT_SCOPE)
set(QT_FEATURE_concurrent OFF PARENT_SCOPE)
set(QT_FEATURE_xml OFF PARENT_SCOPE)
set(QT_FEATURE_dbus OFF PARENT_SCOPE)
set(QT_FEATURE_opengl OFF PARENT_SCOPE)
set(QT_FEATURE_widgets OFF PARENT_SCOPE)
set(QT_FEATURE_testlib OFF PARENT_SCOPE)
set(QT_FEATURE_printsupport OFF PARENT_SCOPE)
#set(QT_FEATURE_commandlineparser OFF PARENT_SCOPE)
set(QT_FEATURE_androiddeployqt OFF PARENT_SCOPE)
set(QT_FEATURE_qmake OFF PARENT_SCOPE)
set(QT_SUPERBUILD OFF PARENT_SCOPE)
set(ANDROID OFF PARENT_SCOPE)
set(WASM OFF PARENT_SCOPE)
#set(QT_WILL_BUILD_TOOLS OFF PARENT_SCOPE)

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
