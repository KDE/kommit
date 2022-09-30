find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS KIO)
include_directories(../src ../src/git)
include(../src/git/CMakeLists.txt)



set(GIT_EXTRA_SOURCE
    ../src/git/filestatus.cpp
    ../src/git/gitglobal.cpp
)

ecm_qt_declare_logging_category(GIT_EXTRA_SOURCE HEADER gitklient_dolphinplugins_debug.h
    IDENTIFIER GIKLIENT_DOLPHIN_PLUGINS_LOG CATEGORY_NAME org.kde.gitklient.dolphinplugins DESCRIPTION "gitklient dolphin plugins" EXPORT GITKLIENT)

#---ACTION PLUGIN---
set(ACTIONS_PLUGIN gitklientitemaction)

set(
    ACTIONS_PLUGIN_SRC
    actionmanager.cpp
    statuscache.cpp
    manager.cpp
    actionmanager.h
    statuscache.h
    manager.h
    ${GIT_EXTRA_SOURCE}
)
kcoreaddons_add_plugin(
    ${ACTIONS_PLUGIN}
    SOURCES ${ACTIONS_PLUGIN_SRC}
    INSTALL_NAMESPACE kf${QT_MAJOR_VERSION}/kfileitemaction
    JSON ${ACTIONS_PLUGIN}.json
)

target_link_libraries(
    ${ACTIONS_PLUGIN}
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOCore
    KF5::KIOWidgets
)

#---OVERLAY PLUGIN---
set(OVERLAY_PLUGIN gitklientoverlayplugin)
set(
    OVERLAY_SRC
    overlayplugin.cpp
    statuscache.cpp
    manager.cpp
    overlayplugin.h
    statuscache.h
    manager.h
    ${GIT_EXTRA_SOURCE}
)

kcoreaddons_add_plugin(
    ${OVERLAY_PLUGIN}
    SOURCES ${OVERLAY_SRC}
    INSTALL_NAMESPACE kf${QT_MAJOR_VERSION}/overlayicon
    JSON ${OVERLAY_PLUGIN}.json
)

target_link_libraries(
    ${OVERLAY_PLUGIN}
    KF5::CoreAddons
    KF5::KIOCore
    KF5::KIOWidgets
)
