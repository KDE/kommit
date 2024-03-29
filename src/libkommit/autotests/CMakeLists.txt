# SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause
macro(add_LIBKOMMIT_test _source)
    set( _test ${_source} testcommon.cpp testcommon.h)
    get_filename_component( _name ${_source} NAME_WE )
    add_executable( ${_name} ${_test} ${ARGN} ${_name}.h)
    add_test(NAME ${_name} COMMAND ${_name} )
    ecm_mark_as_test(${_name})
    target_link_libraries( ${_name} Qt::Test libkommit)
endmacro()

add_LIBKOMMIT_test(tagtest.cpp)
add_LIBKOMMIT_test(clonetest.cpp)
add_LIBKOMMIT_test(indextest.cpp)
add_LIBKOMMIT_test(clonecommandtest.cpp)
add_LIBKOMMIT_test(commandcleantest.cpp)
add_LIBKOMMIT_test(commandpushtest.cpp)
add_LIBKOMMIT_test(addsubmodulecommandtest.cpp)
add_LIBKOMMIT_test(branchestest.cpp)
add_LIBKOMMIT_test(configtest.cpp)
add_LIBKOMMIT_test(remotetest.cpp)
add_LIBKOMMIT_test(stashtest.cpp)
add_LIBKOMMIT_test(submoduletest.cpp)
add_LIBKOMMIT_test(filetest.cpp)
add_LIBKOMMIT_test(branchesdifftest.cpp)
