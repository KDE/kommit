/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_submoduleinfodialog.h"

namespace Git
{
class AddSubmoduleCommand;
class Manager;
}
class LIBKOMMITWIDGETS_EXPORT SubmoduleInfoDialog : public AppDialog, private Ui::SubmoduleInfoDialog
{
    Q_OBJECT

public:
    explicit SubmoduleInfoDialog(Git::Manager *git, QWidget *parent = nullptr);

    [[nodiscard]] bool force() const;
    void setForce(bool newForce);

    [[nodiscard]] QString url() const;
    void setUrl(const QString &newUrl);

    [[nodiscard]] QString path() const;
    void setPath(const QString &newPath);

    [[nodiscard]] QString branch() const;
    void setBranch(const QString &newBranch);

    [[nodiscard]] Git::AddSubmoduleCommand *command() const;

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotLineEditPathUrlSelected(const QUrl &url);
};
