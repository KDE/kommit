/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "abstractcommand.h"

#include <utility>

namespace Git
{

AbstractCommand::~AbstractCommand() = default;

bool AbstractCommand::isValid() const
{
    return mIsValid;
}

QWidget *AbstractCommand::createWidget()
{
    return nullptr;
}

int AbstractCommand::progress() const
{
    return m_progress;
}

AbstractCommand::Status AbstractCommand::status() const
{
    return mStatus;
}

void AbstractCommand::setStatus(Status newStatus)
{
    mStatus = newStatus;
}

const QString &AbstractCommand::errorMessage() const
{
    return mErrorMessage;
}

void AbstractCommand::setErrorMessage(const QString &newErrorMessage)
{
    mErrorMessage = newErrorMessage;
}

AbstractCommand::AbstractCommand(QObject *parent)
    : QObject(parent)
{
}

AbstractCommand::AbstractCommand(QStringList args)
    : QObject()
    , mArgs(std::move(args))
{
}

AbstractCommand::AbstractCommand(Manager *git)
    : QObject()
    , mGit(git)
{
}

bool AbstractCommand::parseOutput(const QByteArray &output, const QByteArray &errorOutput)
{
    Q_UNUSED(output)
    Q_UNUSED(errorOutput)
    return true;
}

void AbstractCommand::parseOutputSection(const QByteArray &output, const QByteArray &errorOutput)
{
    Q_UNUSED(output)
    Q_UNUSED(errorOutput)
}

void AbstractCommand::setProgress(int newProgress)
{
    if (m_progress == newProgress)
        return;
    m_progress = newProgress;
    Q_EMIT progressChanged(newProgress);
}

void AbstractCommand::appendBool(OptionalBool b, QStringList &cmd, const QString &name) const
{
    switch (b) {
    case OptionalBool::True:
        cmd.append(QStringLiteral("--") + name);
        break;
    case OptionalBool::False:
        cmd.append(QStringLiteral("--no-") + name);
        break;
    case OptionalBool::Unset:
        break;
    }
}

void AbstractCommand::appendBool(bool b, QStringList &cmd, const QString &name) const
{
    if (b)
        cmd.append(QStringLiteral("--") + name);
    else
        cmd.append(QStringLiteral("--no-") + name);
}

OptionalBool checkStateToOptionalBool(Qt::CheckState checkState)
{
    switch (checkState) {
    case Qt::Unchecked:
        return OptionalBool::False;
    case Qt::PartiallyChecked:
        return OptionalBool::Unset;
    case Qt::Checked:
        return OptionalBool::True;
    }
    return OptionalBool::Unset;
}

} // namespace Git

#include "moc_abstractcommand.cpp"
