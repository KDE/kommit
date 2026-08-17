/*
SPDX-FileCopyrightText: 2026 Méven Car <meven@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "linkify.h"

#include <QRegularExpression>

namespace Git
{

QString linkifyUrls(const QString &text)
{
    // Anything up to whitespace or a bracket that would not be part of an address.
    static const QRegularExpression url{QStringLiteral(R"((https?|ftp)://[^\s<>"'\)\]}]+)")};

    QString result;
    result.reserve(text.size() + 64);

    qsizetype from{0};
    auto matches = url.globalMatch(text);

    while (matches.hasNext()) {
        const auto match = matches.next();

        auto address = match.captured();
        // A sentence ending in an address leaves its full stop, and a message in brackets
        // leaves the closing one, outside the link.
        while (!address.isEmpty() && QStringLiteral(".,;:!?").contains(address.back()))
            address.chop(1);

        result += text.mid(from, match.capturedStart() - from).toHtmlEscaped();
        result += QStringLiteral("<a href=\"%1\">%1</a>").arg(address.toHtmlEscaped());

        from = match.capturedStart() + address.size();
    }

    result += text.mid(from).toHtmlEscaped();

    // The spacing and the line breaks of the text are what the writer put there, so they are
    // kept rather than folded the way rich text folds them by default.
    return QStringLiteral("<div style=\"white-space:pre-wrap\">%1</div>").arg(result);
}

}
