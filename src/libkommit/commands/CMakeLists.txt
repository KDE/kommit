set(
    GIT_SOURCE
    ${GIT_SOURCE}

    ${CMAKE_CURRENT_LIST_DIR}/abstractcommand.cpp ${CMAKE_CURRENT_LIST_DIR}/abstractcommand.h
    ${CMAKE_CURRENT_LIST_DIR}/commandchangedfiles.cpp ${CMAKE_CURRENT_LIST_DIR}/commandchangedfiles.h
    ${CMAKE_CURRENT_LIST_DIR}/commandclone.cpp ${CMAKE_CURRENT_LIST_DIR}/commandclone.h
    ${CMAKE_CURRENT_LIST_DIR}/commandcommit.cpp ${CMAKE_CURRENT_LIST_DIR}/commandcommit.h
    ${CMAKE_CURRENT_LIST_DIR}/commandfetch.cpp ${CMAKE_CURRENT_LIST_DIR}/commandfetch.h
    ${CMAKE_CURRENT_LIST_DIR}/commandpull.cpp ${CMAKE_CURRENT_LIST_DIR}/commandpull.h
    ${CMAKE_CURRENT_LIST_DIR}/commandpush.cpp ${CMAKE_CURRENT_LIST_DIR}/commandpush.h

    PARENT_SCOPE
)

ecm_generate_headers(libkommit_CamelCase_HEADERS
    HEADER_NAMES

        CommandSwitchBranch
        CommandMerge
        CommandChangedFiles
        AddSubmoduleUommand
        AbstractUommand
        ParameteresCommand
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
