/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandpull.h"

#ifdef GIT_GUI
#include "ui_commandpullwidget.h"
#include <KLocalizedString>
#include <KMessageBox>
#endif

namespace Git
{

bool CommandPull::squash() const
{
    return mSquash;
}

void CommandPull::setSquash(bool newSquash)
{
    mSquash = newSquash;
}

bool CommandPull::noFf() const
{
    return mNoFf;
}

void CommandPull::setNoFf(bool newNoFf)
{
    mNoFf = newNoFf;
}

bool CommandPull::ffOnly() const
{
    return mFfOnly;
}

void CommandPull::setFfOnly(bool newFfOnly)
{
    mFfOnly = newFfOnly;
}

bool CommandPull::noCommit() const
{
    return mNoCommit;
}

void CommandPull::setNoCommit(bool newNoCommit)
{
    mNoCommit = newNoCommit;
}

bool CommandPull::prune() const
{
    return mPrune;
}

void CommandPull::setPrune(bool newPrune)
{
    mPrune = newPrune;
}

bool CommandPull::tags() const
{
    return mTags;
}

void CommandPull::setTags(bool newTags)
{
    mTags = newTags;
}

void CommandPull::parseOutput(const QByteArray &output, const QByteArray &errorOutput)
{
    Q_UNUSED(errorOutput)
#ifdef GIT_GUI
    if (output.contains("Already up to date.")) {
        mUi->labelStatus->setText(i18n("Already up to date."));
    }
    if (errorOutput.startsWith("fatal:")) {
        mUi->labelStatus->setText(errorOutput.mid(6));
        KMessageBox::error(mWidget, errorOutput.mid(6), i18n("Error"));
    }
#endif
}

bool CommandPull::supportWidget() const
{
    return true;
}

QWidget *CommandPull::createWidget()
{
#ifdef GIT_GUI
    mWidget = new QWidget;
    mUi = new Ui::CommandPullWidget;
    mUi->setupUi(mWidget);
    return mWidget;
#else
    return nullptr;
#endif
}

const QString &CommandPull::remote() const
{
    return mRemote;
}

void CommandPull::setRemote(const QString &newRemote)
{
    mRemote = newRemote;
}

const QString &CommandPull::branch() const
{
    return mBranch;
}

void CommandPull::setBranch(const QString &newBranch)
{
    mBranch = newBranch;
}

QStringList CommandPull::generateArgs() const
{
    QStringList args{QStringLiteral("pull"), mRemote, mBranch};
    if (mSquash)
        args.append(QStringLiteral("--squash"));
    if (mNoFf)
        args.append(QStringLiteral("--no-ff"));
    if (mFfOnly)
        args.append(QStringLiteral("--ff-only"));
    if (mNoCommit)
        args.append(QStringLiteral("--no-commit"));
    if (mPrune)
        args.append(QStringLiteral("--prune"));
    if (mTags)
        args.append(QStringLiteral("--tags"));
    return args;
}

CommandPull::CommandPull() = default;

CommandPull::~CommandPull()
{
#ifdef GIT_GUI
    if (mWidget)
        mWidget->deleteLater();

    delete mUi;
#endif
}

} // namespace Git
