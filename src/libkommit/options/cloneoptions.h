/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>

#include <git2/clone.h>

#include "libkommit_export.h"

namespace Git
{

class CheckoutOptions;
class FetchOptions;
class Repository;
class CloneOptionsPrivate;
class LIBKOMMIT_EXPORT CloneOptions
{
    Q_GADGET

public:
    explicit CloneOptions(Repository *repo);
    ~CloneOptions();

    void apply(git_clone_options *opts);

    [[nodiscard]] QString url() const;
    void setUrl(const QString &url);

    [[nodiscard]] QString localPath() const;
    void setLocalPath(const QString &localPath);

    [[nodiscard]] QString checkoutBranchName() const;
    void setCheckoutBranchName(const QString &checkoutBranchName);

    [[nodiscard]] CheckoutOptions *checkoutOptions() const;
    [[nodiscard]] FetchOptions *fetchOptions() const;

private:
    QSharedPointer<CloneOptionsPrivate> d;
};

}
