/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

//
// Created by hamed on 25.03.22.
//

#pragma once

#include <QAbstractListModel>

namespace Git {

class Manager;
class AbstractGitItemsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)

public:
    explicit AbstractGitItemsModel(Manager *git, QObject *parent = nullptr);

    enum Status { NotLoaded, Loading, Loaded };
    Q_ENUM(Status)
    bool isLoaded() const;
    Status status() const;

public slots:
    void load();

protected:
    void setStatus(Status newStatus);
    Manager *_git{nullptr};
    virtual void fill() = 0;

signals:
    void loaded();
    void statusChanged();

private:
    Status m_status{NotLoaded};
};
} // namespace Git

