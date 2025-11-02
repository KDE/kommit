# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_library(libkommit SHARED
    actions/pull.h actions/pull.cpp
    actions/merge.h actions/merge.cpp
    entities/annotatedcommit.h entities/annotatedcommit.cpp)

set(KOMMIT_INSTALL_INCLUDEDIR "${KDE_INSTALL_INCLUDEDIR}/Kommit")
set(KOMMIT_CMAKECONFIG_NAME "Kommit")

ecm_setup_version(1.0.0
    VARIABLE_PREFIX      LIBKOMMIT
    VERSION_HEADER       "libkommit_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/${KOMMIT_CMAKECONFIG_NAME}ConfigVersion.cmake"
    COMPATIBILITY        SameMajorVersion
    SOVERSION            1
)

target_sources(libkommit PRIVATE
    libkommit_global.h

    gitglobal.cpp gitglobal.h
    gitloglist.cpp gitloglist.h
    filestatus.cpp filestatus.h
    repository.cpp repository.h
    types.h
    types.cpp
    abstractreference.cpp abstractreference.h
    buffer.cpp buffer.h
    credential.cpp credential.h
    entities/blob.h entities/blob.cpp
    config.h config.cpp
    filedelta.h filedelta.cpp
    filemode.h filemode.cpp
    refspec.h refspec.cpp
    itree.h
    ioid.h
    blame.h blame.cpp
    blamehunk.h blamehunk.cpp
    certificate.h certificate.cpp
    error.h error.cpp
    remotecallbacks.h remotecallbacks.cpp
    proxy.cpp proxy.h

    actions/abstractaction.h actions/abstractaction.cpp
    actions/push.h actions/push.cpp
    actions/fetch.h actions/fetch.cpp
    actions/clone.h actions/clone.cpp
    actions/checkoutaction.h actions/checkoutaction.cpp
    actions/pushaction.h actions/pushaction.cpp
    actions/commitaction.h actions/commitaction.cpp

    caches/abstractcache.cpp caches/abstractcache.h
    caches/branchescache.cpp caches/branchescache.h
    caches/commitscache.cpp caches/commitscache.h
    caches/remotescache.cpp caches/remotescache.h
    caches/tagscache.cpp caches/tagscache.h
    caches/notescache.cpp caches/notescache.h
    caches/stashescache.cpp caches/stashescache.h
    caches/submodulescache.cpp caches/submodulescache.h
    caches/referencecache.cpp caches/referencecache.h

    commands/abstractcommand.cpp commands/abstractcommand.h
    commands/commandchangedfiles.cpp commands/commandchangedfiles.h
    commands/commandclone.cpp commands/commandclone.h
    commands/commandcommit.cpp commands/commandcommit.h
    commands/commandfetch.cpp commands/commandfetch.h
    commands/commandpull.cpp commands/commandpull.h
    commands/commandpush.cpp commands/commandpush.h
    commands/commandswitchbranch.cpp commands/commandswitchbranch.h
    commands/commandmerge.cpp commands/commandmerge.h
    commands/addsubmodulecommand.cpp commands/addsubmodulecommand.h
    commands/commandaddremote.cpp commands/commandaddremote.h
    commands/parameterescommand.cpp commands/parameterescommand.h
    commands/commandclean.cpp commands/commandclean.h

    entities/branch.cpp
    entities/commit.cpp
    entities/file.cpp entities/file.h
    entities/note.cpp entities/note.h
    entities/reference.cpp
    entities/remote.cpp
    entities/signature.cpp entities/signature.h
    entities/stash.cpp entities/stash.h
    entities/tree.cpp
    entities/tag.cpp entities/tag.h
    entities/submodule.cpp entities/submodule.h
    entities/index.cpp entities/index.h
    entities/treediff.cpp entities/treediff.h
    entities/oid.h
    entities/oid.cpp
    entities/object.h
    entities/object.cpp
    entities/strarray.h entities/strarray.cpp

    observers/fetchobserver.cpp observers/fetchobserver.h
    observers/cloneobserver.cpp observers/cloneobserver.h
    observers/pushobserver.cpp observers/pushobserver.h

    options/addsubmoduleoptions.cpp options/addsubmoduleoptions.h
    options/fetchoptions.cpp options/fetchoptions.h
    options/checkoutoptions.cpp options/checkoutoptions.h
    options/blameoptions.h options/blameoptions.cpp
    options/pushoptions.h options/pushoptions.cpp
    options/stashoptions.h options/stashoptions.cpp
    options/cloneoptions.h options/cloneoptions.cpp
    options/rebaseoptions.h options/rebaseoptions.cpp
    options/mergeoptions.h options/mergeoptions.cpp
    options/commitoptions.h options/commitoptions.cpp
)

generate_export_header(libkommit BASE_NAME libkommit)

ecm_qt_declare_logging_category(libkommit
    HEADER libkommit_debug.h
    IDENTIFIER KOMMITLIB_LOG
    CATEGORY_NAME "org.kde.kommit.libkommit" DESCRIPTION "libkommit" EXPORT KOMMIT
)

