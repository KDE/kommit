/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "checkoutaction.h"

#include "actions/fetch.h"
#include "remotecallbacks.h"

#include <git2/checkout.h>

#include "repository.h"

#include <Kommit/FileDelta>

namespace Git
{

namespace CheckoutActionCallbacks
{
int git_helper_checkout_notify_cb(git_checkout_notify_t why,
                                  const char *path,
                                  const git_diff_file *baseline,
                                  const git_diff_file *target,
                                  const git_diff_file *workdir,
                                  void *payload)
{
    auto opts = reinterpret_cast<CheckoutAction *>(payload);

    Q_EMIT opts->checkoutNotify(static_cast<CheckoutAction::NotifyFlag>(why), QString{path}, DiffFile{baseline}, DiffFile{target}, DiffFile{workdir});
    return GIT_OK;
}

void git_helper_checkout_progress_cb(const char *path, size_t completed_steps, size_t total_steps, void *payload)
{
    auto opts = reinterpret_cast<CheckoutAction *>(payload);

    Q_EMIT opts->checkoutProgress(QString{path}, completed_steps, total_steps);
}
void git_helper_checkout_perfdata_cb(const git_checkout_perfdata *perfdata, void *payload)
{
    auto opts = reinterpret_cast<CheckoutAction *>(payload);
    Q_EMIT opts->checkoutPerfData(perfdata->mkdir_calls, perfdata->stat_calls, perfdata->chmod_calls);
}

}


class CheckoutPrivate : public QSharedData
{
public:
    CheckoutPrivate(Repository *repo);

    Repository *repo;
    Object target;
    QString ancestorLabel;
    QString ourLabel;
    QString theirLabel;
    FileMode dirMode;
    FileMode fileMode;
    CheckoutAction::NotifyFlags notifyFlags{CheckoutAction::NotifyFlag::All};
    CheckoutAction::CheckoutStrategies checkoutStrategies{CheckoutAction::CheckoutStrategy::Safe};
};

CheckoutPrivate::CheckoutPrivate(Repository *repo)
    : repo{repo}
{
}

CheckoutAction::CheckoutAction(Repository *repo, QObject *parent)
    : AbstractAction{parent}
    , d{new CheckoutPrivate{repo}}
{
}

Object CheckoutAction::target() const
{
    return d->target;
}

void CheckoutAction::setTarget(const Object &target)
{
    d->target = target;
}

QString CheckoutAction::ancestorLabel() const
{
    return d->ancestorLabel;
}

void CheckoutAction::setAncestorLabel(const QString &ancestorLabel)
{
    d->ancestorLabel = ancestorLabel;
}

QString CheckoutAction::ourLabel() const
{
    return d->ourLabel;
}

void CheckoutAction::setOurLabel(const QString &ourLabel)
{
    d->ourLabel = ourLabel;
}

QString CheckoutAction::theirLabel() const
{
    return d->theirLabel;
}

void CheckoutAction::setTheirLabel(const QString &theirLabel)
{
    d->theirLabel = theirLabel;
}

FileMode CheckoutAction::fileMode() const
{
    return d->fileMode;
}

void CheckoutAction::setFileMode(const FileMode &fileMode)
{
    d->fileMode = fileMode;
}

FileMode CheckoutAction::dirMode() const
{
    return d->dirMode;
}

void CheckoutAction::setDirMode(const FileMode &dirMode)
{
    d->dirMode = dirMode;
}

CheckoutAction::NotifyFlags CheckoutAction::notifyFlags() const
{
    return d->notifyFlags;
}

void CheckoutAction::setNotifyFlags(const NotifyFlags &notifyFlags)
{
    d->notifyFlags = notifyFlags;
}

CheckoutAction::CheckoutStrategies CheckoutAction::checkoutStrategies() const
{
    return d->checkoutStrategies;
}

void CheckoutAction::setCheckoutStrategies(const CheckoutStrategies &checkoutStrategies)
{
    d->checkoutStrategies = checkoutStrategies;
}

int CheckoutAction::exec()
{
    if (d->target.isNull())
        return -1;

    git_checkout_options opts;
    git_checkout_options_init(&opts, GIT_CHECKOUT_OPTIONS_VERSION);

    opts.checkout_strategy = static_cast<unsigned int>(d->checkoutStrategies);

    if (!d->ancestorLabel.isEmpty())
        opts.ancestor_label = d->ancestorLabel.toLocal8Bit().constData();
    if (!d->ourLabel.isEmpty())
        opts.our_label = d->ourLabel.toLocal8Bit().constData();
    if (!d->theirLabel.isEmpty())
        opts.their_label = d->theirLabel.toLocal8Bit().constData();

    if (d->dirMode.isValid())
        opts.dir_mode = d->dirMode.value();
    if (d->fileMode.isValid())
        opts.file_mode = d->fileMode.value();

    opts.notify_cb = CheckoutActionCallbacks::git_helper_checkout_notify_cb;
    opts.notify_payload = this;
    opts.notify_flags = static_cast<git_checkout_notify_t>(d->notifyFlags.toInt());

    opts.progress_cb = CheckoutActionCallbacks::git_helper_checkout_progress_cb;
    opts.progress_payload = this;

    opts.perfdata_cb = CheckoutActionCallbacks::git_helper_checkout_perfdata_cb;
    opts.perfdata_payload = this;

    return git_checkout_tree(d->repo->repoPtr(), d->target.constData(), &opts);
}

}
