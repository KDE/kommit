/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QDateTime>
#include <QString>

namespace Git
{

class Manager;
class Stash
{

public:
    Stash(Git::Manager *git, QString name);

    void apply();
    void drop();
    void pop();
    const QString &name() const;
    const QString &authorName() const;
    const QString &authorEmail() const;
    const QString &subject() const;
    const QString &branch() const;
    const QDateTime &pushTime() const;

    friend class Manager;
    friend class StashesModel;
private:
    Git::Manager *mGit = nullptr;
    QString mName;
    QString mAuthorName;
    QString mAuthorEmail;
    QString mSubject;
    QString mBranch;
    QDateTime mPushTime;
};

} // namespace Git
