add_library(libgitklient SHARED)

target_sources(libgitklient PRIVATE
    gitglobal.cpp
    gitloglist.cpp
    gitsubmodule.cpp
    stash.cpp
    filestatus.cpp
    gitgraphlane.cpp
    gitmanager.cpp
    nonemptylinescommand.cpp
    blamedata.cpp
    gitfile.cpp
    gitlog.cpp
    gitremote.cpp
    gittag.cpp

    models/abstractgititemsmodel.cpp
    models/logsmodel.cpp
    models/remotesmodel.cpp
    models/submodulesmodel.cpp
    models/branchesmodel.cpp
    models/stashesmodel.cpp
    models/tagsmodel.cpp
    models/authorsmodel.cpp

    commands/abstractcommand.cpp
    commands/commandchangedfiles.cpp
    commands/commandclone.cpp
    commands/commandcommit.cpp
    commands/commandfetch.cpp
    commands/commandpull.cpp
    commands/commandpush.cpp
    commands/commandswitchbranch.cpp
    commands/commandmerge.cpp
    commands/addsubmodulecommand.cpp
    commands/commandaddremote.cpp
    commands/parameterescommand.cpp

    gitglobal.h
    gitloglist.h
    gitsubmodule.h
    stash.h
    filestatus.h
    gitgraphlane.h
    gitmanager.h
    nonemptylinescommand.h
    blamedata.h
    gitfile.h
    gitlog.h
    gitremote.h
    gittag.h

    models/abstractgititemsmodel.h
    models/logsmodel.h
    models/remotesmodel.h
    models/submodulesmodel.h
    models/branchesmodel.h
    models/stashesmodel.h
    models/tagsmodel.h
    models/authorsmodel.h

    commands/abstractcommand.h
    commands/commandchangedfiles.h
    commands/commandclone.h
    commands/commandcommit.h
    commands/commandfetch.h
    commands/commandpull.h
    commands/commandpush.h
    commands/commandswitchbranch.h
    commands/commandmerge.h
    commands/addsubmodulecommand.h
    commands/commandaddremote.h
    commands/parameterescommand.h
)

generate_export_header(libgitklient BASE_NAME libgitklient)

ecm_qt_declare_logging_category(libgitklient
    HEADER libgitklient_debug.h
    IDENTIFIER GITKLIENTLIB_LOG
    CATEGORY_NAME "org.kde.gitklient.libgitklient" DESCRIPTION "libgitklient" EXPORT GITKLIENT
)

set_target_properties(libgitklient
    PROPERTIES OUTPUT_NAME gitklient VERSION ${LIBGITKLIENT_LIB_VERSION} SOVERSION ${LIBGITKLIENT_LIB_SOVERSION}
)

target_link_libraries(libgitklient
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    KF5::I18n
)

install(TARGETS libgitklient ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
