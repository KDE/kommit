/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "checkoutoptions.h"

#include <git2/errors.h>

namespace Git
{

namespace CheckoutCallbacks
{
int git_helper_checkout_notify_cb(git_checkout_notify_t why,
                                  const char *path,
                                  const git_diff_file *baseline,
                                  const git_diff_file *target,
                                  const git_diff_file *workdir,
                                  void *payload)
{
    auto opts = reinterpret_cast<CheckoutOptions *>(payload);

    Q_EMIT opts->checkoutNotify(static_cast<CheckoutOptions::NotifyFlag>(why), QString{path}, DiffFile{baseline}, DiffFile{target}, DiffFile{workdir});
    return GIT_OK;
}

void git_helper_checkout_progress_cb(const char *path, size_t completed_steps, size_t total_steps, void *payload)
{
    auto opts = reinterpret_cast<CheckoutOptions *>(payload);

    Q_EMIT opts->checkoutProgress(QString{path}, completed_steps, total_steps);
}
void git_helper_checkout_perfdata_cb(const git_checkout_perfdata *perfdata, void *payload)
{
    auto opts = reinterpret_cast<CheckoutOptions *>(payload);
    Q_EMIT opts->checkoutPerfData(perfdata->mkdir_calls, perfdata->stat_calls, perfdata->chmod_calls);
}

}

CheckoutOptions::CheckoutOptions(QObject *parent)
    : QObject{parent}
{
}

void CheckoutOptions::apply(git_checkout_options *opts) const
{
    opts->checkout_strategy = static_cast<unsigned int>(mCheckoutStrategies);

    if (!mAncestorLabel.isEmpty())
        opts->ancestor_label = mAncestorLabel.toLocal8Bit().constData();
    if (!mOurLabel.isEmpty())
        opts->our_label = mOurLabel.toLocal8Bit().constData();
    if (!mTheirLabel.isEmpty())
        opts->their_label = mTheirLabel.toLocal8Bit().constData();

    if (mDirMode.isValid())
        opts->dir_mode = mDirMode.value();
    if (mFileMode.isValid())
        opts->file_mode = mFileMode.value();

    opts->notify_cb = CheckoutCallbacks::git_helper_checkout_notify_cb;
    opts->notify_payload = const_cast<CheckoutOptions *>(this);
    opts->notify_flags = static_cast<git_checkout_notify_t>(mNotifyFlags.toInt());

    opts->progress_cb = CheckoutCallbacks::git_helper_checkout_progress_cb;
    opts->progress_payload = const_cast<CheckoutOptions *>(this);

    opts->perfdata_cb = CheckoutCallbacks::git_helper_checkout_perfdata_cb;
    opts->perfdata_payload = const_cast<CheckoutOptions *>(this);
}

QString CheckoutOptions::ancestorLabel() const
{
    return mAncestorLabel;
}

void CheckoutOptions::setAncestorLabel(const QString &newAncestorLabel)
{
    mAncestorLabel = newAncestorLabel;
}

QString CheckoutOptions::ourLabel() const
{
    return mOurLabel;
}

void CheckoutOptions::setOurLabel(const QString &newOurLabel)
{
    mOurLabel = newOurLabel;
}

QString CheckoutOptions::theirLabel() const
{
    return mTheirLabel;
}

void CheckoutOptions::setTheirLabel(const QString &newTheirLabel)
{
    mTheirLabel = newTheirLabel;
}

FileMode CheckoutOptions::dirMode() const
{
    return mDirMode;
}

void CheckoutOptions::setDirMode(const FileMode &newDirMode)
{
    mDirMode = newDirMode;
}

FileMode CheckoutOptions::fileMode() const
{
    return mFileMode;
}

void CheckoutOptions::setFileMode(const FileMode &newFileMode)
{
    mFileMode = newFileMode;
}

CheckoutOptions::NotifyFlags CheckoutOptions::notifyFlags() const
{
    return mNotifyFlags;
}

void CheckoutOptions::setNotifyFlags(const NotifyFlags &newNotifyFlags)
{
    mNotifyFlags = newNotifyFlags;
}

CheckoutOptions::CheckoutStrategies CheckoutOptions::checkoutStrategies() const
{
    return mCheckoutStrategies;
}

void CheckoutOptions::setCheckoutStrategies(const CheckoutStrategies &newCheckoutStrategies)
{
    mCheckoutStrategies = newCheckoutStrategies;
}

} // namespace Git
