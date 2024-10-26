/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"
#include "ui_mergewidget.h"

#include <QScopedPointer>

class MergeWidgetPrivate;
class QAction;
class LIBKOMMITWIDGETS_EXPORT MergeWidget : public QWidget, private Ui::MergeWidget
{
    Q_OBJECT

    Q_PROPERTY(bool isModified READ isModified WRITE setIsModified NOTIFY isModifiedChanged FINAL)
    Q_PROPERTY(int conflicts READ conflicts NOTIFY conflictsChanged FINAL)

public:
    explicit MergeWidget(QWidget *parent = nullptr);
    virtual ~MergeWidget();

    [[nodiscard]] bool ShowPreview() const;
    void setShowPreview(bool newShowPreview);

    void setBaseFile(const QString &title, const QString &content);
    void setLocalFile(const QString &title, const QString &content);
    void setRemoteFile(const QString &title, const QString &content);
    void setResultFile(const QString &title);
    void compare();

    [[nodiscard]] QString result() const;

    [[nodiscard]] bool sameSize() const;
    void setSameSize(bool sameSize);

    QAction *keepMineAction();
    QAction *keepTheirAction();
    QAction *keepMineBeforeTheirAction();
    QAction *keepTheirBeforeMineAction();
    QAction *keepMyFileAction();
    QAction *keepTheirFileAction();
    QAction *gotoPrevDiffAction();
    QAction *gotoNextDiffAction();

    [[nodiscard]] bool isModified() const;
    void setIsModified(bool isModified);

    [[nodiscard]] int conflicts() const;

signals:
    void isModifiedChanged(bool modified);
    void conflictsChanged(int conflicts);

private:
    void slotKeepMineActionTriggered();
    void slotKeepTheirActionTriggered();
    void slotKeepMineBeforeTheirActionTriggered();
    void slotKeepTheirBeforeMineActionTriggered();
    void slotKeepMyFileActionTriggered();
    void slotKeepTheirFileActionTriggered();
    void slotGotoPrevDiffActionTriggered();
    void slotGotoNextDiffActionTriggered();

    void slotCodeEditorScroll();
    void slotCodeBlockSelected();

    QScopedPointer<MergeWidgetPrivate> d_ptr;
    Q_DISABLE_COPY(MergeWidget)
    Q_DECLARE_PRIVATE(MergeWidget)
};
