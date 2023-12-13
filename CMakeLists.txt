cmake_minimum_required(VERSION 3.16)

project(kommit)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 4)
set(PROJECT_VERSION_PATCH 0)

set (RELEASE_SERVICE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set(LIBKOMMIT_LIB_VERSION "${RELEASE_SERVICE_VERSION}")
set(LIBKOMMIT_LIB_SOVERSION "0")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

#find_package(git2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGIT2 "libgit2>=1.0" REQUIRED IMPORTED_TARGET)


add_definitions(-DGK_VERSION="${RELEASE_SERVICE_VERSION}")
set(KF_MIN_VERSION "5.90")
set(QT_MIN_VERSION 5.15.2)
set(KDE_COMPILERSETTINGS_LEVEL "5.82")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

#find_package(git2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGIT2 "libgit2>=1.0" REQUIRED IMPORTED_TARGET)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(
    CMAKE_MODULE_PATH
    ${ECM_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(FeatureSummary)

include(ECMInstallIcons)
include(ECMQtDeclareLoggingCategory)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(ECMAddTests)
include(GenerateExportHeader)
include(KDEGitCommitHooks)
include(KDEClangFormat)
include(ECMDeprecationSettings)
if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_REQUIRED_VERSION "6.4.0")
    set(KF_MIN_VERSION "5.240.0")
    set(KF_MAJOR_VERSION "6")
else()
    set(KF_MAJOR_VERSION "5")
endif()

set (CMAKE_CXX_STANDARD 17)

find_package(
    Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION}
    CONFIG REQUIRED
        Core
        Gui
        Widgets
        Concurrent
)

if (QT_MAJOR_VERSION STREQUAL "6")
    find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Core5Compat)
endif()

find_package(
    KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION}
    REQUIRED COMPONENTS
        ConfigWidgets
        CoreAddons
        Crash
        DBusAddons
        DocTools
        I18n
        XmlGui
        KIO
        TextWidgets
        TextEditor
        SyntaxHighlighting
)
find_package(DolphinVcs REQUIRED)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)
add_definitions(-DQT_NO_CAST_TO_ASCII)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
add_definitions(-DQT_NO_FOREACH)
#add_definitions(-DQT_NO_CAST_FROM_ASCII)

ecm_set_disabled_deprecation_versions(QT 5.15.2
    )


option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if (USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
endif()

if(BUILD_TESTING)
    find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Test)
    add_definitions(-DBUILD_TESTING)
endif(BUILD_TESTING)


if(KF${KF_MAJOR_VERSION}DocTools_FOUND) # For the moment on kf6 it doesn't build depend against deprecated &kf5
    add_subdirectory(doc)
    kdoctools_install(po)
else()

    message(WARNING "DocTools not found. NEED TO FIX IT IN QT6")
endif()

add_subdirectory(icons)
add_subdirectory(src)
#add_subdirectory(autotests)
if (UNIX)
    add_subdirectory(dolphinplugins)
endif (UNIX)


ecm_qt_install_logging_categories(
    EXPORT KOMMIT
    FILE kommit.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

if (KOMMIT_BUILD_DIR)
    file(WRITE ${KOMMIT_BUILD_DIR}/version "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
    message("-- Version in file ${KOMMIT_BUILD_DIR}/version saved")
endif()

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)

#CPack
set(CPACK_PACKAGE_NAME ${PROJECT_NAME}
    CACHE STRING "The resulting package name"
)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "KDE git gui client"
    CACHE STRING "Package description for the package metadata"
)
set(CPACK_PACKAGE_VENDOR "KDE")
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
#SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/_packages")

# https://unix.stackexchange.com/a/11552/254512
#set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/some")#/${CMAKE_PROJECT_VERSION}")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION   "Git client for KDE")

set(CPACK_PACKAGE_CONTACT "hamed.masafi@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Hamed Masafi")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
#set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if (KOMMIT_BUILD_DIR)
    set(CPACK_PACKAGE_FILE_NAME "kommit")
else()
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
endif()

set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
# without this you won't be able to pack only specified component
set(CPACK_DEB_COMPONENT_INSTALL YES)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
