/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"

#ifdef LIBKOMMIT_WIDGET_USE_KF
#include <KXmlGuiWindow>
#endif

class QEventLoop;
class LIBKOMMITWIDGETS_EXPORT AppMainWindow : public
#ifdef LIBKOMMIT_WIDGET_USE_KF
                                              KXmlGuiWindow
#else
                                              QMainWindow
#endif
{
    Q_OBJECT

public:
    enum DialogCode { Rejected, Accepted };

    explicit AppMainWindow(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    int exec();
    void showModal();
    void accept();
    void reject();
    void setVisible(bool visible) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QEventLoop *mLoop{nullptr};
    bool mIsModal{false};
    int mReturnCode{0};
};
