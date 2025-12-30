# SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

find_package(Git)

if(Git_FOUND)
    message(STATUS "Git found: ${GIT_EXECUTABLE}")
    message(STATUS "Git version: ${GIT_VERSION_STRING}")

    # Set a compile definition
    add_definitions(-DGIT_FOUND)

    # Or use target_compile_definitions for specific targets
    # target_compile_definitions(my_target PRIVATE GIT_FOUND)

    # Set the Git path as a definition (escaped for C string)
    string(REPLACE "\\" "\\\\" GIT_PATH_ESCAPED "${GIT_EXECUTABLE}")
    add_definitions(-DGIT_BINARY_PATH="${GIT_PATH_ESCAPED}")

else()
    message(WARNING "Git not found!")
endif()

macro(add_libkommit_test _source)
    set(_test ${_source})
    get_filename_component(_name ${_source} NAME_WE)
    add_executable(${_name} ${_test} ${ARGN} ${_name}.h)
    add_test(NAME ${_name} COMMAND ${_name})
    ecm_mark_as_test(${_name})
    target_link_libraries(${_name} Qt::Test libkommit libkommitTestsCommon)
endmacro()

add_subdirectory(libkommittestscommon)
add_libkommit_test(tagtest.cpp)
add_libkommit_test(clonetest.cpp)
add_libkommit_test(indextest.cpp)
add_libkommit_test(clonecommandtest.cpp)
add_libkommit_test(commandcleantest.cpp)
add_libkommit_test(committest.cpp)
add_libkommit_test(commandpushtest.cpp)
add_libkommit_test(addsubmodulecommandtest.cpp)
add_libkommit_test(branchestest.cpp)
add_libkommit_test(configtest.cpp)
add_libkommit_test(remotetest.cpp)
add_libkommit_test(stashtest.cpp)
add_libkommit_test(submoduletest.cpp)
add_libkommit_test(filetest.cpp)
add_libkommit_test(branchesdifftest.cpp)
add_libkommit_test(notetest.cpp)
add_libkommit_test(cachetest.cpp)
add_libkommit_test(switchtest.cpp)
add_libkommit_test(signaturetest.cpp)
add_libkommit_test(pushtest.cpp)
add_libkommit_test(changedfilestest.cpp)
add_libkommit_test(mergetest.cpp)
