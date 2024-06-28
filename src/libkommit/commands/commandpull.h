/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractcommand.h"
#include "libkommit_export.h"

#ifdef GIT_GUI
namespace Ui
{
class CommandPullWidget;
}
#endif

namespace Git
{

class LIBKOMMIT_EXPORT CommandPull : public AbstractCommand
{
    Q_OBJECT

public:
    enum Rebase {
        None,
        False,
        True,
        Preserve,
        Merge,
    };
    Q_ENUM(Rebase)

    enum FastForward {
        Unset,
        Yes,
        No,
        OnlyFastForward,
    };
    Q_ENUM(FastForward)

    CommandPull();
    ~CommandPull() override;
    Q_REQUIRED_RESULT QStringList generateArgs() const override;

    Q_REQUIRED_RESULT bool squash() const;
    void setSquash(bool newSquash);

    Q_REQUIRED_RESULT bool noCommit() const;
    void setNoCommit(bool newNoCommit);

    Q_REQUIRED_RESULT bool prune() const;
    void setPrune(bool newPrune);

    Q_REQUIRED_RESULT bool tags() const;
    void setTags(bool newTags);

    void parseOutputSection(const QByteArray &output, const QByteArray &errorOutput) override;
    bool supportWidget() const override;
    QWidget *createWidget() override;

    Q_REQUIRED_RESULT const QString &remote() const;
    void setRemote(const QString &newRemote);

    Q_REQUIRED_RESULT const QString &branch() const;
    void setBranch(const QString &newBranch);

    Q_REQUIRED_RESULT Rebase rebase() const;
    void setRebase(Rebase newRebase);

    Q_REQUIRED_RESULT FastForward fastForward() const;
    void setFastForward(FastForward newFastForward);

private:
    bool mSquash{false};
    bool mNoCommit{false};
    bool mPrune{false};
    bool mTags{false};
    QString mRemote;
    QString mBranch;
    Rebase mRebase{None};
    FastForward mFastForward{Unset};

#ifdef GIT_GUI
    QWidget *mWidget = nullptr;
    Ui::CommandPullWidget *mUi = nullptr;
#endif
};

} // namespace Git
