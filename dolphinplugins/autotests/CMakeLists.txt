# SPDX-FileCopyrightText: 2022 Hamed Masafi <hamed.masafi@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

macro(add_dolphinplugins_test _source)
    set( _test ${_source} gittestmanager.cpp gittestmanager.h ../statuscache.cpp ../manager.cpp ../dolphinplugin.cpp ../dolphinplugin.h)
    get_filename_component( _name ${_source} NAME_WE )
    add_executable( ${_name} ${_test} ${ARGN} ${_name}.h)
    add_test(NAME ${_name} COMMAND ${_name} )
    ecm_mark_as_test(${_name})
    target_link_libraries( ${_name} Qt::Test libkommitgui DolphinVcs PkgConfig::LIBGIT2)
endmacro()

add_dolphinplugins_test(overlaytest.cpp)
