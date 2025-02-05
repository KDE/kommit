/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "multipagewidget.h"
#include "widgetbase.h"

#include <QAction>
#include <QActionGroup>
#include <QStyleHints>
#include <QToolButton>

Git::Repository *MultiPageWidget::defaultGitManager() const
{
    return mDefaultGitManager;
}

void MultiPageWidget::setDefaultGitManager(Git::Repository *newDefaultGitManager)
{
    mDefaultGitManager = newDefaultGitManager;
}

int MultiPageWidget::count() const
{
    return mActionGroup->actions().size();
}

bool MultiPageWidget::event(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        qDebug() << "Change theme";
        updateTheme();
    }
    return QWidget::event(event);
}

MultiPageWidget::MultiPageWidget(QWidget *parent)
    : QWidget(parent)
    , mActionGroup(new QActionGroup(this))
{
    Q_SET_OBJECT_NAME(mActionGroup);

    setupUi(this);
    updateStyleSheet();

    connect(mActionGroup, &QActionGroup::triggered, this, &MultiPageWidget::slotPageSelected);
}

void MultiPageWidget::updateStyleSheet()
{
    const bool isDarkTheme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;

    const auto styleSheet = QStringLiteral(R"CSS(
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
                                .arg(isDarkTheme ? palette().color(QPalette::Highlight).darker().rgba() : palette().color(QPalette::Highlight).lighter().rgba(), 0, 16)
                                .arg(palette().color(QPalette::Highlight).rgba(), 0, 16);

    scrollAreaWidgetContents->setStyleSheet(styleSheet);
    updateTheme();
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
    action->setData(mActionGroup->actions().size());
    if (mActionGroup->actions().size() < 10)
        action->setShortcut(QKeySequence(Qt::CTRL + keys[mActionGroup->actions().size()]));
    btn->setDefaultAction(action);
    mActionGroup->addAction(action);

    stackedWidget->addWidget(widget);

    widget->layout()->setContentsMargins({});

    verticalLayoutButtons->insertWidget(mActionGroup->actions().size() - 1, btn);
}

void MultiPageWidget::addPage(WidgetBase *widget, QAction *action, const QIcon &icon)
{
    auto btn = new QToolButton(this);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

    if (!action)
        action = new QAction(this);
    action->setText(widget->windowTitle());

    if (icon.isNull())
        action->setIcon(widget->windowIcon());
    else
        action->setIcon(icon);
    action->setCheckable(true);
    action->setData(mActionGroup->actions().size());
    btn->setDefaultAction(action);
    mActionGroup->addAction(action);

    stackedWidget->addWidget(widget);

    widget->layout()->setContentsMargins({});

    verticalLayoutButtons->insertWidget(mActionGroup->actions().size() - 1, btn);
}

void MultiPageWidget::setCurrentIndex(int index)
{
    mActionGroup->actions().at(index)->trigger();
}

QList<QAction *> MultiPageWidget::actions() const
{
    return mActionGroup->actions();
}

void MultiPageWidget::slotPageSelected(QAction *action)
{
    stackedWidget->setCurrentIndex(action->data().toInt());
    labelTitle->setText(action->text().remove(QLatin1Char('&')));
    labelPageIcon->setPixmap(action->icon().pixmap({32, 32}));
}

bool MultiPageWidget::event(QEvent *e)
{
    if (e->type() == QEvent::PaletteChange) {
        updateStyleSheet();
    }
    return QWidget::event(e);
void MultiPageWidget::updateTheme()
{
    const auto styleSheet = QStringLiteral(R"CSS(
        #scrollAreaWidgetContents {
            color: #%4;
            background-color: #%1;
        }
        QToolButton {
            color: #%4;
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
        }

)CSS")
                                .arg(palette().color(QPalette::Window).rgba(), 0, 16)
                                .arg(palette().color(QPalette::Active, QPalette::Button).rgba(), 0, 16)
                                .arg(palette().color(QPalette::Highlight).rgba(), 0, 16)
                                .arg(palette().color(QPalette::ButtonText).rgba(), 0, 16);
    scrollAreaWidgetContents->setStyleSheet(styleSheet);
}

#include "moc_multipagewidget.cpp"
