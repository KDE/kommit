/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "cloneobserver.h"

namespace Git
{

namespace CloneCallbacks
{

int git_helper_checkout_notify_cb(git_checkout_notify_t why,
                                  const char *path,
                                  const git_diff_file *baseline,
                                  const git_diff_file *target,
                                  const git_diff_file *workdir,
                                  void *payload)
{
    Q_UNUSED(why)
    Q_UNUSED(path)
    Q_UNUSED(baseline)
    Q_UNUSED(target)
    Q_UNUSED(workdir)
    Q_UNUSED(payload)
    // auto observer = reinterpret_cast<Git::CloneObserver *>(payload);

    return 0;
}

void git_helper_checkout_progress_cb(const char *path, size_t completed_steps, size_t total_steps, void *payload)
{
    auto observer = reinterpret_cast<Git::CloneObserver *>(payload);

    if (!observer)
        return;

    Q_EMIT observer->checkoutProgress(QString{path}, static_cast<int>(completed_steps), static_cast<int>(total_steps));
    return;
}

void git_helper_checkout_perfdata_cb(const git_checkout_perfdata *perfdata, void *payload)
{
    auto observer = reinterpret_cast<Git::CloneObserver *>(payload);

    if (!observer)
        return;

    Q_EMIT observer->checkoutPerfData(perfdata->mkdir_calls, perfdata->stat_calls, perfdata->chmod_calls);
}

} // namespace CloneCallbacks

CloneObserver::CloneObserver(QObject *parent)
    : FetchObserver{nullptr}
{
}

void CloneObserver::init(git_checkout_options *opts)
{
    opts->progress_payload = opts->perfdata_payload = opts->notify_payload = this;
    opts->progress_cb = &CloneCallbacks::git_helper_checkout_progress_cb;
    opts->notify_cb = &CloneCallbacks::git_helper_checkout_notify_cb;
    opts->perfdata_cb = &CloneCallbacks::git_helper_checkout_perfdata_cb;
}
}

#include "moc_cloneobserver.cpp"
