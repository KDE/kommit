/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libkommitgui_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
#ifndef LIBKOMMITGUI_TESTS_EXPORT
#define LIBKOMMITGUI_TESTS_EXPORT LIBKOMMITGUI_EXPORT
#endif
#else /* not compiling tests */
#define LIBKOMMITGUI_TESTS_EXPORT
#endif
