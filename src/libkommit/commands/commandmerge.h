/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"
#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT CommandMerge : public AbstractCommand
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
    enum DiffAlgorithm {
        Patience,
        Minimal,
        Histogram,
        Myers
    };
    Q_ENUM(DiffAlgorithm)

    explicit CommandMerge(Repository *git);
    [[nodiscard]] QStringList generateArgs() const override;

    [[nodiscard]] OptionalBool commit() const;
    void setCommit(OptionalBool newCommit);

    [[nodiscard]] OptionalBool allowUnrelatedHistories() const;
    void setAllowUnrelatedHistories(OptionalBool newAllowUnrelatedHistories);

    [[nodiscard]] FastForwardType ff() const;
    void setFf(FastForwardType newFf);

    [[nodiscard]] OptionalBool squash() const;
    void setSquash(OptionalBool newSquash);

    [[nodiscard]] const QString &fromBranch() const;
    void setFromBranch(const QString &newFromBranch);

    [[nodiscard]] const Strategy &strategy() const;
    void setStrategy(Git::CommandMerge::Strategy newStrategy);

    [[nodiscard]] bool ignoreSpaceChange() const;
    void setIgnoreSpaceChange(bool newIgnoreSpaceChange);
    [[nodiscard]] bool ignoreAllSpace() const;
    void setIgnoreAllSpace(bool newIgnoreAllSpace);
    [[nodiscard]] bool ignoreSpaceAtEol() const;
    void setIgnoreSpaceAtEol(bool newIgnoreSpaceAtEol);
    [[nodiscard]] bool ignoreCrAtEol() const;
    void setIgnoreCrAtEol(bool newIgnoreCrAtEol);
    [[nodiscard]] bool renormalize() const;
    void setRenormalize(bool newRenormalize);
    [[nodiscard]] bool noRenames() const;
    void setNoRenames(bool newNoRenames);
    [[nodiscard]] DiffAlgorithm diffAlgorithm() const;
    void setDiffAlgorithm(DiffAlgorithm newDiffAlgorithm);

    [[nodiscard]] bool ours() const;
    void setOurs(bool newOurs);

    [[nodiscard]] bool theirs() const;
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
