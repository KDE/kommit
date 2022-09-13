/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "ignorefiledialog.h"

#include "git/gitmanager.h"

#include <QDebug>
#include <QFileInfo>
#include <QPushButton>

#include <klocalizedstring.h>
#include <kmessagebox.h>

IgnoreFileDialog::IgnoreFileDialog(Git::Manager *git, const QString &filePath, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    QFileInfo fi{filePath};

    if (fi.isDir())
        groupBoxFileName->hide();

    mPath = fi.absolutePath() + QLatin1Char('/');
    mPath = mPath.remove(git->path());
    mName = fi.baseName();
    mExt = fi.completeSuffix();

    if (mPath == "/")
        radioButtonDirIgnoreFile->setEnabled(false);

    generateIgnorePattern();

    auto isIgnored = git->isIgnored(mName + QLatin1Char('.') + mExt);

    if (isIgnored) {
        groupBoxFileName->setEnabled(false);
        groupBoxIgnoreFile->setEnabled(false);
        groupBoxPath->setEnabled(false);
        KMessageBox::error(this, i18n("The file is ignored already"));
        mIsIgnoredAlready = true;
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

void IgnoreFileDialog::generateIgnorePattern()
{
    QString s;
    if (radioButtonName->isChecked())
        s = mName + ".*";
    else if (radioButtonExt->isChecked())
        s = "*." + mExt;
    else {
        if (mExt.isEmpty())
            s = mName;
        else
            s = mName + QLatin1Char('.') + mExt;
    }

    if (radioButtonExactPath->isChecked())
        s = mPath + s;

    lineEdit->setText(s);
    qDebug() << getIgnoreFile();
}

void IgnoreFileDialog::on_buttonBox_accepted()
{
    if (mIsIgnoredAlready) {
        accept();
        close();
        return;
    }

    if (lineEdit->text().isEmpty()) {
        KMessageBox::error(this, i18n("Please enter the pattern"));
        return;
    }
    QFile f(getIgnoreFile());
    if (!f.open(QIODevice::Append | QIODevice::Text)) {
        KMessageBox::error(this, i18n("Unable to open file: %1", getIgnoreFile()));
        return;
    }

    f.write("\n" + lineEdit->text().toUtf8());
    f.close();

    accept();
    close();
}

QString IgnoreFileDialog::getIgnoreFile() const
{
    if (radioButtonRootIgnoreFile->isChecked())
        return mGit->path() + QStringLiteral("/.gitignore");
    else if (radioButtonDirIgnoreFile->isChecked())
        return mGit->path() + mPath + QStringLiteral(".gitignore");
    else
        return mGit->path() + QStringLiteral("/.git/info/exclude");
}
