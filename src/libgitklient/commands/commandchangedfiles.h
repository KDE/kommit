/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"
#include "filestatus.h"

#include <QList>

namespace Git
{

class CommandChangedFiles : public AbstractCommand
{
public:
    explicit CommandChangedFiles(Manager *git);

    const QList<FileStatus> &files() const;
    bool ignored() const;
    void setIgnored(bool newIgnored);
    bool untracked() const;
    void setUntracked(bool newUntracked);
    bool ignoreSubmodules() const;
    void setIgnoreSubmodules(bool newIgnoreSubmodules);

    QStringList generateArgs() const override;
    void parseOutput(const QByteArray &output, const QByteArray &errorOutput) override;

private:
    QList<FileStatus> mFiles;
    bool mIgnored{false};
    bool mUntracked{false};
    bool mIgnoreSubmodules{true};
};

} // namespace Git
