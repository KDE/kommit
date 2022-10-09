# SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause
add_executable(gitklient main.cpp)

target_link_libraries(gitklient
	libgitklient
	libgitklientgui
)

install(TARGETS gitklient ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
