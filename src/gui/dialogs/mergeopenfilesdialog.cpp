/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mergeopenfilesdialog.h"

#include <KLocalizedString>
#include <QFileDialog>

MergeOpenFilesDialog::MergeOpenFilesDialog(QWidget *parent)
    : AppDialog(parent)
{
    setupUi(this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &MergeOpenFilesDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MergeOpenFilesDialog::reject);

    connect(toolButtonSelectLocalFile, &QToolButton::clicked, this, &MergeOpenFilesDialog::slotToolButtonSelectLocalFileClicked);
    connect(toolButtonSelectRemoteFile, &QToolButton::clicked, this, &MergeOpenFilesDialog::slotToolButtonSelectRemoteFileClicked);
    connect(toolButtonSelectBaseFile, &QToolButton::clicked, this, &MergeOpenFilesDialog::slotToolButtonSelectBaseFileClicked);
}

QString MergeOpenFilesDialog::filePathLocal() const
{
    return lineEditLocalFile->text();
}

void MergeOpenFilesDialog::setFilePathLocal(const QString &newFilePathLocal)
{
    lineEditLocalFile->setText(newFilePathLocal);
}

QString MergeOpenFilesDialog::filePathRemote() const
{
    return lineEditRemoteFile->text();
}

void MergeOpenFilesDialog::setFilePathRemote(const QString &newFilePathRemote)
{
    lineEditRemoteFile->setText(newFilePathRemote);
}

QString MergeOpenFilesDialog::filePathBase() const
{
    return lineEditBaseFile->text();
}

void MergeOpenFilesDialog::setFilePathBase(const QString &newFilePathBase)
{
    lineEditBaseFile->setText(newFilePathBase);
}

void MergeOpenFilesDialog::slotToolButtonSelectLocalFileClicked()
{
    const auto fileName = QFileDialog::getOpenFileName(this, i18n("Select local file"));
    if (!fileName.isEmpty())
        lineEditLocalFile->setText(fileName);
}

void MergeOpenFilesDialog::slotToolButtonSelectRemoteFileClicked()
{
    const auto fileName = QFileDialog::getOpenFileName(this, i18n("Select remote file"));
    if (!fileName.isEmpty())
        lineEditRemoteFile->setText(fileName);
}

void MergeOpenFilesDialog::slotToolButtonSelectBaseFileClicked()
{
    const auto fileName = QFileDialog::getOpenFileName(this, i18n("Select base file"));
    if (!fileName.isEmpty())
        lineEditBaseFile->setText(fileName);
}
