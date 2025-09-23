/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "refspec.h"

namespace Git
{

class RefSpecPrivate
{
public:
    explicit RefSpecPrivate(const git_refspec *refspec = nullptr);
    const git_refspec *refspec{nullptr};
};

RefSpecPrivate::RefSpecPrivate(const git_refspec *refspec)
    : refspec{refspec}
{
}

RefSpec::RefSpec(const git_refspec *refspec)
    : d{new RefSpecPrivate{refspec}}
{
}

RefSpec::RefSpec(const RefSpec &other)
    : d{other.d}
{
}

RefSpec &RefSpec::operator=(const RefSpec &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool RefSpec::operator==(const RefSpec &other) const
{
    return d->refspec == other.d->refspec;
}

bool RefSpec::operator!=(const RefSpec &other) const
{
    return !(*this == other);
}

QString RefSpec::name() const
{
    return QString{git_refspec_string(d->refspec)};
}

RefSpec::Direction RefSpec::direction() const
{
    return static_cast<Direction>(git_refspec_direction(d->refspec));
}

QString RefSpec::destination() const
{
    return QString{git_refspec_dst(d->refspec)};
}

QString RefSpec::source() const
{
    return QString{git_refspec_src(d->refspec)};
}

bool RefSpec::isNull() const
{
    return !d->refspec;
}

}
