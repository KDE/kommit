/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changelogsdata.h"

#include <KLocalizedString>

class ChangeLogsDataPrivate
{
    ChangeLogsData *const q_ptr;
    Q_DECLARE_PUBLIC(ChangeLogsData)
public:
    explicit ChangeLogsDataPrivate(ChangeLogsData *parent);

    struct Row {
        QVersionNumber version;
        QStringList changes;
    };
    QList<Row> data;

    void fill();
};

ChangeLogsData::ChangeLogsData()
    : d_ptr{new ChangeLogsDataPrivate{this}}
{
    Q_D(ChangeLogsData);
    d->fill();
}

ChangeLogsData::~ChangeLogsData()
{
}

QString ChangeLogsData::generateMarkdown(const QVersionNumber &until) const
{
    Q_D(const ChangeLogsData);
    QString s;
    for (const auto &row : std::as_const(d->data))
        if (until.isNull() || row.version >= until)
            s.append(QStringLiteral("## %1 %2 \n - %3\n").arg(i18n("Version"), row.version.toString(), row.changes.join(QStringLiteral("\n - "))));

    return s;
}

ChangeLogsDataPrivate::ChangeLogsDataPrivate(ChangeLogsData *parent)
    : q_ptr{parent}
{
}

void ChangeLogsDataPrivate::fill()
{
    // clang-format off
    // Add change logs here

    data << Row{
        QVersionNumber{1, 6, 1},
        QStringList{
            i18n("Improve graph painting"),
            i18n("Improve on kommit lib (remove extra dependencies like kf)"),
            i18n("Add changes logs dialog"),
            i18n("Fix many small bugs")
        }
    };

    data << Row{
        QVersionNumber{1, 6, 2},
        QStringList{
            i18n("Fix bug in branch selection in commits widget")
        }
    };

    data << Row{
        QVersionNumber{1, 7, 0},
        QStringList{
            i18n("Improve merge algorithm and text rendering"),
            i18n("Improve diff window text rendering"),
            i18n("Fix dolphin menu")
        }
    };

    data << Row{
        QVersionNumber{1, 8, 0},
        QStringList{
            i18n("Change icons to symbolic to reponse the theme changes"),
            i18n("Fix crash on diff page"),
            i18n("Fix missing icons")
        }
    };

    // clang-format on

    std::sort(data.begin(), data.end(), [](const Row &r1, const Row &r2) {
        return r1.version > r2.version;
    });
}
