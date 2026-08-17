/*
SPDX-FileCopyrightText: 2026 Méven Car <meven@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"

#include <QString>

namespace Git
{

/**
 * @p text as rich text, with every web address in it turned into a link.
 *
 * Spacing and line breaks are kept as they were written, so a message hard wrapped at
 * seventy characters still reads that way, and the trailers a review leaves behind, the
 * "Reviewed-on" line and the like, can be followed to the page they name.
 */
[[nodiscard]] LIBKOMMITWIDGETS_EXPORT QString linkifyUrls(const QString &text);

}
