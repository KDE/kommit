# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_library(libkommit SHARED)

set(SOURCES
    libkommit_global.h

    gitglobal.cpp
    gitloglist.cpp
    filestatus.cpp
    gitmanager.cpp
    blamedata.cpp
    types.cpp
    abstractreference.cpp
    buffer.cpp
    credential.cpp

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

    observers/fetchobserver.cpp
    observers/cloneobserver.cpp
    observers/checkoutobserver.cpp
    observers/pushobserver.cpp

    options/addsubmoduleoptions.cpp
    options/fetchoptions.cpp
    options/checkoutoptions.cpp
)
set(HEADERS
    gitglobal.h
    gitloglist.h
    filestatus.h
    gitmanager.h
    blamedata.h
    types.h
    abstractreference.h
    buffer.h
    credential.h

    caches/abstractcache.h
    caches/branchescache.h
    caches/commitscache.h
    caches/remotescache.h
    caches/tagscache.h
    caches/notescache.h
    caches/stashescache.h
    caches/submodulescache.h
    caches/referencecache.h

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
    commands/addsubmodulecommand.h

    entities/branch.h
    entities/commit.h
    entities/file.h
    entities/note.h
    entities/reference.h
    entities/remote.h
    entities/signature.h
    entities/stash.h
    entities/tree.h
    entities/tag.h
    entities/submodule.h
    entities/treediff.h
    entities/index.h

    observers/fetchobserver.h
    observers/cloneobserver.h
    observers/checkoutobserver.h
    observers/pushobserver.h

    options/addsubmoduleoptions.h
    options/fetchoptions.h
    options/checkoutoptions.h
)
target_sources(libkommit PRIVATE ${SOURCES} ${HEADERS})
# target_sources(libkommit
#     PUBLIC FILE_SET HEADERS
#     BASE_DIRS ${MY_LIB_SOURCE_DIR}
#     FILES ${HEADERS}
#)

generate_export_header(libkommit BASE_NAME libkommit)

ecm_qt_declare_logging_category(libkommit
    HEADER libkommit_debug.h
    IDENTIFIER KOMMITLIB_LOG
    CATEGORY_NAME "org.kde.kommit.libkommit" DESCRIPTION "libkommit" EXPORT KOMMIT
)

set_target_properties(libkommit
    PROPERTIES OUTPUT_NAME kommit VERSION ${LIBKOMMIT_LIB_VERSION} SOVERSION ${LIBKOMMIT_LIB_SOVERSION}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/kommit)

target_link_libraries(libkommit
    PkgConfig::LIBGIT2
    Qt::Core
    KF${KF_MAJOR_VERSION}::I18n
)

if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

install(TARGETS libkommit ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
# install(FILES ${HEADERS} DESTINATION include/kommit) future releases
