/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"
#include "libgitklient_export.h"
namespace Git
{

class LIBGITKLIENT_EXPORT CommandFetch : public AbstractCommand
{
public:
    CommandFetch();

    Q_REQUIRED_RESULT QStringList generateArgs() const override;

    Q_REQUIRED_RESULT bool noFf() const;
    void setNoFf(bool newNoFf);

    Q_REQUIRED_RESULT bool ffOnly() const;
    void setFfOnly(bool newFfOnly);

    Q_REQUIRED_RESULT bool noCommit() const;
    void setNoCommit(bool newNoCommit);

    Q_REQUIRED_RESULT bool prune() const;
    void setPrune(bool newPrune);

    Q_REQUIRED_RESULT bool tags() const;
    void setTags(bool newTags);

    void parseOutputSection(const QByteArray &output, const QByteArray &errorOutput) override;

    Q_REQUIRED_RESULT const QString &remote() const;
    void setRemote(const QString &newRemote);

    Q_REQUIRED_RESULT const QString &branch() const;
    void setBranch(const QString &newBranch);

private:
    bool mNoFf{false};
    bool mFfOnly{false};
    bool mNoCommit{false};
    bool mPrune{false};
    bool mTags{false};
    QString mRemote;
    QString mBranch;
};

} // namespace Git
