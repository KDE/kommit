/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "proxy.h"

#include <git2/errors.h>

#include <Kommit/Credential>

namespace Git
{

namespace ProxyCallbacks
{

int git_helper_credentials_cb(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload)
{
    auto bridge = reinterpret_cast<Proxy *>(payload);

    if (!Q_UNLIKELY(bridge))
        return GIT_EUSER;

    Credential cred;

    cred.setUsername(username_from_url);
    cred.setAllowedTypes(static_cast<Credential::AllowedTypes>(allowed_types));

    bool accept{false};

    Q_EMIT bridge->credentialRequested(QString{url}, &cred, &accept);

    if (!accept)
        return GIT_PASSTHROUGH;

    git_credential_userpass_plaintext_new(out, cred.username().toLocal8Bit().data(), cred.password().toLocal8Bit().data());

    return 0;
}

int git_helper_transport_certificate_check(git_cert *cert, int valid, const char *host, void *payload)
{
    auto bridge = reinterpret_cast<Proxy *>(payload);

    if (Q_UNLIKELY(!bridge))
        return GIT_EUSER;

    Certificate c{cert, static_cast<bool>(valid), QString{host}};

    bool b{true};

    Q_EMIT bridge->certificateCheck(c, &b);
    return b ? 0 : -1;
}
}

void Proxy::apply(git_proxy_options *opts)
{
    opts->type = static_cast<git_proxy_t>(mType);
    opts->url = mUrl.toUtf8().data();
    opts->certificate_check = ProxyCallbacks::git_helper_transport_certificate_check;
    opts->credentials = ProxyCallbacks::git_helper_credentials_cb;
    opts->payload = this;
}
QString Proxy::url() const
{
    return mUrl;
}
void Proxy::setUrl(const QString &url)
{
    mUrl = url;
}
Proxy::Type Proxy::type() const
{
    return mType;
}
void Proxy::setType(Proxy::Type type)
{
    mType = type;
}

}
#include "moc_proxy.cpp"
