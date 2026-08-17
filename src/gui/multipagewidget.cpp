/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "multipagewidget.h"
#include "widgetbase.h"

#include <QAction>
#include <QActionGroup>
#include <QScrollBar>
#include <QStyle>
#include <QStyleHints>
#include <QToolButton>

namespace
{
/// The strip is no narrower than this, however short the names on it are.
constexpr int SmallestStripWidth = 120;
}

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
    if (event->type() == QEvent::Resize)
        scheduleStripUpdate();

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

    connect(scrollAreaPages->verticalScrollBar(), &QScrollBar::rangeChanged, this, &MultiPageWidget::scheduleStripUpdate);
}

void MultiPageWidget::scheduleStripUpdate()
{
    if (mStripUpdateScheduled)
        return;

    // After the layout has settled: whether a scroll bar is there is only known once it has,
    // and the width and the margins are both worked out from that.
    mStripUpdateScheduled = true;
    QMetaObject::invokeMethod(
        this,
        [this] {
            mStripUpdateScheduled = false;
            updateStripWidth();
        },
        Qt::QueuedConnection);
}

void MultiPageWidget::updateStripWidth()
{
    // The strip is as wide as the widest of the names on it, so a name is never cut in half.
    int widest{0};
    for (int i = 0; i < verticalLayoutButtons->count(); ++i) {
        if (auto *button = verticalLayoutButtons->itemAt(i)->widget())
            widest = qMax(widest, button->sizeHint().width());
    }

    if (!widest)
        return;

    // Room for the scroll bar as well, which comes and goes with the height of the window, so
    // a name is not cut short when it appears.
    const int reserved = style()->pixelMetric(QStyle::PM_ScrollBarExtent);

    // An even width for the buttons, so an icon or a name of even width lands exactly in the
    // middle of one rather than half a pixel off it.
    const int frames = 2 * scrollAreaPages->frameWidth();
    const int width = qMax(SmallestStripWidth, widest + reserved + frames);

    scrollAreaPages->setFixedWidth(width + ((width - reserved - frames) % 2));
}

void MultiPageWidget::updateStyleSheet()
{
    const bool isDarkTheme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;

    const auto styleSheet =
        QStringLiteral(R"CSS(
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
        action->setShortcut(QKeySequence(Qt::CTRL | keys[mActionGroup->actions().size()]));
    btn->setDefaultAction(action);
    mActionGroup->addAction(action);

    stackedWidget->addWidget(widget);

    widget->layout()->setContentsMargins({});

    verticalLayoutButtons->insertWidget(mActionGroup->actions().size() - 1, btn);

    scheduleStripUpdate();
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

// bool MultiPageWidget::event(QEvent *e)
// {
//     if (e->type() == QEvent::PaletteChange) {
//         updateStyleSheet();
//     }
//     return QWidget::event(e);
// }

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
