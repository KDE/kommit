/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractaction.h"

#include "libkommit_export.h"

#include <QDateTime>
#include <QSharedPointer>

#include <Kommit/Branch>

namespace Git {

class CommitActionPrivate;

class LIBKOMMIT_EXPORT CommitAction : public AbstractAction
{
public:
    CommitAction(Repository *repo);
    explicit CommitAction(const CommitAction &other) = default;
    explicit CommitAction(CommitAction &&other) noexcept = default;
    CommitAction &operator=(const CommitAction &other) = default;
    CommitAction &operator=(CommitAction &&other) noexcept = default;
    ~CommitAction() = default;

    bool amend() const;
    void setAmend(const bool &amend);

    bool allowEmptyCommit() const;
    void setAllowEmptyCommit(const bool &allowEmptyCommit);

    QString message() const;
    void setMessage(const QString &message);

    QString authorName() const;
    void setAuthorName(const QString &authorName);

    QString authorEmail() const;
    void setAuthorEmail(const QString &authorEmail);

    QDateTime authTime() const;
    void setAuthTime(const QDateTime &authTime);

    QString committerName() const;
    void setCommitterName(const QString &committerName);

    QString committerEmail() const;
    void setCommitterEmail(const QString &committerEmail);

    QDateTime commitTime() const;
    void setCommitTime(const QDateTime &commitTime);

    QString reflogMessage() const;
    void setReflogMessage(const QString &reflogMessage);

    Branch branch() const;
    void setBranch(const Branch &branch);

protected:
    int exec();

private:
    QSharedPointer<CommitActionPrivate> d;
};
}
