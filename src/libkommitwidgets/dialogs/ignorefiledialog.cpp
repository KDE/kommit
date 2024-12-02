/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "ignorefiledialog.h"

#include "repository.h"

#include "core/kmessageboxhelper.h"
#include "libkommitwidgets_appdebug.h"
#include <QFileInfo>
#include <QPushButton>

#include <KLocalizedString>

IgnoreFileDialog::IgnoreFileDialog(Git::Repository *git, const QString &filePath, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    QFileInfo fi{filePath};

    if (fi.isDir()) {
        groupBoxFileName->hide();
        mPath = filePath;

        mPath = mPath.remove(git->path());

        if (mPath.endsWith(QLatin1Char('/')))
            mPath.removeLast();
    } else {
        mPath = fi.absolutePath();
        if (!mPath.endsWith(QLatin1Char('/')))
            mPath.append(QLatin1Char('/'));

        mPath = mPath.remove(git->path());
        mName = fi.baseName();
        mExt = fi.completeSuffix();
    }

    if (mPath == QLatin1Char('/'))
        radioButtonDirIgnoreFile->setEnabled(false);

    generateIgnorePattern();

    const auto isIgnored = git->isIgnored(mPath);

    if (isIgnored) {
        groupBoxFileName->setEnabled(false);
        groupBoxIgnoreFile->setEnabled(false);
        groupBoxPath->setEnabled(false);
        KMessageBoxHelper::error(this, i18n("The file is ignored already"));
        mIsIgnoredAlready = true;
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }

    connect(buttonBox, &QDialogButtonBox::accepted, this, &IgnoreFileDialog::slotAccept);

    connect(radioButtonName, &QRadioButton::toggled, this, &IgnoreFileDialog::generateIgnorePattern);
    connect(radioButtonExt, &QRadioButton::toggled, this, &IgnoreFileDialog::generateIgnorePattern);
    connect(radioButtonNameAndExt, &QRadioButton::toggled, this, &IgnoreFileDialog::generateIgnorePattern);
    connect(radioButtonExactPath, &QRadioButton::toggled, this, &IgnoreFileDialog::generateIgnorePattern);
    connect(radioButtonEveryWhere, &QRadioButton::toggled, this, &IgnoreFileDialog::generateIgnorePattern);
}

void IgnoreFileDialog::generateIgnorePattern()
{
    QString s;
    if (radioButtonName->isChecked())
        s = mName + QStringLiteral(".*");
    else if (radioButtonExt->isChecked())
        s = QStringLiteral("*.") + mExt;
    else {
        if (mExt.isEmpty())
            s = mName;
        else
            s = mName + QLatin1Char('.') + mExt;
    }

    if (radioButtonExactPath->isChecked())
        s = mPath + s;

    lineEdit->setText(s);
    qCDebug(KOMMIT_WIDGETS_LOG) << getIgnoreFile();
}

void IgnoreFileDialog::slotAccept()
{
    if (mIsIgnoredAlready) {
        accept();
        return;
    }

    if (lineEdit->text().trimmed().isEmpty()) {
        KMessageBoxHelper::error(this, i18n("Please enter the pattern"));
        return;
    }

    QFile f(getIgnoreFile());
    if (!f.open(QIODevice::Append | QIODevice::Text)) {
        KMessageBoxHelper::error(this, i18n("Unable to open file: %1", getIgnoreFile()));
        return;
    }

    f.write("\n" + lineEdit->text().toUtf8());
    f.close();

    accept();
}

QString IgnoreFileDialog::getIgnoreFile() const
{
    if (radioButtonRootIgnoreFile->isChecked())
        return mGit->path() + QStringLiteral(".gitignore");
    else if (radioButtonDirIgnoreFile->isChecked())
        return mGit->path() + mPath + QStringLiteral("/.gitignore");
    else
        return mGit->path() + QStringLiteral(".git/info/exclude");
}

#include "moc_ignorefiledialog.cpp"
