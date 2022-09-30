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

    QStringList generateArgs() const override;

    bool noFf() const;
    void setNoFf(bool newNoFf);

    bool ffOnly() const;
    void setFfOnly(bool newFfOnly);

    bool noCommit() const;
    void setNoCommit(bool newNoCommit);

    bool prune() const;
    void setPrune(bool newPrune);

    bool tags() const;
    void setTags(bool newTags);

    void parseOutput(const QByteArray &output, const QByteArray &errorOutput) override;

    const QString &remote() const;
    void setRemote(const QString &newRemote);

    const QString &branch() const;
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
