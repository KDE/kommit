add_library(libkommit SHARED)

target_sources(libkommit PRIVATE
    gitglobal.cpp
    gitloglist.cpp
    filestatus.cpp
    gitgraphlane.cpp
    gitmanager.cpp
    blamedata.cpp
    types.h types.cpp
    gitglobal.h
    gitloglist.h
    filestatus.h
    gitgraphlane.h
    gitmanager.h
    blamedata.h
    abstractreference.h abstractreference.cpp
    interfaces.h

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
    commands/commandclean.cpp

    entities/branch.cpp
    entities/branch.h
    entities/commit.cpp
    entities/commit.h
    entities/file.cpp
    entities/file.h
    entities/note.cpp
    entities/note.h
    entities/reference.cpp
    entities/reference.h
    entities/remote.cpp
    entities/remote.h
    entities/signature.cpp
    entities/signature.h
    entities/stash.cpp
    entities/stash.h
    entities/tag.cpp
    entities/tag.h
    entities/tree.cpp
    entities/tree.h
    entities/submodule.h entities/submodule.cpp
    entities/index.h entities/index.cpp
    entities/treediff.h entities/treediff.cpp

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
    commands/commandclean.h

    observers/fetchobserver.h observers/fetchobserver.cpp
    observers/cloneobserver.h observers/cloneobserver.cpp
    observers/checkoutobserver.h observers/checkoutobserver.cpp
    observers/pushobserver.h observers/pushobserver.cpp

    options/addsubmoduleoptions.h options/addsubmoduleoptions.cpp
    options/fetchoptions.h options/fetchoptions.cpp
    options/checkoutoptions.h options/checkoutoptions.cpp
)

generate_export_header(libkommit BASE_NAME libkommit)

ecm_qt_declare_logging_category(libkommit
    HEADER libkommit_debug.h
    IDENTIFIER KOMMITLIB_LOG
    CATEGORY_NAME "org.kde.kommit.libkommit" DESCRIPTION "libkommit" EXPORT KOMMIT
)

set_target_properties(libkommit
    PROPERTIES OUTPUT_NAME kommit VERSION ${LIBKOMMIT_LIB_VERSION} SOVERSION ${LIBKOMMIT_LIB_SOVERSION}
)

target_link_libraries(libkommit
    PkgConfig::LIBGIT2
    Qt::Core
    Qt::Widgets
    Qt::Concurrent
    KF${KF_MAJOR_VERSION}::I18n
)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

install(TARGETS libkommit ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
