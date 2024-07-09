/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "appmainwindow.h"
#include "libkommitwidgets_appdebug.h"

#include <QCloseEvent>
#include <QEventLoop>

AppMainWindow::AppMainWindow(QWidget *parent, Qt::WindowFlags f)
#ifdef LIBKOMMIT_WIDGET_USE_KF
    : KXmlGuiWindow(parent, f)
#else
    : QMainWindow{parent}
#endif
{
}

int AppMainWindow::exec()
{
    QEventLoop eventLoop;
    mLoop = &eventLoop;
    showModal();
    (void)eventLoop.exec(QEventLoop::DialogExec);
    mLoop = nullptr;
    qCDebug(KOMMIT_WIDGETS_LOG) << "returnCode=" << mReturnCode;
    return mReturnCode;
}

void AppMainWindow::showModal()
{
    mIsModal = true;
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose, true);
    show();
}

void AppMainWindow::accept()
{
    mReturnCode = Accepted;
}

void AppMainWindow::reject()
{
    mReturnCode = Rejected;
}

void AppMainWindow::setVisible(bool visible)
{
    if (!visible && mLoop)
        mLoop->exit();
    KXmlGuiWindow::setVisible(visible);
}

void AppMainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && mIsModal)
        close();

    KXmlGuiWindow::keyPressEvent(event);
}

#include "moc_appmainwindow.cpp"
