/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "error.h"

#include <git2/errors.h>

#include <QMetaEnum>

namespace Git
{

Error::Error()
{
    _type = static_cast<Type>(git_error_last()->klass);
    _message = QString{git_error_last()->message};
}

QString Error::typeString() const
{
    auto e = QMetaEnum::fromType<Type>();
    return QString{e.valueToKey(static_cast<quint64>(_type))};
}

QString Error::message() const
{
    return _message;
}

int Error::lastType()
{
    return static_cast<Type>(git_error_last()->klass);
}

QString Error::lastTypeString()
{
    auto e = QMetaEnum::fromType<Type>();
    return QString{e.valueToKey(static_cast<quint64>(git_error_last()->klass))};
}

QString Error::lastMessage()
{
    return QString{git_error_last()->message};
}

Error::Type Error::type() const
{
    return _type;
}
}
