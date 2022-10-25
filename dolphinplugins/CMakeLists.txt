find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS KIO)

set(GIT_EXTRA_SOURCE
)

ecm_qt_declare_logging_category(GIT_EXTRA_SOURCE HEADER gitklient_dolphinplugins_debug.h
    IDENTIFIER GIKLIENT_DOLPHIN_PLUGINS_LOG CATEGORY_NAME org.kde.gitklient.dolphinplugins DESCRIPTION "gitklient dolphin plugins" EXPORT GITKLIENT)

#---ACTION PLUGIN---
kcoreaddons_add_plugin(gitklientitemaction
    INSTALL_NAMESPACE kf${QT_MAJOR_VERSION}/kfileitemaction
)
target_sources(gitklientitemaction PRIVATE
    actionmanager.cpp
    statuscache.cpp
    manager.cpp
    actionmanager.h
    statuscache.h
    manager.h
    actions.h
    ${GIT_EXTRA_SOURCE}
)

target_link_libraries(
    gitklientitemaction
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOCore
    KF5::KIOWidgets
    libgitklient
)

#---OVERLAY PLUGIN---
kcoreaddons_add_plugin(gitklientoverlayplugin
    INSTALL_NAMESPACE kf${QT_MAJOR_VERSION}/overlayicon
)

target_sources(gitklientoverlayplugin PRIVATE
    overlayplugin.cpp
    statuscache.cpp
    manager.cpp
    overlayplugin.h
    statuscache.h
    manager.h
    ${GIT_EXTRA_SOURCE}
)


target_link_libraries(
    gitklientoverlayplugin
    KF5::CoreAddons
    KF5::KIOCore
    KF5::KIOWidgets
    libgitklient
)
