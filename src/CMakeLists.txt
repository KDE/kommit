include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(git)

set(CORE_SRC
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
)

set(ACTIONS_SRC
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
)

set(WIDGETS_SRC
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
)

set(MODELS_SRC
    models/difftreemodel.cpp
    models/difftreemodel.h
    models/filesmodel.cpp
    models/filesmodel.h
    models/historymodel.cpp
    models/historymodel.h
    models/treemodel.cpp
    models/treemodel.h
)

set(DIFF_SRC
    diff/codeeditordocument.cpp
    diff/codeeditordocument.h
    diff/diff.cpp
    diff/diff.h
    diff/segmentsmapper.cpp
    diff/segmentsmapper.h
)

set(DIALOGS_SRC
    dialogs/changedfilesdialog.cpp
    dialogs/changedfilesdialog.h
    dialogs/clonedialog.cpp
    dialogs/clonedialog.h
    dialogs/commitpushdialog.cpp
    dialogs/commitpushdialog.h
    dialogs/diffdialog.cpp
    dialogs/diffdialog.h
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
)

set(APP_SRCS
#    ${GIT_SRC}
    ${DIFF_SRC}
    ${CORE_WIDGETS_SRC}
    ${MODELS_SRC}
    ${WIDGETS_SRC}
    ${DIALOGS_SRC}
    ${COMMON_SRC}
    ${ACTIONS_SRC}
    ${CORE_SRC}

    main.cpp
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
    gitklient.qrc
)

ecm_qt_declare_logging_category(APP_SRCS
    HEADER gitklient_appdebug.h
    IDENTIFIER GITKLIENT_LOG
    CATEGORY_NAME "org.kde.gitklient" DESCRIPTION "gitklient" EXPORT GITKLIENT
)

kconfig_add_kcfg_files(APP_SRCS settings/GitKlientSettings.kcfgc)

add_compile_definitions(GIT_GUI)

ki18n_wrap_ui(
    APP_SRCS

    settings/settingspagebase.ui
    settings/settingspagediff.ui
    settings/settingspagegit.ui
    multipagewidget.ui
    gitklientmergeview.ui
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
    dialogs/diffdialog.ui
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
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(
        gitklient
        MANUAL_FINALIZATION
        ${APP_SRCS}
    )
else()
    add_executable(gitklient ${APP_SRCS})
endif()

target_link_libraries(gitklient
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent

    libgitklient

    KF5::CoreAddons
    KF5::I18n
    KF5::XmlGui
    KF5::ConfigWidgets
    KF5::DBusAddons
    KF5::Crash
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::KIOWidgets
    KF5::KIONTLM
    KF5::Parts
    KF5::TextWidgets
    KF5::TextEditor

    KF5::SyntaxHighlighting
)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(gitklient PROPERTIES UNITY_BUILD ON)
endif()


set_target_properties(
    gitklient PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.kde.gitklient
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gitklient)
endif()


#set_property(TARGET KI18n APPEND PROPERTY
#  INTERFACE_AUTOUIC_OPTIONS ${autouic_options}
#)

install(TARGETS gitklient ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS gitklientdiff.sh DESTINATION bin RENAME gitklientdiff)
install(PROGRAMS gitklientmerge.sh DESTINATION bin RENAME gitklientmerge)
install(PROGRAMS org.kde.gitklient.desktop        DESTINATION ${KDE_INSTALL_APPDIR})
install(PROGRAMS org.kde.gitklient.diff.desktop   DESTINATION ${KDE_INSTALL_APPDIR})
install(PROGRAMS org.kde.gitklient.merge.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.gitklient.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
