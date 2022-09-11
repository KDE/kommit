/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "abstractactions.h"

#include "git/gitmanager.h"

#include <klocalizedstring.h>
#include <QMenu>
#include <QWidget>

void AbstractActions::setActionEnabled(QAction *action, bool enabled)
{
    if (mGit->isValid()) {
        action->setEnabled(enabled);
    } else {
        action->setEnabled(false);
        _actionStatuses.insert(action, enabled);
    }
}

AbstractActions::AbstractActions(Git::Manager *git, QWidget *parent)
    : QObject{parent}, mGit{git} , mParent{parent}
{
    mMenu = new QMenu(parent);
    connect(git, &Git::Manager::pathChanged, this, &AbstractActions::git_reloaded);
}

void AbstractActions::popup()
{
    mMenu->popup(QCursor::pos());
}

void AbstractActions::popup(const QPoint &pos)
{
    mMenu->popup(pos);
}

void AbstractActions::git_reloaded()
{
    if (!mGit->isValid()) {
        for (auto &a: qAsConst(mActions))
            a->setEnabled(false);
        _actionStatuses.clear();
        return;
    }

    for (auto i = _actionStatuses.begin(); i != _actionStatuses.end(); ++i) {
        i.key()->setEnabled(i.value());
    }

    _actionStatuses.clear();
}

QAction *AbstractActions::createAction(const QString &text, bool enabled, bool addToMenu)
{
    auto a = new QAction(this);
    a->setText(text);
    setActionEnabled(a, enabled);
    if (addToMenu)
        mMenu->addAction(a);
    mActions.append(a);
    return a;
}
