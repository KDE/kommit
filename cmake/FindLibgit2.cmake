# In cmake/FindLibgit2.cmake
# Best practice Find module for libgit2

find_path(LIBGIT2_INCLUDE_DIR
    NAMES git2.h
    PATH_SUFFIXES libgit2
)

find_library(LIBGIT2_LIBRARY
    NAMES git2
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libgit2
    REQUIRED_VARS
        LIBGIT2_LIBRARY
        LIBGIT2_INCLUDE_DIR
)

if(Libgit2_FOUND)
    set(LIBGIT2_LIBRARIES ${LIBGIT2_LIBRARY})
    set(LIBGIT2_INCLUDE_DIRS ${LIBGIT2_INCLUDE_DIR})

    if(NOT TARGET Libgit2::libgit2)
        add_library(Libgit2::libgit2 UNKNOWN IMPORTED)
        set_target_properties(Libgit2::libgit2 PROPERTIES
            IMPORTED_LOCATION "${LIBGIT2_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIBGIT2_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(LIBGIT2_INCLUDE_DIR LIBGIT2_LIBRARY)
