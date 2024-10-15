/*
SPDX-FileCopyrightText: 2023 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_certificateinfodialog.h"

namespace Git {
class Certificate;
}

class CertificateInfoDialog : public QDialog, private Ui::CertificateInfoDialog
{
    Q_OBJECT

public:
    explicit CertificateInfoDialog(Git::Certificate *cert, QWidget *parent = nullptr);
};
