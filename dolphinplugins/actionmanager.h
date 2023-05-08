/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "actions.h"
#include "statuscache.h"
#include <KAbstractFileItemActionPlugin>
#include <KFileItem>

class ActionManager : public KAbstractFileItemActionPlugin
{
    Q_OBJECT
public:
    explicit ActionManager(QObject *parent, const QList<QVariant> &);

    void addMenu(QMenu *menu, const QString &title, const QStringList &args, const QString &icon = QString());
    QList<QAction *> actions(const KFileItemListProperties &fileItemInfos, QWidget *parentWidget) override;

private:
    void initActions();

private Q_SLOTS:
#define f(name, text, args, icon) void name##Clicked()
    ACTIONS_FOR_EACH(f)
#undef f
private:
    StatusCache mCache;

#define decl(name, text, args, icon) QAction *name;
    ACTIONS_FOR_EACH(decl)
#undef decl

    QAction *mMainAction = nullptr;

    QString mPath;
};
