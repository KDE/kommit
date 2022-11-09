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

void CommandPull::parseOutputSection(const QByteArray &output, const QByteArray &errorOutput)
{
#ifdef GIT_GUI
    if (output.contains("Already up to date.")) {
        mUi->labelStatus->setText(i18n("Already up to date."));
    }
    if (errorOutput.startsWith("fatal:")) {
        mUi->labelStatus->setText(errorOutput.mid(6));
        KMessageBox::error(mWidget, errorOutput.mid(6), i18n("Error"));
    }
#else
    Q_UNUSED(output)
    Q_UNUSED(errorOutput)
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

CommandPull::Rebase CommandPull::rebase() const
{
    return mRebase;
}

void CommandPull::setRebase(Rebase newRebase)
{
    mRebase = newRebase;
}

CommandPull::FastForward CommandPull::fastForward() const
{
    return mFastForward;
}

void CommandPull::setFastForward(FastForward newFastForward)
{
    mFastForward = newFastForward;
}

QStringList CommandPull::generateArgs() const
{
    QStringList args{QStringLiteral("pull"), mRemote, mBranch};
    if (mSquash)
        args.append(QStringLiteral("--squash"));
    if (mNoCommit)
        args.append(QStringLiteral("--no-commit"));
    if (mPrune)
        args.append(QStringLiteral("--prune"));
    if (mTags)
        args.append(QStringLiteral("--tags"));

    switch (mFastForward) {
    case Unset:
        break;
    case Yes:
        args << QStringLiteral("--ff");
        break;
    case No:
        args << QStringLiteral("--no-ff");
        break;
    case OnlyFastForward:
        args << QStringLiteral("--ff-only");
        break;
    }

    switch (mRebase) {
    case None:
        break;
    case False:
        args << QStringLiteral("--rebase=false");
        break;
    case True:
        args << QStringLiteral("--rebase=true");
        break;
    case Preserve:
        args << QStringLiteral("--rebase=preserve");
        break;
    case Merge:
        args << QStringLiteral("--rebase=merge");
        break;
    }

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
