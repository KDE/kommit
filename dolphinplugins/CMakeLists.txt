find_package(KF${KF_MAJOR_VERSION} ${KF${KF_MAJOR_VERSION}_MIN_VERSION} REQUIRED COMPONENTS KIO)

set(GIT_EXTRA_SOURCE
)

ecm_qt_declare_logging_category(GIT_EXTRA_SOURCE HEADER kommit_dolphinplugins_debug.h
    IDENTIFIER KOMMIT_DOLPHIN_PLUGINS_LOG CATEGORY_NAME org.kde.kommit.dolphinplugins DESCRIPTION "kommit dolphin plugins" EXPORT kommit)

#--- VCS Plugin ---
add_definitions(-DTRANSLATION_DOMAIN=\"fileviewgitplugin\")

kcoreaddons_add_plugin(kommitdolphinplugin INSTALL_NAMESPACE "dolphin/vcs")

target_sources(kommitdolphinplugin PRIVATE
    dolphinplugin.h dolphinplugin.cpp
    statuscache.h statuscache.cpp
)

target_link_libraries(kommitdolphinplugin PRIVATE
    Qt::Core
    Qt::Widgets
    KF${KF_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::KIOCore
    DolphinVcs
    PkgConfig::LIBGIT2
)

if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(kommitdolphinplugin PRIVATE
        Qt::Core5Compat
    )
endif()

#install(FILES fileviewgitpluginsettings.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
