/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include "abstractreport.h"
#include <QDateTime>
#include <QSharedPointer>

namespace Git
{
class Signature;
};

class LIBKOMMIT_EXPORT AuthorsReport : public AbstractReport
{
    Q_OBJECT

public:
    AuthorsReport(Git::Manager *git, QObject *parent = nullptr);

    void reload() override;
    QString name() const override;

    int columnCount() const override;
    QStringList headerData() const override;

    bool supportChart() const override;
    QString axisXTitle() const override;
    QString axisYTitle() const override;

    int labelsAngle() const override;

private:
    struct DatesRange {
        int count = 0;
        QDateTime first;
        QDateTime last;

        void begin(const QDateTime &time);
        void increase(const QDateTime &time);
    };

    struct Author {
        QString email;
        QString name;

        DatesRange commits;
        DatesRange authoredCommits;
        DatesRange tags;
    };

    enum AuthorCreateReason {
        Commit,
        AuthoredCommit,
        Tag,
    };

    Author *findOrCreate(QSharedPointer<Git::Signature> signature, AuthorCreateReason reason);
    QList<Author *> mData;
};
