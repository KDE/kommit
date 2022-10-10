/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "libgitklientgui_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
#ifndef LIBGITKLIENTGUI_TESTS_EXPORT
#define LIBGITKLIENTGUI_TESTS_EXPORT LIBGITKLIENTGUI_EXPORT
#endif
#else /* not compiling tests */
#define LIBGITKLIENTGUI_TESTS_EXPORT
#endif
