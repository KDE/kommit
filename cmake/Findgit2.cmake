# Find git2 Library
#
#  GIT2_INCLUDE_DIRS - where to find git2.h, etc.
#  GIT2_LIBRARIES    - List of libraries when using libgit2.
#  GIT2_FOUND        - True if libgit2 is found.

# GIT2_INCLUDE_PATH
find_path(GIT2_INCLUDE_PATH NAMES git2.h)
# GIT2_LIBRARY
find_library(GIT2_LIBRARY NAMES git2)

# handle the QUIETLY and REQUIRED arguments and set GIT2_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libgit2 REQUIRED_VARS GIT2_LIBRARY GIT2_INCLUDE_PATH)


if (LIBGIT2_FOUND)
  set(LIBGIT2_INCLUDE_DIR  ${GIT2_INCLUDE_PATH})
  set(LIBGIT2_INCLUDE_DIRS ${GIT2_INCLUDE_PATH})
  set(LIBGIT2_LIBRARIES    ${GIT2_LIBRARY})
endif()

mark_as_advanced(
  LIBGIT2_INCLUDE_PATH
  LIBGIT2_LIBRARY
)
