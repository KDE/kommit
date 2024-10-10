/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractactions.h"
#include "libkommitwidgets_export.h"

namespace Git
{
class Submodule;
}

class LIBKOMMITWIDGETS_EXPORT SubmoduleActions : public AbstractActions
{
    Q_OBJECT

public:
    explicit SubmoduleActions(Git::Repository *git, QWidget *parent = nullptr);

    [[nodiscard]] QSharedPointer<Git::Submodule> submodule() const;
    void setSubmodule(QSharedPointer<Git::Submodule> newSubmodule);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void init();
    LIBKOMMITWIDGETS_NO_EXPORT void update();
    LIBKOMMITWIDGETS_NO_EXPORT void create();
    LIBKOMMITWIDGETS_NO_EXPORT void deinit();
    LIBKOMMITWIDGETS_NO_EXPORT void sync();
    DEFINE_ACTION(actionInit)
    DEFINE_ACTION(actionUpdate)
    DEFINE_ACTION(actionCreate)
    DEFINE_ACTION(actionDeinit)
    DEFINE_ACTION(actionSync)

    QSharedPointer<Git::Submodule> mSubmodule;
};
