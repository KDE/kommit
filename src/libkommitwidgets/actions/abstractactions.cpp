/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "abstractactions.h"

#include "repository.h"

#include <KLocalizedString>
#include <QMenu>
#include <QWidget>

AbstractActions::AbstractActions(Git::Repository *git, QWidget *parent)
    : QObject{parent}
    , mGit{git}
    , mParent{parent}
    , mMenu(new QMenu(parent))
{
    connect(git, &Git::Repository::pathChanged, this, &AbstractActions::gitReloaded);
}

void AbstractActions::setActionEnabled(QAction *action, bool enabled)
{
    if (mGit->isValid()) {
        action->setEnabled(enabled);
    } else {
        action->setEnabled(false);
        mActionStatuses.insert(action, enabled);
    }
}

void AbstractActions::popup()
{
    mMenu->popup(QCursor::pos());
}

void AbstractActions::popup(const QPoint &pos)
{
    mMenu->popup(pos);
}

void AbstractActions::gitReloaded()
{
    if (!mGit->isValid()) {
        for (auto &a : std::as_const(mActions))
            a->setEnabled(false);
        mActionStatuses.clear();
        return;
    }

    for (auto i = mActionStatuses.begin(); i != mActionStatuses.end(); ++i) {
        i.key()->setEnabled(i.value());
    }

    mActionStatuses.clear();
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

#include "moc_abstractactions.cpp"
