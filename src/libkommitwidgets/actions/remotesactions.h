/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"
#include "libkommitwidgets_export.h"

namespace Git
{
class Remote;
}

class LIBKOMMITWIDGETS_EXPORT RemotesActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit RemotesActions(Git::Manager *git, QWidget *parent = nullptr);

    [[nodiscard]] const QString &remoteName() const;
    void setRemoteName(const QString &newRemoteName);

    [[nodiscard]] QSharedPointer<Git::Remote> remote() const;
    void setRemote(QSharedPointer<Git::Remote> newRemote);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void create();
    LIBKOMMITWIDGETS_NO_EXPORT void remove();
    LIBKOMMITWIDGETS_NO_EXPORT void changeUrl();
    LIBKOMMITWIDGETS_NO_EXPORT void rename();
    LIBKOMMITWIDGETS_NO_EXPORT void update();
    DEFINE_ACTION(actionCreate)
    DEFINE_ACTION(actionRemove)
    DEFINE_ACTION(actionRename)
    DEFINE_ACTION(actionUpdate)
    DEFINE_ACTION(actionChangeUrl)

    QString mRemoteName;
    QSharedPointer<Git::Remote> mRemote;
};
