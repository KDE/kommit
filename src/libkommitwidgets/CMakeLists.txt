include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libkommitwidgets SHARED)

generate_export_header(libkommitwidgets BASE_NAME libkommitwidgets)

target_sources(libkommitwidgets PRIVATE
    kommitwidgetsglobaloptions.h
    kommitwidgetsglobaloptions.cpp
    gitgraphlane.h
    gitgraphlane.cpp

    widgets/reportwidget.h
    widgets/graphpainter.cpp
    widgets/segmentsscrollbar.cpp
    widgets/difftreeview.h
    widgets/filesstatuseslist.h
    widgets/commitdetails.cpp
    widgets/avatarview.h
    widgets/codeeditor.cpp
    widgets/segmentsmapper.h
    widgets/segmentsscrollbar.h
    widgets/reportwidget.cpp
    widgets/codeeditorsidebar.h
    widgets/segmentsmapper.cpp
    widgets/segmentconnector.h
    widgets/diffwidget.h
    widgets/avatarview.cpp
    widgets/codeeditorsidebar.cpp
    widgets/codeeditor.h
    widgets/blamecodeview.h
    widgets/completiontextedit.cpp
    widgets/completiontextedit.h
    widgets/graphpainter.h
    widgets/segmentconnector.cpp
    widgets/blamecodeview.cpp
    widgets/diffwidget.cpp
    widgets/difftreeview.cpp
    widgets/filesstatuseslist.cpp
    widgets/commitdetails.h
    widgets/urlrequester.cpp
    widgets/urlrequester.h
    widgets/fetchresultwidget.h
    widgets/fetchresultwidget.cpp

    core/kmessageboxhelper.h
    core/kmessageboxhelper.cpp
    core/editactionsmapper.h
    core/editactionsmapper.cpp
    core/gravatarcache.cpp
    core/gravatarcache.h

    reports/authorsreport.cpp
    reports/authorsreport.h
    reports/abstractreport.cpp
    reports/abstractreport.h
    reports/commitsbydayweek.cpp
    reports/commitsbydayweek.h
    reports/commitsbydayhour.cpp
    reports/commitsbydayhour.h
    reports/commitsbymonth.cpp
    reports/commitsbymonth.h

    windows/appmainwindow.cpp
    windows/mergewindow.h
    windows/diffwindow.h
    windows/diffwindow.cpp
    windows/mergewindow.cpp
    windows/appmainwindow.h

    actions/submoduleactions.cpp
    actions/remotesactions.h
    actions/tagsactions.cpp
    actions/branchactions.cpp
    actions/changedfileactions.h
    actions/branchactions.h
    actions/fileactions.cpp
    actions/submoduleactions.h
    actions/stashactions.h
    actions/abstractactions.cpp
    actions/abstractactions.h
    actions/fileactions.h
    actions/commitactions.cpp
    actions/changedfileactions.cpp
    actions/stashactions.cpp
    actions/commitactions.h
    actions/tagsactions.h
    actions/remotesactions.cpp

    models/treemodel.h
    models/treemodel.cpp
    models/changedfilesmodel.h
    models/difftreemodel.h
    models/difftreemodel.cpp
    models/filesmodel.cpp
    models/changedfilesmodel.cpp
    models/filesmodel.h

    models/abstractgititemsmodel.h
    models/abstractgititemsmodel.cpp
    models/submodulesmodel.cpp
    models/submodulesmodel.h
    models/branchesmodel.cpp
    models/branchesmodel.h
    models/commitsmodel.cpp
    models/commitsmodel.h
    models/remotesmodel.cpp
    models/remotesmodel.h
    models/stashesmodel.cpp
    models/stashesmodel.h
    models/tagsmodel.cpp
    models/tagsmodel.h


    dialogs/appdialog.h
    dialogs/runnerdialog.cpp
    dialogs/submoduleinfodialog.cpp
    dialogs/credentialdialog.cpp
    dialogs/taginfodialog.h
    dialogs/selectbranchestodiffdialog.h
    dialogs/selectbranchestodiffdialog.cpp
    dialogs/initdialog.h
    dialogs/changedfilesdialog.h
    dialogs/mergedialog.h
    dialogs/filehistorydialog.h
    dialogs/changedsubmodulesdialog.h
    dialogs/ignorefiledialog.cpp
    dialogs/switchbranchdialog.h
    dialogs/changedsubmodulesdialog.cpp
    dialogs/initdialog.cpp
    dialogs/cleanupdialog.h
    dialogs/ignorefiledialog.h
    dialogs/reposettingsdialog.h
    dialogs/mergeopenfilesdialog.h
    dialogs/remoteinfodialog.h
    dialogs/diffopendialog.cpp
    dialogs/fileviewerdialog.h
    dialogs/commitpushdialog.cpp
    dialogs/mergedialog.cpp
    dialogs/filestreedialog.h
    dialogs/fileblamedialog.h
    dialogs/submoduleinfodialog.h
    dialogs/changedfilesdialog.cpp
    dialogs/switchbranchdialog.cpp
    dialogs/mergecloseeventdialog.cpp
    dialogs/reposettingsdialog.cpp
    dialogs/mergecloseeventdialog.h
    dialogs/mergeopenfilesdialog.cpp
    dialogs/notedialog.h
    dialogs/remoteinfodialog.cpp
    dialogs/pulldialog.h
    dialogs/searchdialog.cpp
    dialogs/taginfodialog.cpp
    dialogs/filestreedialog.cpp
    dialogs/fetchdialog.h
    dialogs/diffopendialog.h
    dialogs/credentialdialog.h
    dialogs/fetchdialog.cpp
    dialogs/fileviewerdialog.cpp
    dialogs/cleanupdialog.cpp
    dialogs/commitpushdialog.h
    dialogs/notedialog.cpp
    dialogs/clonedialog.cpp
    dialogs/appdialog.cpp
    dialogs/runnerdialog.h
    dialogs/pulldialog.cpp
    dialogs/clonedialog.h
    dialogs/fileblamedialog.cpp
    dialogs/searchdialog.h
    dialogs/filehistorydialog.cpp

)

