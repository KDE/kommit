/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diffopendialog.h"

#include <KLocalizedString>
#include <QFileDialog>
#include <QSettings>

DiffOpenDialog::DiffOpenDialog(QWidget *parent)
    : AppDialog(parent)
{
    setupUi(this);
    radioButtonCompareFiles->setChecked(true);
    widgetSelectDirectories->hide();
    readSettings();
    // TODO update ok button enabled

    connect(toolButtonBrowseNewFile, &QToolButton::clicked, this, &DiffOpenDialog::slotToolButtonBrowseNewFileClicked);
    connect(toolButtonBrowseOldFile, &QToolButton::clicked, this, &DiffOpenDialog::slotToolButtonBrowseOldFileClicked);
    connect(toolButtonBrowseNewDirectory, &QToolButton::clicked, this, &DiffOpenDialog::slotToolButtonBrowseNewDirectoryClicked);
    connect(toolButtonBrowseOldDirectory, &QToolButton::clicked, this, &DiffOpenDialog::slotToolButtonBrowseOldDirectoryClicked);
}

DiffOpenDialog::~DiffOpenDialog()
{
    readSettings();
}

void DiffOpenDialog::readSettings()
{
    QSettings s;
    s.beginGroup(QStringLiteral("diff"));
    s.setValue(QStringLiteral("oldFile"), lineEditOldFile->text());
    s.setValue(QStringLiteral("newFile"), lineEditNewFile->text());
    s.setValue(QStringLiteral("oldDir"), lineEditOldDirectory->text());
    s.setValue(QStringLiteral("newDir"), lineEditNewDirectory->text());
    s.endGroup();
    s.sync();
}

void DiffOpenDialog::saveSettings()
{
    QSettings s;
    s.beginGroup(QStringLiteral("diff"));
    lineEditOldFile->setText(s.value(QStringLiteral("oldFile")).toString());
    lineEditNewFile->setText(s.value(QStringLiteral("newFile")).toString());
    lineEditOldDirectory->setText(s.value(QStringLiteral("oldDir")).toString());
    lineEditNewDirectory->setText(s.value(QStringLiteral("newDir")).toString());
    s.endGroup();
}

QString DiffOpenDialog::oldFile() const
{
    return lineEditOldFile->text();
}

QString DiffOpenDialog::newFile() const
{
    return lineEditNewFile->text();
}

QString DiffOpenDialog::oldDir() const
{
    return lineEditOldDirectory->text();
}

QString DiffOpenDialog::newDir() const
{
    return lineEditNewDirectory->text();
}

DiffOpenDialog::Mode DiffOpenDialog::mode() const
{
    return radioButtonCompareFiles->isChecked() ? Files : Dirs;
}

void DiffOpenDialog::slotToolButtonBrowseNewFileClicked()
{
    const auto f = QFileDialog::getOpenFileName(this, i18n("Select new file"));
    if (!f.isEmpty())
        lineEditNewFile->setText(f);
}

void DiffOpenDialog::slotToolButtonBrowseOldFileClicked()
{
    const auto f = QFileDialog::getOpenFileName(this, i18n("Select old file"));
    if (!f.isEmpty())
        lineEditOldFile->setText(f);
}

void DiffOpenDialog::slotToolButtonBrowseNewDirectoryClicked()
{
    const auto f = QFileDialog::getExistingDirectory(this, i18n("Select new directory"));
    if (!f.isEmpty())
        lineEditNewDirectory->setText(f);
}

void DiffOpenDialog::slotToolButtonBrowseOldDirectoryClicked()
{
    const auto f = QFileDialog::getExistingDirectory(this, i18n("Select old directory"));
    if (!f.isEmpty())
        lineEditOldDirectory->setText(f);
}
