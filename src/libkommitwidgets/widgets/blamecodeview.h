/*
SPDX-FileCopyrightText: 2020-2022 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "blamedata.h"
#include "codeeditor.h"

#include "libkommitwidgets_export.h"

class LIBKOMMITWIDGETS_EXPORT BlameCodeView : public CodeEditor
{
    Q_OBJECT

public:
    explicit BlameCodeView(QWidget *parent = nullptr);

    void setBlameData(QSharedPointer<Git::BlameData> newBlameData);

private:
    QSharedPointer<Git::BlameData> mBlameData;
};
