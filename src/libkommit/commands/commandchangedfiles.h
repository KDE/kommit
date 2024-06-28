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

    [[nodiscard]] const QList<FileStatus> &files() const;
    [[nodiscard]] bool ignored() const;
    void setIgnored(bool newIgnored);
    [[nodiscard]] bool untracked() const;
    void setUntracked(bool newUntracked);
    [[nodiscard]] bool ignoreSubmodules() const;
    void setIgnoreSubmodules(bool newIgnoreSubmodules);

    [[nodiscard]] QStringList generateArgs() const override;
    void parseOutputSection(const QByteArray &output, const QByteArray &errorOutput) override;

private:
    QList<FileStatus> mFiles;
    bool mIgnored{false};
    bool mUntracked{false};
    bool mIgnoreSubmodules{true};
};

} // namespace Git
