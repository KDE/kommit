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

    lineEditNewDirectory->setMode(UrlRequester::Mode::Directory);
    lineEditOldDirectory->setMode(UrlRequester::Mode::Directory);
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
    s.setValue(QStringLiteral("oldDir"), lineEditOldDirectory->url().toLocalFile());
    s.setValue(QStringLiteral("newDir"), lineEditNewDirectory->url().toLocalFile());
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

#include "moc_diffopendialog.cpp"
