/*
SPDX-FileCopyrightText: 2023 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_certificateinfodialog.h"

#include <Kommit/Certificate>

class CertificateInfoDialog : public QDialog, private Ui::CertificateInfoDialog
{
    Q_OBJECT

public:
    explicit CertificateInfoDialog(const Git::Certificate &cert, QWidget *parent = nullptr);
};
