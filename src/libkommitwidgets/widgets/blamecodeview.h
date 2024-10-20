/*
SPDX-FileCopyrightText: 2020-2022 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "codeeditor.h"

#include "libkommitwidgets_export.h"

#include <Kommit/Blame>

class LIBKOMMITWIDGETS_EXPORT BlameCodeView : public CodeEditor
{
    Q_OBJECT

public:
    explicit BlameCodeView(QWidget *parent = nullptr);

    void setBlameData(const Git::Blame &newBlameData);

private:
    Git::Blame mBlameData;
};
