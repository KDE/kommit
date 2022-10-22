/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_openfilesdialog.h"

class OpenFilesDialog : public QDialog, private Ui::OpenFilesDialog
{
    Q_OBJECT

public:
    explicit OpenFilesDialog(QWidget *parent = nullptr);

    QString filePathLocal() const;
    void setFilePathLocal(const QString &newFilePathLocal);

    QString filePathRemote() const;
    void setFilePathRemote(const QString &newFilePathRemote);

    QString filePathBase() const;
    void setFilePathBase(const QString &newFilePathBase);

private slots:
    void slotToolButtonSelectLocalFileClicked();
    void slotToolButtonSelectRemoteFileClicked();
    void slotToolButtonSelectBaseFileClicked();
};
