/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "widgetbase.h"

#include "gitmanager.h"

#include "appwindow.h"
#include <QDialog>
#include <QHeaderView>
#include <QPushButton>
#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>

WidgetBase::WidgetBase(QWidget *parent)
    : QWidget(parent)
{
    // TODO: remove this singelton call
    mGit = Git::Manager::instance();
    connect(mGit, &Git::Manager::pathChanged, this, &WidgetBase::gitPathChanged);
}

WidgetBase::WidgetBase(Git::Manager *git, AppWindow *parent)
    : QWidget(parent)
    , mGit(git)
    , mParent{parent}
{
    // TODO: do we need this?
    if (!mGit)
        mGit = Git::Manager::instance();
    connect(mGit, &Git::Manager::pathChanged, this, &WidgetBase::gitPathChanged);
}

Git::Manager *WidgetBase::git() const
{
    return mGit;
}

void WidgetBase::setGit(Git::Manager *newGit)
{
    mGit = newGit;
}

void WidgetBase::reload()
{
}

int WidgetBase::exec(QWidget *parent)
{
    auto oldParent = this->parentWidget();
    QDialog d(parent);
    QVBoxLayout layout(&d);
    this->setParent(&d);
    layout.addWidget(this);
    //    layout.addWidget(new QPushButton(&d));
    int ret = d.exec();
    setParent(oldParent);
    return ret;
}

void WidgetBase::saveState(QSettings &settings) const
{
    Q_UNUSED(settings)
}

void WidgetBase::restoreState(QSettings &settings)
{
    Q_UNUSED(settings)
}

void WidgetBase::save(QSettings &settings, QSplitter *splitter) const
{
    settings.setValue(stateName(splitter), splitter->saveState());
}

void WidgetBase::restore(QSettings &settings, QSplitter *splitter)
{
    auto orientation = splitter->orientation();
    splitter->restoreState(settings.value(stateName(splitter)).toByteArray());
    splitter->setOrientation(orientation);
}

void WidgetBase::save(QSettings &settings, QTreeView *treeView) const
{
    settings.setValue(stateName(treeView), treeView->header()->saveState());
}

void WidgetBase::restore(QSettings &settings, QTreeView *treeView)
{
    treeView->header()->restoreState(settings.value(stateName(treeView)).toByteArray());
}

void WidgetBase::settingsUpdated()
{
}

void WidgetBase::gitPathChanged()
{
    reload();
}

QString WidgetBase::stateName(QWidget *w) const
{
    return QStringLiteral("%1_%2_state").arg(metaObject()->className(), w->objectName());
}

#include "moc_widgetbase.cpp"
