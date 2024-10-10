/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"
#include "libkommitwidgets_export.h"

namespace Git
{
class Tag;
};

class LIBKOMMITWIDGETS_EXPORT TagsActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit TagsActions(Git::Repository *git, QWidget *parent = nullptr);

    [[nodiscard]] const QString &tagName() const;
    void setTagName(const QString &newTagName);

    [[nodiscard]] QSharedPointer<Git::Tag> tag() const;
    void setTag(QSharedPointer<Git::Tag> tag);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void create();
    LIBKOMMITWIDGETS_NO_EXPORT void remove();
    LIBKOMMITWIDGETS_NO_EXPORT void checkout();
    LIBKOMMITWIDGETS_NO_EXPORT void diff();
    LIBKOMMITWIDGETS_NO_EXPORT void push();
    DEFINE_ACTION(actionCreate)
    DEFINE_ACTION(actionRemove)
    DEFINE_ACTION(actionCheckout)
    DEFINE_ACTION(actionDiff)
    DEFINE_ACTION(actionPush)
    QSharedPointer<Git::Tag> mTag;
};
