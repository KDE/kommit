/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractgititemsmodel.h"
#include "entities/signature.h"

#include "libkommit_export.h"

#include <QDateTime>
#include <QMutex>
#include <QSharedPointer>

namespace Git
{

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

// TODO: delete me
class LIBKOMMIT_EXPORT AuthorsModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    enum AuthorsModelsRoles {
        Name,
        Email,
        Commits,
        Autheds,
        Tags,
        LastColumn,
    };
    enum AuthorCreateReason {
        Commit,
        AuthoredCommit,
        Tag,
    };

    Q_ENUM(AuthorsModelsRoles)
    explicit AuthorsModel(Manager *git, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Author *findOrCreate(const QString &name, const QString &email);

    Q_DECL_DEPRECATED_X("Use smart pointer overload")
    Author *findOrCreate(const QString &name, const QString &email, const QDateTime &time, AuthorCreateReason reason);
    Author *findOrCreate(QSharedPointer<Signature> signature, AuthorCreateReason reason);
    void clear();

protected:
    void fill() override;

private:
    QList<Author *> mData;
    QMutex mDataMutex;
};

} // namespace Git
