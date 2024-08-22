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
    [[nodiscard]] QStringList generateArgs() const override;

    [[nodiscard]] bool squash() const;
    void setSquash(bool newSquash);

    [[nodiscard]] bool noCommit() const;
    void setNoCommit(bool newNoCommit);

    [[nodiscard]] bool prune() const;
    void setPrune(bool newPrune);

    [[nodiscard]] bool tags() const;
    void setTags(bool newTags);

    void parseOutputSection(const QByteArray &output, const QByteArray &errorOutput) override;
    bool supportWidget() const override;
    QWidget *createWidget() override;

    [[nodiscard]] const QString &remote() const;
    void setRemote(const QString &newRemote);

    [[nodiscard]] const QString &branch() const;
    void setBranch(const QString &newBranch);

    [[nodiscard]] Rebase rebase() const;
    void setRebase(Rebase newRebase);

    [[nodiscard]] FastForward fastForward() const;
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
