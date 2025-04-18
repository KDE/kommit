# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_library(libkommitdiff SHARED)

target_sources(
    libkommitdiff
    PRIVATE
    diff.cpp
    diff.h
    results.cpp
    results.h
    types.h
    segments.h
    segments.cpp
    text.h
    text.cpp
    array.h
    array.cpp
    pair.h
    pair.cpp
    solution.h
    solution.cpp
    lcs.h
    lcs.cpp

    diff2.h diff2.cpp
    diff3.h diff3.cpp
    hunk.h hunk.cpp
)

generate_export_header(libkommitdiff BASE_NAME libkommitdiff)

ecm_qt_declare_logging_category(libkommitdiff
    HEADER libkommitdiff_debug.h
    IDENTIFIER KOMMITDIFFLIB_LOG
    CATEGORY_NAME "org.kde.kommit.libkommitdiff" DESCRIPTION "libkommitdiff" EXPORT KOMMIT
)

ecm_generate_headers(libkommitdiff_CamelCase_HEADERS
    HEADER_NAMES
        Diff
        Diff2
        Diff3
        Hunk
    PREFIX KommitDiff
    REQUIRED_HEADERS kommitdiff_HEADERS
)

set_target_properties(libkommitdiff
    PROPERTIES OUTPUT_NAME kommitdiff VERSION ${LIBKOMMIT_LIB_VERSION} SOVERSION ${LIBKOMMIT_LIB_SOVERSION}
)

target_link_libraries(libkommitdiff
    Qt::Core
)

install(
    TARGETS libkommitdiff ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    LIBRARY NAMELINK_SKIP
)

if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()