target_link_libraries(libkommitwidgets
    Qt::Core
    Qt::Widgets
    Qt::Concurrent
    Qt::Network

    libkommit
    libkommitdiff

    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::SyntaxHighlighting
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::KIOWidgets
    KF${KF_MAJOR_VERSION}::Parts

)

if(TARGET Qt::Charts)
    target_link_libraries(libkommitwidgets Qt::Charts)
endif()

install(TARGETS libkommitwidgets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

ecm_qt_declare_logging_category(libkommitwidgets
    HEADER libkommitwidgets_appdebug.h
    IDENTIFIER KOMMIT_WIDGETS_LOG
    CATEGORY_NAME "org.kde.kommit.widgets" DESCRIPTION "kommit" EXPORT KOMMIT
)

add_compile_definitions(LIBKOMMIT_WIDGET_USE_KF)

ki18n_wrap_ui(
    libkommitwidgets

    widgets/diffwidget.ui
    widgets/commitdetails.ui
    widgets/reportwidget.ui
    widgets/difftreeview.ui
    windows/kommitmergeview.ui
    widgets/fetchresultwidget.ui

    dialogs/mergecloseeventdialog.ui
    dialogs/searchdialog.ui
    dialogs/credentialdialog.ui
    dialogs/submoduleinfodialog.ui
    dialogs/remoteinfodialog.ui
    dialogs/changedfilesdialog.ui
    dialogs/cleanupdialog.ui
    dialogs/clonedialog.ui
    dialogs/selectbranchestodiffdialog.ui
    dialogs/pulldialog.ui
    dialogs/diffopendialog.ui
    dialogs/runnerdialog.ui
    dialogs/fileblamedialog.ui
    dialogs/changedsubmodulesdialog.ui
    dialogs/initdialog.ui
    dialogs/switchbranchdialog.ui
    dialogs/mergeopenfilesdialog.ui
    dialogs/fileviewerdialog.ui
    dialogs/filestreedialog.ui
    dialogs/filehistorydialog.ui
    dialogs/commitpushdialog.ui
    dialogs/mergedialog.ui
    dialogs/reposettingsdialog.ui
    dialogs/fetchdialog.ui
    dialogs/ignorefiledialog.ui
    dialogs/notedialog.ui
    dialogs/taginfodialog.ui
)

if(COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(libkommitwidgets PROPERTIES UNITY_BUILD ON)
endif()

set_target_properties(libkommitwidgets
    PROPERTIES OUTPUT_NAME kommitwidgets VERSION ${LIBKOMMIT_LIB_VERSION} SOVERSION ${LIBKOMMIT_LIB_SOVERSION}
)

set_target_properties(
    libkommitwidgets PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.kde.kommit
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
