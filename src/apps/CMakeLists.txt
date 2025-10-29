# SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause
include_directories(${CMAKE_BINARY_DIR})
add_executable(kommit main.cpp)

target_link_libraries(kommit
        libkommit
        libkommitdiff
        libkommitgui
        KF${KF_MAJOR_VERSION}::IconThemes
)

install(TARGETS kommit ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
