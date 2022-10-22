/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "openfilesdialog.h"

#include <QFileDialog>

OpenFilesDialog::OpenFilesDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    connect(toolButtonSelectLocalFile, &QToolButton::clicked, this, &OpenFilesDialog::slotToolButtonSelectLocalFileClicked);
    connect(toolButtonSelectRemoteFile, &QToolButton::clicked, this, &OpenFilesDialog::slotToolButtonSelectRemoteFileClicked);
    connect(toolButtonSelectBaseFile, &QToolButton::clicked, this, &OpenFilesDialog::slotToolButtonSelectBaseFileClicked);
}

QString OpenFilesDialog::filePathLocal() const
{
    return lineEditLocalFile->text();
}

void OpenFilesDialog::setFilePathLocal(const QString &newFilePathLocal)
{
    lineEditLocalFile->setText(newFilePathLocal);
}

QString OpenFilesDialog::filePathRemote() const
{
    return lineEditRemoteFile->text();
}

void OpenFilesDialog::setFilePathRemote(const QString &newFilePathRemote)
{
    lineEditRemoteFile->setText(newFilePathRemote);
}

QString OpenFilesDialog::filePathBase() const
{
    return lineEditBaseFile->text();
}

void OpenFilesDialog::setFilePathBase(const QString &newFilePathBase)
{
    lineEditBaseFile->setText(newFilePathBase);
}

void OpenFilesDialog::slotToolButtonSelectLocalFileClicked()
{
    auto fileName = QFileDialog::getOpenFileName(this, i18n("Select local file"));
    if (!fileName.isEmpty())
        lineEditLocalFile->setText(fileName);
}

void OpenFilesDialog::slotToolButtonSelectRemoteFileClicked()
{
    auto fileName = QFileDialog::getOpenFileName(this, i18n("Select remote file"));
    if (!fileName.isEmpty())
        lineEditRemoteFile->setText(fileName);
}

void OpenFilesDialog::slotToolButtonSelectBaseFileClicked()
{
    auto fileName = QFileDialog::getOpenFileName(this, i18n("Select base file"));
    if (!fileName.isEmpty())
        lineEditBaseFile->setText(fileName);
}