ecm_generate_export_header(libkommit
    VERSION ${LIBKOMMIT_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
    USE_VERSION_HEADER
)

set_target_properties(libkommit PROPERTIES
    VERSION     ${KOMMIT_VERSION}
    SOVERSION   ${KOMMIT_SOVERSION}
    EXPORT_NAME "Kommit"
)

set_target_properties(libkommit
    PROPERTIES OUTPUT_NAME kommit VERSION ${LIBKOMMIT_LIB_VERSION} SOVERSION ${LIBKOMMIT_LIB_SOVERSION}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/kommit)
target_include_directories(libkommit
    INTERFACE
        "$<INSTALL_INTERFACE:${KOMMIT_INSTALL_INCLUDEDIR}>"
)

target_include_directories(libkommit PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/entities>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/options>
)

target_link_libraries(libkommit
    Qt::Core
    LibGit2::LibGit2
    KF${KF_MAJOR_VERSION}::I18n
)

if (OpenSSL_FOUND)
    message(STATUS "OpenSSL found: ${OpenSSL_VERSION}")
    add_definitions(-DLIBKOMMIT_USE_OPENSSL)
    target_link_libraries(libkommit OpenSSL::SSL OpenSSL::Crypto)
else()
    message(STATUS "OpenSSL not found, building without OpenSSL support")
endif()


if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

# root folder
ecm_generate_headers(libkommit_CamelCase_HEADERS
    HEADER_NAMES
        Repository

        Blame
        BlameHunk
        Config

        # ITree
        # IOid
        AbstractReference

        Buffer
        Credential
        Certificate
        FileStatus
        Types
        Error
        FileDelta
        FileMode
        Proxy
        RefSpec
        ITree
        IOid
        RemoteCallbacks
    PREFIX Kommit
    REQUIRED_HEADERS kommit_HEADERS
)

# options sub folder
ecm_generate_headers(libkommit_CamelCase_HEADERS
    HEADER_NAMES
        AddSubmoduleOptions
        BlameOptions
        CheckoutOptions
        CloneOptions
        FetchOptions
        StashOptions
        PushOptions
        RebaseOptions
        MergeOptions
        CommitOptions
    PREFIX Kommit
    REQUIRED_HEADERS kommit_HEADERS
    RELATIVE
        options
)

# entities sub folder
ecm_generate_headers(libkommit_CamelCase_HEADERS
    HEADER_NAMES
        Blob
        Branch
        Commit
        File
        Index
        Note
        Object
        Oid
        Reference
        Remote
        Signature
        Stash
        StrArray
        SubModule
        Tag
        Tree
        TreeDiff
    PREFIX Kommit
    REQUIRED_HEADERS kommit_HEADERS
    RELATIVE
        entities
)

# actions sub folder
ecm_generate_headers(libkommit_CamelCase_HEADERS
    HEADER_NAMES
        AbstractAction
        CheckoutAction
        CommitAction
        PushAction
        Fetch
        Clone
    PREFIX Kommit
    REQUIRED_HEADERS kommit_HEADERS
    RELATIVE
        actions
)

# commands sub folder
ecm_generate_headers(libkommit_CamelCase_HEADERS
    HEADER_NAMES
        AbstractCommand
        ParameteresCommand
        AddSubmoduleCommand
        CommandSwitchBranch
        CommandMerge
        CommandChangedFiles
        CommandPush
        CommandPull
        CommandFetch
        CommandCommit
        CommandClone
        CommandClean
        CommandAddRemote
    PREFIX Kommit
    REQUIRED_HEADERS kommit_HEADERS
    RELATIVE
        commands
)

# caches sub folder
ecm_generate_headers(libkommit_CamelCase_HEADERS
    HEADER_NAMES
        TagsCache
        SubmodulesCache
        StashesCache
        RemotesCache
        ReferenceCache
        NotesCache
        CommitsCache
        BranchesCache
        AbstractCache
    PREFIX Kommit
    REQUIRED_HEADERS kommit_HEADERS
    RELATIVE
        caches
)

# observers sub folder
ecm_generate_headers(libkommit_CamelCase_HEADERS
    HEADER_NAMES
        FetchObserver
        PushObserver
        CloneObserver
    PREFIX Kommit
    REQUIRED_HEADERS kommit_HEADERS
    RELATIVE
        observers
)

install(
    FILES
        ${libkommit_CamelCase_HEADERS}
        ${CMAKE_CURRENT_BINARY_DIR}/libkommit_export.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libkommit_global.h
        ${CMAKE_CURRENT_BINARY_DIR}/libkommit_version.h
    DESTINATION
        ${KOMMIT_INSTALL_INCLUDEDIR}/Kommit
    COMPONENT
        Devel
)

install(
    FILES
        ${kommit_HEADERS}
        ${CMAKE_CURRENT_BINARY_DIR}/libkommit_export.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libkommit_global.h
        ${CMAKE_CURRENT_BINARY_DIR}/libkommit_version.h
    DESTINATION
        ${KOMMIT_INSTALL_INCLUDEDIR}/kommit
    COMPONENT
        Devel
)


install(TARGETS libkommit ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
# install(FILES ${HEADERS} DESTINATION include/kommit) future releases
