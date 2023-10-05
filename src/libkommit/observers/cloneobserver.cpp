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
    auto observer = reinterpret_cast<Git::CloneObserver *>(payload);

    return 0;
}

void git_helper_checkout_progress_cb(const char *path, size_t completed_steps, size_t total_steps, void *payload)
{
    auto observer = reinterpret_cast<Git::CloneObserver *>(payload);
}

void git_helper_checkout_perfdata_cb(const git_checkout_perfdata *perfdata, void *payload)
{
    auto observer = reinterpret_cast<Git::CloneObserver *>(payload);
}

}

CloneObserver::CloneObserver(QObject *parent)
    : FetchObserver{parent}
{
}

}
