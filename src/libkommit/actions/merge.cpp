#include "merge.h"

#include <git2.h>
#include <remote.h>

#include "fetch.h"
#include "reference.h"
#include <Kommit/ReferenceCache>

namespace Git
{

class MergePrivate
{
    Fetch fetch;
    Reference reference;
    Remote remote;
    Branch branch;
};

Merge::Merge(QObject *parent)
    : AbstractAction{parent}
{
}

int Merge::exec()
{
    // d->fetch.setRemote(d->remote);
    // d->fetch.setBranch(d->branch);
    // d->fetch.runAsync();

    // git_annotated_commit* annotated_commit = nullptr;

    // auto remoteReference = repo->references()->findForRemote(d->remote);

    // if (remoteReference.isNull())
    //     return 0;

    // if (git_annotated_commit_from_ref(&annotated_commit, m_repo, remoteReference.data()) != 0) {
    //     printError("Failed to create annotated commit");
    //     return false;
    // }

    // git_merge_analysis_t analysis;
    // git_merge_preference_t preference;
    // const git_annotated_commit *merge_heads[] = {annotated_commit};

    // git_merge_analysis(&analysis, &preference, nullptr, merge_heads, 1);
    return 0;
}

}
