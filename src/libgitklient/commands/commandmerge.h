/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"
#include "libgitklient_export.h"

namespace Git
{

class LIBGITKLIENT_EXPORT CommandMerge : public AbstractCommand
{
    Q_OBJECT

public:
    enum Strategy {
        Default,
        Ort,
        Recursive,
        Resolve,
        Octopus,
        Ours,
        Subtree,
    };
    Q_ENUM(Strategy)
    enum DiffAlgorithm { Patience, Minimal, Histogram, Myers };
    Q_ENUM(DiffAlgorithm)

    explicit CommandMerge(Manager *git);
    Q_REQUIRED_RESULT QStringList generateArgs() const override;

    Q_REQUIRED_RESULT OptionalBool commit() const;
    void setCommit(OptionalBool newCommit);

    Q_REQUIRED_RESULT OptionalBool allowUnrelatedHistories() const;
    void setAllowUnrelatedHistories(OptionalBool newAllowUnrelatedHistories);

    FastForwardType ff() const;
    void setFf(FastForwardType newFf);

    Q_REQUIRED_RESULT OptionalBool squash() const;
    void setSquash(OptionalBool newSquash);

    Q_REQUIRED_RESULT const QString &fromBranch() const;
    void setFromBranch(const QString &newFromBranch);

    Q_REQUIRED_RESULT const Strategy &strategy() const;
    void setStrategy(Git::CommandMerge::Strategy newStrategy);

    Q_REQUIRED_RESULT bool ignoreSpaceChange() const;
    void setIgnoreSpaceChange(bool newIgnoreSpaceChange);
    Q_REQUIRED_RESULT bool ignoreAllSpace() const;
    void setIgnoreAllSpace(bool newIgnoreAllSpace);
    Q_REQUIRED_RESULT bool ignoreSpaceAtEol() const;
    void setIgnoreSpaceAtEol(bool newIgnoreSpaceAtEol);
    Q_REQUIRED_RESULT bool ignoreCrAtEol() const;
    void setIgnoreCrAtEol(bool newIgnoreCrAtEol);
    Q_REQUIRED_RESULT bool renormalize() const;
    void setRenormalize(bool newRenormalize);
    Q_REQUIRED_RESULT bool noRenames() const;
    void setNoRenames(bool newNoRenames);
    Q_REQUIRED_RESULT DiffAlgorithm diffAlgorithm() const;
    void setDiffAlgorithm(DiffAlgorithm newDiffAlgorithm);

    Q_REQUIRED_RESULT bool ours() const;
    void setOurs(bool newOurs);

    Q_REQUIRED_RESULT bool theirs() const;
    void setTheirs(bool newTheirs);

private:
    QString mFromBranch;

    OptionalBool mCommit{OptionalBool::Unset};
    OptionalBool mAllowUnrelatedHistories{OptionalBool::Unset};
    OptionalBool mSquash{false};

    FastForwardType mFf{FastForwardType::Unset};
    Strategy mStrategy{Default};
    DiffAlgorithm mDiffAlgorithm{Histogram};

    bool mIgnoreSpaceChange{false};
    bool mIgnoreAllSpace{false};
    bool mIgnoreSpaceAtEol{false};
    bool mIgnoreCrAtEol{false};
    bool mRenormalize{false};
    bool mNoRenames{false};
    bool mOurs{false};
    bool mTheirs{false};
};

}
