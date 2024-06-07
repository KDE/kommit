/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QAction>
#include <QMap>
#include <QObject>

#include "libkommitwidgets_export.h"

#define ADD_ACTION_IMPL(name, text, slot, enabled)                                                                                                             \
    _##name = new QAction(this);                                                                                                                               \
    _##name->setText(i18n(text));                                                                                                                              \
    _actions.append(_##name);                                                                                                                                  \
    setActionEnabled(_##name, enabled);                                                                                                                        \
    connect(_##name, &QAction::triggered, this, slot);

#define ADD_HIDDEN_ACTION(name, text, slot) ADD_ACTION_IMPL(name, text, slot, true)

#define ADD_ACTION_ENABLED(name, text, slot)                                                                                                                   \
    ADD_ACTION_IMPL(name, text, slot, true);                                                                                                                   \
    _menu->addAction(_##name);

#define ADD_ACTION_DISABLED(name, text, slot)                                                                                                                  \
    ADD_ACTION_IMPL(name, text, slot, false);                                                                                                                  \
    _menu->addAction(_##name);

#define ADD_ACTION(name, text, slot) ADD_ACTION_ENABLED(name, text, slot);

#define DEFINE_ACTION(name)                                                                                                                                    \
private:                                                                                                                                                       \
    QAction *_##name;                                                                                                                                          \
                                                                                                                                                               \
public:                                                                                                                                                        \
    QAction *name() const                                                                                                                                      \
    {                                                                                                                                                          \
        return _##name;                                                                                                                                        \
    }

namespace Git
{
class Manager;
}

class QWidget;
class QMenu;
class LIBKOMMITWIDGETS_EXPORT AbstractActions : public QObject
{
    Q_OBJECT

protected:
    Git::Manager *mGit = nullptr;
    QList<QAction *> mActions;
    QMenu *mMenu = nullptr;
    QWidget *mParent = nullptr;
    void setActionEnabled(QAction *action, bool enabled = true);

    template<class Receiver, class Func>
    inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, QAction>::type *
    addAction(const QString &text, const Receiver *receiver, Func slot, bool enabled = true, bool addToMenu = true)
    {
        auto a = createAction(text, enabled, addToMenu);
        connect(a, &QAction::triggered, receiver, slot);
        return a;
    }

    template<class Receiver, class Func>
    inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, QAction>::type *
    addActionHidden(const QString &text, const Receiver *receiver, Func slot)
    {
        return addAction(text, receiver, slot, true, false);
    }

    template<class Receiver, class Func>
    inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, QAction>::type *
    addActionDisabled(const QString &text, const Receiver *receiver, Func slot)
    {
        return addAction(text, receiver, slot, false, true);
    }

public:
    explicit AbstractActions(Git::Manager *git, QWidget *parent);
    void popup();
    void popup(const QPoint &pos);

private Q_SLOTS:
    void gitReloaded();

private:
    QAction *createAction(const QString &text, bool enabled, bool addToMenu);
    QMap<QAction *, bool> mActionStatuses;
};
