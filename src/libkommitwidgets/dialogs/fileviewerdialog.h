/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"
#include "ui_fileviewerdialog.h"

#include <QPointer>

#include <KParts/MainWindow>
#include <KParts/ReadOnlyPart>

class QMimeType;
namespace Git
{
class Blob;
}
class LIBKOMMITWIDGETS_EXPORT FileViewerDialog : public KParts::MainWindow, private Ui::FileViewerDialog
{
    Q_OBJECT

public:
    explicit FileViewerDialog(QSharedPointer<Git::Blob> file, QWidget *parent = nullptr);

    ~FileViewerDialog() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QString mFilePath;

    LIBKOMMITWIDGETS_NO_EXPORT void showFile(const Git::Blob &file);
    LIBKOMMITWIDGETS_NO_EXPORT void showInEditor(const Git::Blob &file);
    LIBKOMMITWIDGETS_NO_EXPORT void showAsImage(const Git::Blob &file);
    LIBKOMMITWIDGETS_NO_EXPORT bool showWithParts(const QMimeType &mime, const Git::Blob &file);
    QPointer<KParts::ReadOnlyPart> mPart;
};
