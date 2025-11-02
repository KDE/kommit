/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "chooseconflictedfiledialog.h"
#include "dialogs/fileviewerdialog.h"
#include <windows/diffwindow.h>

#include <Kommit/Blob>

#include <KLocalizedString>

#include <QDialogButtonBox>
#include <QPushButton>

namespace
{
void setLabelText(QLabel *label, const Git::Blob &file)
{
    if (file.isNull()) {
        label->setText(i18n("Removed"));
        auto p = label->palette();
        p.setColor(QPalette::WindowText, Qt::red);
        label->setPalette(p);
    } else {
        label->setText(QStringLiteral("<a href='#'>%1</a>").arg(file.fileName()));
    }
}

}

ChooseConflictedFileDialog::ChooseConflictedFileDialog(const Git::Blob &baseFile, const Git::Blob &localFile, const Git::Blob &remoteFile, QWidget *parent)
    : QDialog(parent)
    , mBaseFile(std::move(baseFile))
    , mLocalFile(std::move(localFile))
    , mRemoteFile(std::move(remoteFile))
{
    setupUi(this);

    setLabelText(labelBaseFile, mBaseFile);
    setLabelText(labelLocalFile, mLocalFile);
    setLabelText(labelRemoteFile, mRemoteFile);

    connect(labelBaseFile, &QLabel::linkActivated, this, &ChooseConflictedFileDialog::slotLabelLinkActivated);
    connect(labelLocalFile, &QLabel::linkActivated, this, &ChooseConflictedFileDialog::slotLabelLinkActivated);
    connect(labelRemoteFile, &QLabel::linkActivated, this, &ChooseConflictedFileDialog::slotLabelLinkActivated);

    connect(radioButtonBaseFile, &QRadioButton::clicked, this, &ChooseConflictedFileDialog::slotRadioButtonBaseFileClicked);
    connect(radioButtonLocalFile, &QRadioButton::clicked, this, &ChooseConflictedFileDialog::slotRadioButtonLocalFileClicked);
    connect(radioButtonRemoteFile, &QRadioButton::clicked, this, &ChooseConflictedFileDialog::slotRadioButtonRemoteFileClicked);

    connect(labelDiffBaseWithLocal, &QLabel::linkActivated, this, &ChooseConflictedFileDialog::slotLabelDiffBaseWithLocalLinkActivated);
    connect(labelDiffBaseWithRemote, &QLabel::linkActivated, this, &ChooseConflictedFileDialog::slotLabelDiffBaseWithRemoteLinkActivated);
    connect(labelDiffLocalWithRemote, &QLabel::linkActivated, this, &ChooseConflictedFileDialog::slotLabelDiffLocalWithRemoteLinkActivated);

    labelDiffBaseWithLocal->setVisible(!mBaseFile.isNull() && !mLocalFile.isNull());
    labelDiffBaseWithRemote->setVisible(!mBaseFile.isNull() && !mRemoteFile.isNull());
    labelDiffLocalWithRemote->setVisible(!mLocalFile.isNull() && !mRemoteFile.isNull());

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void ChooseConflictedFileDialog::slotLabelLinkActivated(const QString &link)
{
    Q_UNUSED(link)

    auto lbl = qobject_cast<QLabel *>(sender());

    Git::Blob file;

    if (lbl == labelBaseFile)
        file = mBaseFile;
    else if (lbl == labelLocalFile)
        file = mLocalFile;
    else if (lbl == labelRemoteFile)
        file = mRemoteFile;
    else
        return;

    auto d = new FileViewerDialog(file, this);
    d->setWindowModality(Qt::ApplicationModal);
    d->setAttribute(Qt::WA_DeleteOnClose, true);
    d->show();
}

void ChooseConflictedFileDialog::slotLabelDiffBaseWithLocalLinkActivated(const QString &link)
{
    Q_UNUSED(link)
    auto d = new DiffWindow{mBaseFile, mLocalFile};
    d->setWindowModality(Qt::ApplicationModal);
    d->setAttribute(Qt::WA_DeleteOnClose, true);
    d->exec();
}

void ChooseConflictedFileDialog::slotLabelDiffBaseWithRemoteLinkActivated(const QString &link)
{
    Q_UNUSED(link)
    auto d = new DiffWindow{mBaseFile, mRemoteFile};
    d->setWindowModality(Qt::ApplicationModal);
    d->setAttribute(Qt::WA_DeleteOnClose, true);
    d->exec();
}

void ChooseConflictedFileDialog::slotLabelDiffLocalWithRemoteLinkActivated(const QString &link)
{
    Q_UNUSED(link)
    auto d = new DiffWindow{mLocalFile, mRemoteFile};
    d->setWindowModality(Qt::ApplicationModal);
    d->setAttribute(Qt::WA_DeleteOnClose, true);
    d->exec();
}

void ChooseConflictedFileDialog::slotRadioButtonBaseFileClicked()
{
    _selectedSide = Side::Base;
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void ChooseConflictedFileDialog::slotRadioButtonLocalFileClicked()
{
    _selectedSide = Side::Local;
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void ChooseConflictedFileDialog::slotRadioButtonRemoteFileClicked()
{
    _selectedSide = Side::Remote;
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

ChooseConflictedFileDialog::Side ChooseConflictedFileDialog::selectedSide() const
{
    return _selectedSide;
}

#include "moc_chooseconflictedfiledialog.cpp"
