find_package(KF${KF_MAJOR_VERSION} ${KF${KF_MAJOR_VERSION}_MIN_VERSION} REQUIRED COMPONENTS KIO)

set(GIT_EXTRA_SOURCE
)

ecm_qt_declare_logging_category(GIT_EXTRA_SOURCE HEADER kommit_dolphinplugins_debug.h
    IDENTIFIER KOMMIT_DOLPHIN_PLUGINS_LOG CATEGORY_NAME org.kde.kommit.dolphinplugins DESCRIPTION "kommit dolphin plugins" EXPORT kommit)

#---ACTION PLUGIN---
kcoreaddons_add_plugin(kommititemaction
    INSTALL_NAMESPACE kf${QT_MAJOR_VERSION}/kfileitemaction
)
target_sources(kommititemaction PRIVATE
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
    kommititemaction
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    KF${KF_MAJOR_VERSION}::KIOCore
    KF${KF_MAJOR_VERSION}::KIOWidgets
    libkommit
)

#---OVERLAY PLUGIN---
kcoreaddons_add_plugin(kommitoverlayplugin
    INSTALL_NAMESPACE kf${QT_MAJOR_VERSION}/overlayicon
)

target_sources(kommitoverlayplugin PRIVATE
    overlayplugin.cpp
    statuscache.cpp
    manager.cpp
    overlayplugin.h
    statuscache.h
    manager.h
    ${GIT_EXTRA_SOURCE}
)


target_link_libraries(
    kommitoverlayplugin
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::KIOCore
    KF${KF_MAJOR_VERSION}::KIOWidgets
    libkommit
)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
