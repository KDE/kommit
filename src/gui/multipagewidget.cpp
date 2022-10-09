/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "multipagewidget.h"
#include "widgetbase.h"

#include <QAction>
#include <QToolButton>

Git::Manager *MultiPageWidget::defaultGitManager() const
{
    return mDefaultGitManager;
}

void MultiPageWidget::setDefaultGitManager(Git::Manager *newDefaultGitManager)
{
    mDefaultGitManager = newDefaultGitManager;
}

int MultiPageWidget::count() const
{
    return m_actionGroup->actions().size();
}

MultiPageWidget::MultiPageWidget(QWidget *parent)
    : QWidget(parent)
    , m_actionGroup(new QActionGroup(this))
{
    Q_SET_OBJECT_NAME(m_actionGroup);

    setupUi(this);

    connect(m_actionGroup, &QActionGroup::triggered, this, &MultiPageWidget::slotPageSelected);
    auto styleSheet = QString(R"CSS(
        #scrollAreaWidgetContents {
            background-color: #%1;
        }
        QToolButton {
            background-color: #%1;
            border: none;
            padding-top: 10px;
            padding-bottom: 10px;
            height: 48px;
        }

        QToolButton:hover {
            background-color: #%2;
        }

        QToolButton:checked {
            background-color: #%3;
            color: white;
        }

)CSS")
                          .arg(palette().color(QPalette::Base).rgba(), 0, 16)
                          .arg(palette().color(QPalette::Highlight).lighter().rgba(), 0, 16)
                          .arg(palette().color(QPalette::Highlight).rgba(), 0, 16);

    scrollAreaWidgetContents->setStyleSheet(styleSheet);
}

void MultiPageWidget::addPage(const QString &title, const QIcon &icon, WidgetBase *widget)
{
    const QList<Qt::Key> keys = {Qt::Key_0, Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9};
    auto btn = new QToolButton(this);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto action = new QAction(this);
    action->setText(title);
    action->setIcon(icon);
    action->setCheckable(true);
    action->setData(m_actionGroup->actions().size());
    if (m_actionGroup->actions().size() < 10)
        action->setShortcut(QKeySequence(Qt::CTRL + keys[m_actionGroup->actions().size()]));
    btn->setDefaultAction(action);
    m_actionGroup->addAction(action);

    stackedWidget->addWidget(widget);

    widget->layout()->setContentsMargins(0, 0, 0, 0);

    verticalLayoutButtons->insertWidget(m_actionGroup->actions().size() - 1, btn);
}

void MultiPageWidget::addPage(WidgetBase *widget, QAction *action)
{
    auto btn = new QToolButton(this);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

    if (!action)
        action = new QAction(this);
    action->setText(widget->windowTitle());
    action->setIcon(widget->windowIcon());
    action->setCheckable(true);
    action->setData(m_actionGroup->actions().size());
    btn->setDefaultAction(action);
    m_actionGroup->addAction(action);

    stackedWidget->addWidget(widget);

    widget->layout()->setContentsMargins(0, 0, 0, 0);

    verticalLayoutButtons->insertWidget(m_actionGroup->actions().size() - 1, btn);
}

void MultiPageWidget::setCurrentIndex(int index)
{
    m_actionGroup->actions().at(index)->trigger();
}

QList<QAction *> MultiPageWidget::actions() const
{
    return m_actionGroup->actions();
}

void MultiPageWidget::slotPageSelected(QAction *action)
{
    stackedWidget->setCurrentIndex(action->data().toInt());
    labelTitle->setText(action->text().remove(QLatin1Char('&')));
    labelPageIcon->setPixmap(action->icon().pixmap({32, 32}));
}
