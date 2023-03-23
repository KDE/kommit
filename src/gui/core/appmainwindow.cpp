/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "appmainwindow.h"
#include "kommit_appdebug.h"

#include <QCloseEvent>
#include <QEventLoop>

#include <KCrash>

AppMainWindow::AppMainWindow(QWidget *parent, Qt::WindowFlags f)
    : KXmlGuiWindow(parent, f)
{
}

int AppMainWindow::exec()
{
    KCrash::initialize();
    QEventLoop eventLoop;
    mLoop = &eventLoop;
    showModal();
    (void)eventLoop.exec(QEventLoop::DialogExec);
    mLoop = nullptr;
    qCDebug(KOMMIT_LOG) << "returnCode=" << mReturnCode;
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

// void AppMainWindow::closeDialog(int resultCode)
//{
//     _returnCode = resultCode;
//     if (_loop && _loop->isRunning())
//         _loop->quit();
// }

// void AppMainWindow::closeEvent(QCloseEvent *event)
//{
//     qCDebug(KOMMIT_LOG) << Q_FUNC_INFO;
//     Q_UNUSED(event)
//     if (_loop && _loop->isRunning())
//         _loop->quit();
//     KXmlGuiWindow::closeEvent(event);
// }

void AppMainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && mIsModal)
        close();

    KXmlGuiWindow::keyPressEvent(event);
}
