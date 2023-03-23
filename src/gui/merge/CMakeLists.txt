set(kommit_SRCS
    ${kommit_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/kommitmergewindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/closeeventdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openfilesdialog.cpp
    PARENT_SCOPE
)

ki18n_wrap_ui(
    ${CMAKE_CURRENT_SOURCE_DIR}/kommitmergeview.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/closeeventdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/openfilesdialog.ui
)
