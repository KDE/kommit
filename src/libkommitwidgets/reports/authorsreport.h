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

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    enum AuthorsReportRoles {
        Name,
        Email,
        Commits,
        Autheds,
        Tags,
        LastColumn,
    };

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

    void findOrCreate(QSharedPointer<Git::Signature> signature, AuthorCreateReason reason);
    QList<Author *> mData;
};
