/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>
#include <QString>

#include <git2/submodule.h>

#include "checkoutoptions.h"
#include "fetchoptions.h"
#include "libkommit_export.h"

namespace Git
{

class AddSubmoduleOptionsPrivate;
class LIBKOMMIT_EXPORT AddSubmoduleOptions
{
public:
    AddSubmoduleOptions();

    [[nodiscard]] QString url() const;
    void setUrl(const QString &url);

    [[nodiscard]] QString path() const;
    void setPath(const QString &path);

    FetchOptions *fetchOptions() const;
    CheckoutOptions *checkoutOptions() const;

    void apply(git_submodule_update_options *opts);

private:
    QSharedPointer<AddSubmoduleOptionsPrivate> d;
};

}
