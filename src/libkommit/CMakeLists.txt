# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_library(libkommit SHARED)

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

    gitglobal.cpp
    gitloglist.cpp
    filestatus.cpp
    repository.cpp
    types.cpp
    abstractreference.cpp
    buffer.cpp
    credential.cpp
    entities/blob.h entities/blob.cpp
    config.h config.cpp
    filedelta.h filedelta.cpp
    filemode.h filemode.cpp
    refspec.h refspec.cpp
    itree.h
    ioid.h
    blame.h blame.cpp
    blamehunk.h blamehunk.cpp
    fetch.h fetch.cpp
    certificate.h certificate.cpp
    error.h error.cpp
    remotecallbacks.h remotecallbacks.cpp
    proxy.cpp proxy.h
    clone.h clone.cpp

    caches/abstractcache.cpp
    caches/branchescache.cpp
    caches/commitscache.cpp
    caches/remotescache.cpp
    caches/tagscache.cpp
    caches/notescache.cpp
    caches/stashescache.cpp
    caches/submodulescache.cpp
    caches/referencecache.cpp

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
    entities/commit.cpp
    entities/file.cpp
    entities/note.cpp
    entities/reference.cpp
    entities/remote.cpp
    entities/signature.cpp
    entities/stash.cpp
    entities/tree.cpp
    entities/tag.cpp
    entities/submodule.cpp
    entities/index.cpp
    entities/treediff.cpp
    entities/oid.h
    entities/oid.cpp
    entities/object.h
    entities/object.cpp
    entities/strarray.h entities/strarray.cpp

    observers/fetchobserver.cpp
    observers/cloneobserver.cpp
    observers/pushobserver.cpp

    options/addsubmoduleoptions.cpp
    options/fetchoptions.cpp
    options/checkoutoptions.cpp
    options/blameoptions.h options/blameoptions.cpp
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
    Libgit2
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
        Clone
        Repository

        Blame
        BlameHunk
        Fetch
        # BlameDataRow

        # ConfigEntry
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
