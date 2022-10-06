cmake_minimum_required(VERSION 3.16)

project(gitklient)
set(PROJECT_VERSION 5)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 4)
set(PROJECT_VERSION_PATCH 0)

set (RELEASE_SERVICE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set(LIBGITKLIENT_LIB_VERSION "${RELEASE_SERVICE_VERSION}")
set(LIBGITKLIENT_LIB_SOVERSION "0")


add_definitions(-DGK_VERSION="${RELEASE_SERVICE_VERSION}")
set(REQUIRED_KF5_VERSION "5.80")

find_package(ECM ${REQUIRED_KF5_VERSION} REQUIRED NO_MODULE)
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


set (CMAKE_CXX_STANDARD 17)

if(NOT DEFINED ${QT_MAJOR_VERSION})
  set(QT_MAJOR_VERSION 5)
endif()

set(QT_MIN_VERSION 5.12.0)
find_package(
    Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION}
    CONFIG REQUIRED
        Core
        Gui
        Widgets
        Concurrent
        Test
)

set_package_properties(
    Qt${QT_VERSION_MAJOR} PROPERTIES
    PURPOSE "Required for tests"
    TYPE OPTIONAL
)


find_package(
    KF5 ${REQUIRED_KF5_VERSION}
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

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if (USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
endif()

if(BUILD_TESTING)
   add_definitions(-DBUILD_TESTING)
endif(BUILD_TESTING)


if(KF5DocTools_FOUND)
    add_subdirectory(doc)
    kdoctools_install(po)
else()
    message(WARNING "DocTools not found.")
endif()

add_subdirectory(icons)
add_subdirectory(src)
#add_subdirectory(autotests)
if (UNIX)
    add_subdirectory(dolphinplugins)
endif (UNIX)


ecm_qt_install_logging_categories(
        EXPORT GITKLIENT
        FILE gitklient.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

if (GITKLIENT_BUILD_DIR)
    file(WRITE ${GITKLIENT_BUILD_DIR}/version "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
    message("-- Version in file ${GITKLIENT_BUILD_DIR}/version saved")
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

if (GITKLIENT_BUILD_DIR)
    set(CPACK_PACKAGE_FILE_NAME "gitklient")
else()
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
endif()

set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
# without this you won't be able to pack only specified component
set(CPACK_DEB_COMPONENT_INSTALL YES)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
