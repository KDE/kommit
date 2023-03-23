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

    Q_REQUIRED_RESULT const QList<FileStatus> &files() const;
    Q_REQUIRED_RESULT bool ignored() const;
    void setIgnored(bool newIgnored);
    Q_REQUIRED_RESULT bool untracked() const;
    void setUntracked(bool newUntracked);
    Q_REQUIRED_RESULT bool ignoreSubmodules() const;
    void setIgnoreSubmodules(bool newIgnoreSubmodules);

    Q_REQUIRED_RESULT QStringList generateArgs() const override;
    void parseOutputSection(const QByteArray &output, const QByteArray &errorOutput) override;

private:
    QList<FileStatus> mFiles;
    bool mIgnored{false};
    bool mUntracked{false};
    bool mIgnoreSubmodules{true};
};

} // namespace Git
