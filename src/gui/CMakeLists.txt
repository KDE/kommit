include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libkommitgui SHARED)
generate_export_header(libkommitgui BASE_NAME libkommitgui)
target_sources(libkommitgui PRIVATE
    core/listview.cpp
    core/listview.h
    core/pushbutton.cpp
    core/pushbutton.h
    core/treeview.cpp
    core/treeview.h
    core/commitsfiltermodel.h
    core/commitsfiltermodel.cpp
    core/kmessageboxhelper.h
    core/kmessageboxhelper.cpp
    core/repositorydata.h
    core/repositorydata.cpp

    models/difftreemodel.cpp
    models/difftreemodel.h
    models/filesmodel.cpp
    models/filesmodel.h
    models/treemodel.cpp
    models/treemodel.h
    models/changedfilesmodel.h
    models/changedfilesmodel.cpp

    pages/reportswidget.h
    pages/reportswidget.cpp
    pages/branchesstatuswidget.h
    pages/branchesstatuswidget.cpp
    pages/commitswidget.h
    pages/commitswidget.cpp
    pages/historyviewwidget.h
    pages/historyviewwidget.cpp
    pages/remoteswidget.h
    pages/remoteswidget.cpp
    pages/stasheswidget.h
    pages/stasheswidget.cpp
    pages/submoduleswidget.h
    pages/submoduleswidget.cpp
    pages/tagswidget.h
    pages/tagswidget.cpp
    pages/widgetbase.h
    pages/widgetbase.cpp

    appwindow.cpp
    buttonsgroup.cpp
    multipagewidget.cpp


    commandargsparser.cpp
    appconfig.cpp
    settings/settingsmanager.cpp
    appwindow.h
    buttonsgroup.h
    multipagewidget.h


    commandargsparser.h
    appconfig.h
    settings/settingsmanager.h
    kommit.qrc
    icons/icons.qrc

    pages/reportswidget.h
    pages/reportswidget.cpp
)

set_target_properties(libkommitgui
    PROPERTIES OUTPUT_NAME kommitgui VERSION ${LIBKOMMIT_LIB_VERSION} SOVERSION ${LIBKOMMIT_LIB_SOVERSION}
)

add_compile_definitions(LIBKOMMIT_WIDGET_USE_KF)

install(TARGETS libkommitgui ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

ecm_qt_declare_logging_category(libkommitgui
    HEADER kommit_appdebug.h
    IDENTIFIER KOMMIT_LOG
    CATEGORY_NAME "org.kde.kommit" DESCRIPTION "kommit" EXPORT KOMMIT
)

kconfig_add_kcfg_files(libkommitgui settings/KommitSettings.kcfgc)

add_compile_definitions(GIT_GUI)

ki18n_wrap_ui(
    libkommitgui

    settings/settingspagebase.ui
    settings/settingspagediff.ui
    settings/settingspagegit.ui
    multipagewidget.ui
    kommitmergeview.ui
    commandpullwidget.ui

    pages/reportswidget.ui
    pages/branchesstatuswidget.ui
    pages/commitswidget.ui
    pages/historyviewwidget.ui
    pages/remoteswidget.ui
    pages/stasheswidget.ui
    pages/submoduleswidget.ui
    pages/tagswidget.ui
    pages/reportswidget.ui
)

target_link_libraries(libkommitgui
    Qt::Core
    Qt::Widgets
    Qt::Concurrent
    Qt::Network

    libkommit
    libkommitdiff
    libkommitwidgets

    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::ConfigWidgets
    KF${KF_MAJOR_VERSION}::DBusAddons
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::KIOCore
    KF${KF_MAJOR_VERSION}::KIOFileWidgets
    KF${KF_MAJOR_VERSION}::KIOWidgets
    KF${KF_MAJOR_VERSION}::Parts
    KF${KF_MAJOR_VERSION}::TextWidgets
    KF${KF_MAJOR_VERSION}::TextEditor

    KF${KF_MAJOR_VERSION}::SyntaxHighlighting
)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(libkommitgui PROPERTIES UNITY_BUILD ON)
endif()


set_target_properties(
    libkommitgui PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.kde.kommit
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
