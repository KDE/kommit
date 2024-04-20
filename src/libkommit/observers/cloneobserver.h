/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "fetchobserver.h"
#include "libkommit_export.h"
#include <git2/checkout.h>
#include <git2/types.h>

namespace Git
{
namespace CloneCallbacks
{
int git_helper_checkout_notify_cb(git_checkout_notify_t why,
                                  const char *path,
                                  const git_diff_file *baseline,
                                  const git_diff_file *target,
                                  const git_diff_file *workdir,
                                  void *payload);

void git_helper_checkout_progress_cb(const char *path, size_t completed_steps, size_t total_steps, void *payload);

void git_helper_checkout_perfdata_cb(const git_checkout_perfdata *perfdata, void *payload);
}

class LIBKOMMIT_EXPORT CloneObserver : public FetchObserver
{
public:
    explicit CloneObserver(QObject *parent = nullptr);

    void init(git_checkout_options *opts);
};

}
