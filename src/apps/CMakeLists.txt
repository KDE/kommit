# SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause
add_executable(kommit main.cpp)

target_link_libraries(kommit
        libkommit
        libkommitdiff
        libkommitgui
)

install(TARGETS kommit ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
