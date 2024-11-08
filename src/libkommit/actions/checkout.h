/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractaction.h"

#include <QExplicitlySharedDataPointer>
#include <QString>

#include <Kommit/FileMode>

namespace Git
{

class CheckoutPrivate;
class Repository;
class Checkout : public AbstractAction
{
public:
    Checkout(Repository *repo, QObject *parent = nullptr);

protected:
    int exec() override;

private:
    QExplicitlySharedDataPointer<CheckoutPrivate> d;
};

}
