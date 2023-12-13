include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libkommitwidgets SHARED)
generate_export_header(libkommitwidgets BASE_NAME libkommitwidgets)

macro(kommitwidgets_add_dir dir)
    file(GLOB ${dir}_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*.h)
    file(GLOB ${dir}_UI_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*.ui)
endmacro()

kommitwidgets_add_dir(core)
kommitwidgets_add_dir(widgets)
kommitwidgets_add_dir(windows)
kommitwidgets_add_dir(models)
kommitwidgets_add_dir(dialogs)
kommitwidgets_add_dir(actions)

target_sources(libkommitwidgets PRIVATE
    kommitwidgetsglobaloptions.h kommitwidgetsglobaloptions.cpp

    ${core_FILES}
    ${widgets_FILES}
    ${windows_FILES}
    ${dialogs_FILES}
    ${models_FILES}
    ${actions_FILES}
)

target_link_libraries(libkommitwidgets
    Qt::Core
    Qt::Widgets
    Qt::Concurrent
    Qt::Network

    libkommit
    libkommitdiff

    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::SyntaxHighlighting
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::KIOWidgets
    KF${KF_MAJOR_VERSION}::Parts

)

install(TARGETS libkommitwidgets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

ecm_qt_declare_logging_category(libkommitwidgets
    HEADER libkommitwidgets_appdebug.h
    IDENTIFIER KOMMIT_WIDGETS_LOG
    CATEGORY_NAME "org.kde.kommit.widgets" DESCRIPTION "kommit" EXPORT KOMMIT
)

add_compile_definitions(LIBKOMMIT_WIDGET_USE_KF)

ki18n_wrap_ui(
    libkommitwidgets

    ${widgets_UI_FILES}
    ${windows_UI_FILES}
    ${dialogs_UI_FILES}
    ${models_UI_FILES}
)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(libkommitwidgets PROPERTIES UNITY_BUILD ON)
endif()

set_target_properties(libkommitwidgets
    PROPERTIES OUTPUT_NAME kommitwidgets VERSION ${LIBKOMMIT_LIB_VERSION} SOVERSION ${LIBKOMMIT_LIB_SOVERSION}
)

set_target_properties(
    libkommitwidgets PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.kde.kommit
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# if (BUILD_TESTING)
#     add_subdirectory(autotests)
# endif()
