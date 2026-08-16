/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

//
// Created by hamed on 25.03.22.
//

#pragma once
#include "libkommitwidgets_export.h"
#include <QAbstractListModel>

namespace Git
{
class Repository;
}

class LIBKOMMITWIDGETS_EXPORT AbstractGitItemsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)

public:
    explicit AbstractGitItemsModel(Git::Repository *git);
    explicit AbstractGitItemsModel(Git::Repository *git, QObject *parent);

    enum Status {
        NotLoaded,
        Loading,
        Loaded
    };
    Q_ENUM(Status)
    [[nodiscard]] bool isLoaded() const;
    [[nodiscard]] Status status() const;
    virtual void clear();

    [[nodiscard]] Git::Repository *manager() const;

    /**
     * When set, opening a repository only marks the model out of date instead of loading
     * it there and then. Whoever shows the model is then responsible for calling
     * loadIfNeeded(). Use it for models whose load is expensive enough to be worth
     * skipping while nothing displays them.
     */
    [[nodiscard]] bool loadOnDemand() const;
    void setLoadOnDemand(bool loadOnDemand);

public Q_SLOTS:
    void load();
    /// Loads the model unless it already holds the current repository's content.
    void loadIfNeeded();

protected:
    void setStatus(Status newStatus);
    Git::Repository *mGit{nullptr};
    virtual void reload() = 0;

Q_SIGNALS:
    void loaded();
    void statusChanged();

private:
    LIBKOMMITWIDGETS_NO_EXPORT void repositoryChanged();

    Status m_status{NotLoaded};
    bool mLoadOnDemand{false};
    bool mStale{true};
};
