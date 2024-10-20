/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"
#include "libkommitwidgets_export.h"

#include <QSharedPointer>

#include <Kommit/Commit>

namespace Git
{
class Commit;
}

class LIBKOMMITWIDGETS_EXPORT CommitActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit CommitActions(Git::Repository *git, QWidget *parent = nullptr);

    [[nodiscard]] const Git::Commit &commit() const;
    void setCommit(const Git::Commit &commit);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void browse();
    LIBKOMMITWIDGETS_NO_EXPORT void checkout();
    LIBKOMMITWIDGETS_NO_EXPORT void diff();
    LIBKOMMITWIDGETS_NO_EXPORT void note();

    DEFINE_ACTION(actionBrowse)
    DEFINE_ACTION(actionCheckout)
    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionNote)
    Git::Commit mCommit;
};
