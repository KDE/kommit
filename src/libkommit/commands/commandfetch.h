/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"
#include "libkommit_export.h"
namespace Git
{

class LIBKOMMIT_EXPORT CommandFetch : public AbstractCommand
{
public:
    CommandFetch();

    [[nodiscard]] QStringList generateArgs() const override;

    [[nodiscard]] bool noFf() const;
    void setNoFf(bool newNoFf);

    [[nodiscard]] bool ffOnly() const;
    void setFfOnly(bool newFfOnly);

    [[nodiscard]] bool noCommit() const;
    void setNoCommit(bool newNoCommit);

    [[nodiscard]] bool prune() const;
    void setPrune(bool newPrune);

    [[nodiscard]] bool tags() const;
    void setTags(bool newTags);

    void parseOutputSection(const QByteArray &output, const QByteArray &errorOutput) override;

    [[nodiscard]] const QString &remote() const;
    void setRemote(const QString &newRemote);

    [[nodiscard]] const QString &branch() const;
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
