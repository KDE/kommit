/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libgitklient_export.h"
#include <QByteArray>
#include <QObject>
#include <QStringList>

class QWidget;

namespace Git
{

enum class OptionalBool {
    False,
    Unset,
    True,
};

enum class FastForwardType { Unset, Yes, No, OnlyFastForward };

LIBGITKLIENT_EXPORT OptionalBool checkStateToOptionalBool(Qt::CheckState checkState);

class Manager;
class LIBGITKLIENT_EXPORT AbstractCommand : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    enum Status { None, InvalidArgs, Ready, Running, Finished, Error };

    explicit AbstractCommand(QObject *parent = nullptr);
    explicit AbstractCommand(QStringList args);
    explicit AbstractCommand(Manager *git);

    virtual ~AbstractCommand();

    virtual QStringList generateArgs() const = 0;

    virtual bool parseOutput(const QByteArray &output, const QByteArray &errorOutput);
    virtual void parseOutputSection(const QByteArray &output, const QByteArray &errorOutput);

    bool isValid() const;

    virtual bool supportWidget() const
    {
        return false;
    }
    virtual bool supportProgress() const
    {
        return false;
    }
    virtual QWidget *createWidget();
    int progress() const;

    Q_REQUIRED_RESULT Status status() const;
    void setStatus(Status newStatus);

    Q_REQUIRED_RESULT const QString &errorMessage() const;
    void setErrorMessage(const QString &newErrorMessage);

Q_SIGNALS:
    void progressChanged(int progress);

protected:
    QStringList mArgs;
    Manager *mGit{nullptr};
    void setProgress(int newProgress);
    void appendBool(OptionalBool b, QStringList &cmd, const QString &name) const;
    void appendBool(bool b, QStringList &cmd, const QString &name) const;

private:
    bool mIsValid{false};
    int m_progress{0};
    Status mStatus{None};
    QString mErrorMessage;
};

} // namespace Git
