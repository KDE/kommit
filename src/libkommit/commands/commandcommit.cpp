/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandcommit.h"

#include <QDebug>

namespace Git
{

CommandCommit::CommandCommit() = default;

QStringList CommandCommit::generateArgs() const
{
    QStringList args{QStringLiteral("commit"), QStringLiteral("--message"), mMessage};

    if (mAmend)
        args.append(QStringLiteral("--amend"));

    switch (mIncludeStatus) {
    case OptionalBool::True:
        args.append(QStringLiteral("--status"));
        break;
    case OptionalBool::False:
        args.append(QStringLiteral("--no-status"));
        break;
    case OptionalBool::Unset:
        break;
    }
    return args;
}

const QString &CommandCommit::message() const
{
    return mMessage;
}

void CommandCommit::setMessage(const QString &newMessage)
{
    mMessage = newMessage;
}

bool CommandCommit::amend() const
{
    return mAmend;
}

void CommandCommit::setAmend(bool newAmend)
{
    mAmend = newAmend;
}

OptionalBool CommandCommit::includeStatus() const
{
    return mIncludeStatus;
}

void CommandCommit::setIncludeStatus(OptionalBool newIncludeStatus)
{
    mIncludeStatus = newIncludeStatus;
}

bool CommandCommit::parseOutput(const QByteArray &output, const QByteArray &errorOutput)
{
    Q_UNUSED(errorOutput);
    auto lines = output.split('\n');
    QString error;
    bool errorFound = false;
    for (const auto &line : lines) {
        if (errorFound)
            error.append(QLatin1Char('/') + line);
        if (line.startsWith("ERROR: ")) {
            errorFound = true;
            error = line.mid(7);
        }
    }
    if (errorFound)
        setErrorMessage(error);
    return !errorFound;
}

} // namespace Git
