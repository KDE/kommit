# SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause
macro(add_libkommitgui_test _source)
    set(_test ${_source})
    get_filename_component(_name ${_source} NAME_WE)
    add_executable(${_name} ${_test} ${ARGN} ${_name}.h)
    add_test(NAME ${_name} COMMAND ${_name})
    ecm_mark_as_test(${_name})
    target_link_libraries(${_name} Qt::Test libkommitgui)
endmacro()
add_libkommitgui_test(initdialogtest.cpp)
add_libkommitgui_test(clonedialogtest.cpp)
add_libkommitgui_test(filesstatuseslisttest.cpp)
