/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

#include <Kommit/Certificate>
#include <git2/proxy.h>

namespace Git
{
class Credential;

class Proxy : public QObject
{
    Q_OBJECT
public:
    enum class Type { None = GIT_PROXY_NONE, Auto = GIT_PROXY_AUTO, Specified = GIT_PROXY_SPECIFIED };

    void apply(git_proxy_options *opts);

    [[nodiscard]] QString url() const;
    void setUrl(const QString &url);

    [[nodiscard]] Type type() const;
    void setType(Type type);

Q_SIGNALS:
    void credentialRequested(const QString &url, Git::Credential *cred, bool *accept);
    void certificateCheck(const Git::Certificate &cert, bool *accept);

private:
    QString mUrl;
    Type mType{Type::None};
};

}
