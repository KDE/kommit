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

    enum Status { NotLoaded, Loading, Loaded };
    Q_ENUM(Status)
    [[nodiscard]] bool isLoaded() const;
    [[nodiscard]] Status status() const;
    virtual void clear();

    [[nodiscard]] Git::Repository *manager() const;

public Q_SLOTS:
    void load();

protected:
    void setStatus(Status newStatus);
    Git::Repository *mGit{nullptr};
    virtual void reload() = 0;

Q_SIGNALS:
    void loaded();
    void statusChanged();

private:
    Status m_status{NotLoaded};
};
