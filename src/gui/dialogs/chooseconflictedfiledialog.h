/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_chooseconflictedfiledialog.h"

#include <QSharedPointer>

namespace Git {
class Blob;
}

class ChooseConflictedFileDialog : public QDialog, private Ui::ChooseConflictedFileDialog
{
    Q_OBJECT

public:
    enum class Side {
        None,
        Base,
        Local,
        Remote
    };

    ChooseConflictedFileDialog(QSharedPointer<Git::Blob> baseFile,
                               QSharedPointer<Git::Blob> localFile,
                               QSharedPointer<Git::Blob> remoteFile,
                               QWidget *parent = nullptr);

    [[nodiscard]] Side selectedSide() const;

private:
    void slotLabelLinkActivated(const QString& link);
    void slotLabelDiffBaseWithLocalLinkActivated(const QString &link);
    void slotLabelDiffBaseWithRemoteLinkActivated(const QString &link);
    void slotLabelDiffLocalWithRemoteLinkActivated(const QString &link);

    void slotRadioButtonBaseFileClicked();
    void slotRadioButtonLocalFileClicked();
    void slotRadioButtonRemoteFileClicked();

    Side _selectedSide;

    QSharedPointer<Git::Blob> mBaseFile;
    QSharedPointer<Git::Blob> mLocalFile;
    QSharedPointer<Git::Blob> mRemoteFile;
};
