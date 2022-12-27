add_library(libgitklientdiff SHARED)

target_sources(
    libgitklientdiff
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
)

generate_export_header(libgitklientdiff BASE_NAME libgitklientdiff)

ecm_qt_declare_logging_category(libgitklientdiff
    HEADER libgitklientdiff_debug.h
    IDENTIFIER GITKLIENTDIFFLIB_LOG
    CATEGORY_NAME "org.kde.gitklient.libgitklientdiff" DESCRIPTION "libgitklientdiff" EXPORT GITKLIENT
)

set_target_properties(libgitklientdiff
    PROPERTIES OUTPUT_NAME gitklientdiff VERSION ${LIBGITKLIENT_LIB_VERSION} SOVERSION ${LIBGITKLIENT_LIB_SOVERSION}
)

target_link_libraries(libgitklientdiff
    Qt::Core
    Qt::Gui
)

install(
    TARGETS libgitklientdiff ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    LIBRARY NAMELINK_SKIP
)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
