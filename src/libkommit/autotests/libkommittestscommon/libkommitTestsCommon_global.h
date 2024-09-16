/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QtCore/qglobal.h>

#if defined(LIBKOMMITTESTSCOMMON_LIBRARY)
#define LIBKOMMITTESTSCOMMON_EXPORT Q_DECL_EXPORT
#else
#define LIBKOMMITTESTSCOMMON_EXPORT Q_DECL_IMPORT
#endif
