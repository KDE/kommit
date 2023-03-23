find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS KIO)

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
    KF5::I18n
    KF5::WidgetsAddons
    KF5::KIOCore
    KF5::KIOWidgets
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
    KF5::CoreAddons
    KF5::KIOCore
    KF5::KIOWidgets
    libkommit
)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
