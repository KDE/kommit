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
#include <KService>

class QMimeType;
namespace Git
{
class File;
}
class LIBKOMMITWIDGETS_EXPORT FileViewerDialog : public KParts::MainWindow, private Ui::FileViewerDialog
{
    Q_OBJECT

public:
    explicit FileViewerDialog(QSharedPointer<Git::File> file, QWidget *parent = nullptr);

    ~FileViewerDialog() override;

private:
    QString mFilePath;

    LIBKOMMITWIDGETS_NO_EXPORT void showFile(const Git::File &file);
    LIBKOMMITWIDGETS_NO_EXPORT void showInEditor(const Git::File &file);
    LIBKOMMITWIDGETS_NO_EXPORT void showAsImage(const Git::File &file);
    LIBKOMMITWIDGETS_NO_EXPORT bool showWithParts(const QMimeType &mime, const Git::File &file);
    QPointer<KParts::ReadOnlyPart> m_part;

protected:
    void keyPressEvent(QKeyEvent *event) override;
};
