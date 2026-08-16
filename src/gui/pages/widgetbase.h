/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QWidget>

#include <QSettings>

class QSplitter;
class QTreeView;
class AppWindow;
class RepositoryData;

class WidgetBase : public QWidget
{
    Q_OBJECT

protected:
    RepositoryData *mGit{nullptr};
    AppWindow *mParent{nullptr};
    QString stateName(QWidget *w) const;

    /**
     * Asks for a reload the next time this page is shown.
     *
     * Only one page is on screen at a time, so a page that is not visible has no reason to
     * read the repository yet; deferring it is what keeps opening a large repository from
     * doing the same expensive work once per page.
     */
    void markNeedsReload();
    void showEvent(QShowEvent *event) override;

public:
    explicit WidgetBase(RepositoryData *git, AppWindow *parent = nullptr);
    RepositoryData *git() const;
    void setGit(RepositoryData *newGit);

    virtual void reload();
    virtual void clear();

    int exec(QWidget *parent = nullptr);

    virtual void saveState(QSettings &settings) const;
    virtual void restoreState(QSettings &settings);

    void save(QSettings &settings, QSplitter *splitter) const;
    void restore(QSettings &settings, QSplitter *splitter);

    void save(QSettings &settings, QTreeView *treeView) const;
    void restore(QSettings &settings, QTreeView *treeView);

    virtual void settingsUpdated();

private:
    void gitPathChanged();

    bool mNeedsReload{false};
};
