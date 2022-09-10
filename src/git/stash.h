/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QDateTime>
#include <QString>


namespace Git {

class Manager;
class Stash
{
    Git::Manager *_git;
    QString _name;
    QString _authorName;
    QString _authorEmail;
    QString _subject;
    QString _branch;
    QDateTime _pushTime;
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
};

} // namespace Git

