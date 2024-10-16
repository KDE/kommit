/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_diffwidget.h"

#include <entities/blob.h>
#include <entities/file.h>

#include "libkommitwidgets_export.h"

#include <QScopedPointer>
#include <QTextOption>
#include <QWidget>

class DiffWidgetPrivate;
class CodeEditor;
class LIBKOMMITWIDGETS_EXPORT DiffWidget : public QWidget, private Ui::DiffWIdget
{
    Q_OBJECT

    Q_PROPERTY(bool sameSize READ sameSize WRITE setSameSize NOTIFY sameSizeChanged)

public:
    explicit DiffWidget(QWidget *parent = nullptr);
    // DiffWidget(QSharedPointer<Git::Blob> oldFile, QSharedPointer<Git::Blob> newFile, QWidget *parent = nullptr);
    ~DiffWidget();

    void setOldFile(QSharedPointer<Git::Blob> newOldFile);
    void setNewFile(QSharedPointer<Git::Blob> newNewFile);

    void setOldFile(QSharedPointer<Git::File> newOldFile);
    void setNewFile(QSharedPointer<Git::File> newNewFile);

    void setOldFile(const QString &filePath);
    void setNewFile(const QString &filePath);

    void setOldFile(const QString &title, const QString &content);
    void setNewFile(const QString &title, const QString &content);

    void compare();

    CodeEditor *oldCodeEditor() const;
    CodeEditor *newCodeEditor() const;

    [[nodiscard]] bool sameSize() const;
    void setSameSize(bool newSameSize);

    void scrollToTop();

    void setOldFileText(const QString &newOldFile);
    void setNewFileText(const QString &newNewFile);

public Q_SLOTS:
    void showHiddenChars(bool show);
    void showFilesInfo(bool show);
    void showSameSize(bool show);

Q_SIGNALS:
    void sameSizeChanged();

private Q_SLOTS:
    LIBKOMMITWIDGETS_NO_EXPORT void slotSegmentsScrollbarHover(int y, double pos);
    LIBKOMMITWIDGETS_NO_EXPORT void slotSegmentsScrollbarMouseMove(int y, double pos);
    LIBKOMMITWIDGETS_NO_EXPORT void slotSplitterSplitterMoved(int, int);
    LIBKOMMITWIDGETS_NO_EXPORT void oldCodeEditor_scroll(int value);
    LIBKOMMITWIDGETS_NO_EXPORT void newCodeEditor_scroll(int value);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    QScopedPointer<DiffWidgetPrivate> d_ptr;
    Q_DECLARE_PRIVATE(DiffWidget)
};
