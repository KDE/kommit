include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libkommitgui SHARED)
generate_export_header(libkommitgui BASE_NAME libkommitgui)
target_sources(libkommitgui PRIVATE
    core/appdialog.cpp
    core/appdialog.h
    core/appmainwindow.cpp
    core/appmainwindow.h
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
    actions/abstractactions.cpp
    actions/abstractactions.h
    actions/branchactions.cpp
    actions/branchactions.h
    actions/changedfileactions.cpp
    actions/changedfileactions.h
    actions/commitactions.cpp
    actions/commitactions.h
    actions/fileactions.cpp
    actions/fileactions.h
    actions/remotesactions.cpp
    actions/remotesactions.h
    actions/stashactions.cpp
    actions/stashactions.h
    actions/submoduleactions.cpp
    actions/submoduleactions.h
    actions/tagsactions.cpp
    actions/tagsactions.h
    widgets/blamecodeview.cpp
    widgets/blamecodeview.h
    widgets/branchesstatuswidget.cpp
    widgets/branchesstatuswidget.h
    widgets/codeeditor.cpp
    widgets/codeeditor.h
    widgets/codeeditorsidebar.cpp
    widgets/codeeditorsidebar.h
    widgets/commitswidget.cpp
    widgets/commitswidget.h
    widgets/completiontextedit.cpp
    widgets/completiontextedit.h
    widgets/difftreeview.cpp
    widgets/difftreeview.h
    widgets/diffwidget.cpp
    widgets/diffwidget.h
    widgets/editactionsmapper.cpp
    widgets/editactionsmapper.h
    widgets/filesstatuseslist.cpp
    widgets/filesstatuseslist.h
    widgets/graphpainter.cpp
    widgets/graphpainter.h
    widgets/historyviewwidget.cpp
    widgets/historyviewwidget.h
    widgets/logdetailswidget.cpp
    widgets/logdetailswidget.h
    widgets/remoteinfodialog.cpp
    widgets/remoteinfodialog.h
    widgets/remoteswidget.cpp
    widgets/remoteswidget.h
    widgets/segmentconnector.cpp
    widgets/segmentconnector.h
    widgets/stasheswidget.cpp
    widgets/stasheswidget.h
    widgets/submoduleswidget.cpp
    widgets/submoduleswidget.h
    widgets/tagswidget.cpp
    widgets/tagswidget.h
    widgets/widgetbase.cpp
    widgets/widgetbase.h
    widgets/segmentsscrollbar.h
    widgets/segmentsscrollbar.cpp
    widgets/authorswidget.h
    widgets/authorswidget.cpp
    widgets/segmentsmapper.h
    widgets/segmentsmapper.cpp
    models/difftreemodel.cpp
    models/difftreemodel.h
    models/filesmodel.cpp
    models/filesmodel.h
    models/treemodel.cpp
    models/treemodel.h
    models/changedfilesmodel.h
    models/changedfilesmodel.cpp
    dialogs/changedfilesdialog.cpp
    dialogs/changedfilesdialog.h
    dialogs/clonedialog.cpp
    dialogs/clonedialog.h
    dialogs/commitpushdialog.cpp
    dialogs/commitpushdialog.h
    dialogs/diffopendialog.cpp
    dialogs/diffopendialog.h
    dialogs/fetchdialog.cpp
    dialogs/fetchdialog.h
    dialogs/fileblamedialog.cpp
    dialogs/fileblamedialog.h
    dialogs/filehistorydialog.cpp
    dialogs/filehistorydialog.h
    dialogs/filestreedialog.cpp
    dialogs/filestreedialog.h
    dialogs/fileviewerdialog.cpp
    dialogs/fileviewerdialog.h
    dialogs/ignorefiledialog.cpp
    dialogs/ignorefiledialog.h
    dialogs/initdialog.cpp
    dialogs/initdialog.h
    dialogs/mergecloseeventdialog.cpp
    dialogs/mergecloseeventdialog.h
    dialogs/mergedialog.cpp
    dialogs/mergedialog.h
    dialogs/mergeopenfilesdialog.cpp
    dialogs/mergeopenfilesdialog.h
    dialogs/notedialog.cpp
    dialogs/notedialog.h
    dialogs/pulldialog.cpp
    dialogs/pulldialog.h
    dialogs/reposettingsdialog.cpp
    dialogs/reposettingsdialog.h
    dialogs/runnerdialog.cpp
    dialogs/runnerdialog.h
    dialogs/searchdialog.cpp
    dialogs/searchdialog.h
    dialogs/selectbranchestodiffdialog.cpp
    dialogs/selectbranchestodiffdialog.h
    dialogs/submoduleinfodialog.cpp
    dialogs/submoduleinfodialog.h
    dialogs/switchbranchdialog.cpp
    dialogs/switchbranchdialog.h
    dialogs/taginfodialog.cpp
    dialogs/taginfodialog.h
    dialogs/cleanupdialog.cpp
    dialogs/cleanupdialog.h

    appwindow.cpp
    buttonsgroup.cpp
    multipagewidget.cpp
    diffwindow.cpp
    mergewindow.cpp
    commandargsparser.cpp
    appconfig.cpp
    settings/settingsmanager.cpp
    appwindow.h
    buttonsgroup.h
    multipagewidget.h
    diffwindow.h
    mergewindow.h
    commandargsparser.h
    appconfig.h
    settings/settingsmanager.h
    kommit.qrc
)


set_target_properties(libkommitgui
    PROPERTIES OUTPUT_NAME kommitgui VERSION ${LIBKOMMIT_LIB_VERSION} SOVERSION ${LIBKOMMIT_LIB_SOVERSION}
)

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

    widgets/branchesstatuswidget.ui
    widgets/remoteinfodialog.ui
    widgets/remoteswidget.ui
    widgets/stasheswidget.ui
    widgets/historyviewwidget.ui
    widgets/submoduleswidget.ui
    widgets/commitswidget.ui
    widgets/tagswidget.ui
    widgets/difftreeview.ui
    widgets/diffwidget.ui
    widgets/authorswidget.ui

    dialogs/ignorefiledialog.ui
    dialogs/mergeopenfilesdialog.ui
    dialogs/mergecloseeventdialog.ui
    dialogs/changedfilesdialog.ui
    dialogs/filestreedialog.ui
    dialogs/commitpushdialog.ui
    dialogs/pulldialog.ui
    dialogs/mergedialog.ui
    dialogs/runnerdialog.ui
    dialogs/fileviewerdialog.ui
    dialogs/clonedialog.ui
    dialogs/submoduleinfodialog.ui
    dialogs/taginfodialog.ui
    dialogs/diffopendialog.ui
    dialogs/selectbranchestodiffdialog.ui
    dialogs/filehistorydialog.ui
    dialogs/fileblamedialog.ui
    dialogs/searchdialog.ui
    dialogs/reposettingsdialog.ui
    dialogs/fetchdialog.ui
    dialogs/switchbranchdialog.ui
    dialogs/initdialog.ui
    dialogs/notedialog.ui
    dialogs/cleanupdialog.ui
)

target_link_libraries(libkommitgui
    Qt::Core
    Qt::Widgets
    Qt::Concurrent

    libkommit
    libkommitdiff

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
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
