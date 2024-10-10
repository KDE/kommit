/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "initdialog.h"
#include <QPushButton>

InitDialog::InitDialog(Git::Repository *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);
    lineEditPath->setMode(UrlRequester::Mode::Directory);
    connect(lineEditPath, &UrlRequester::textChanged, this, [this](const QString &text) {
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.trimmed().isEmpty());
    });
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

QString InitDialog::path() const
{
    return lineEditPath->text();
}

void InitDialog::setPath(const QString &path)
{
    lineEditPath->setText(path);
}

#include "moc_initdialog.cpp"
