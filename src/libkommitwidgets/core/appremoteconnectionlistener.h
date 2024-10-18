/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <Kommit/RemoteConnectionListener>

#include "libkommitwidgets_export.h"

class QWidget;
class LIBKOMMITWIDGETS_EXPORT AppRemoteConnectionListener : public Git::RemoteConnectionListener
{
public:
    static AppRemoteConnectionListener *instance();

    [[nodiscard]] QWidget *parentWidget() const;
    void setParentWidget(QWidget *parentWidget);

    void credentialRequeted(const QString &url, Git::Credential *cred, bool *accept) override;
    void certificateCheck(Git::Certificate *cert, bool *accept) override;

private:
    QWidget *mParentWidget{nullptr};
    int mRetryCount{};
};
