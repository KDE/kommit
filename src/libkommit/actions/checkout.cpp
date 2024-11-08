/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "checkout.h"

#include "actions/fetch.h"
#include "remotecallbacks.h"

#include <git2/checkout.h>

namespace Git
{

class CheckoutPrivate : public QSharedData
{
    Checkout *q_ptr;
    Q_DECLARE_PUBLIC(Checkout)

public:
    CheckoutPrivate(Checkout *parent, Repository *repo);
    Repository *repo;
};

CheckoutPrivate::CheckoutPrivate(Checkout *parent, Repository *repo)
    : q_ptr{parent}
    , repo{repo}
{
}

Checkout::Checkout(Repository *repo, QObject *parent)
    : AbstractAction{parent}
    , d{new CheckoutPrivate{this, repo}}
{
}

int Checkout::exec()
{
    return 0;
}

}
