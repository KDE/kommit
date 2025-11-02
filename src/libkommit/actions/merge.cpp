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
public:
    Fetch fetch;
    Reference reference;
    Remote remote;
    Branch branch;

    Branch sourceBranch;
    Branch destinationBranch;
};

Merge::Merge(QObject *parent)
    : AbstractAction{parent}
{
}

int Merge::exec()
{
    auto annotatedCommit = d->sourceBranch.annotatedCommit();

    git_merge_analysis_t analysis;
    git_merge_preference_t preference;
    const git_annotated_commit *merge_heads[] = {annotatedCommit.data()};
    git_oid source_oid;

    if (git_merge_analysis(&analysis, &preference, repo->repoPtr(), merge_heads, 1) != 0) {
        // git_annotated_commit_free(annotatedCommit);
        return 1;
    }

    // Check if merge is needed
    // if (analysis & GIT_MERGE_ANALYSIS_UP_TO_DATE) {
    //     // result.success = true;
    //     // result.message = "Already up-to-date";
    //     // git_annotated_commit_free(annotated_commit);
    //     // git_commit_free(source_commit);
    //     return result;
    // }

    // if (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD) {
    //     // Fast-forward merge
    //     git_reference* new_ref = nullptr;
    //     if (git_reference_set_target(&new_ref, d->destinationBranch.data(), &source_oid, "Fast-forward merge") != 0) {
    //         // result.message = QString("Failed to fast-forward: %1").arg(git_error_last()->message);
    //         // git_annotated_commit_free(annotated_commit);
    //         // git_commit_free(source_commit);
    //         return result;
    //     }

    //     git_reference_free(new_ref);
    //     // result.success = true;
    //     // result.message = "Fast-forward merge completed";
    //     // git_annotated_commit_free(annotated_commit);
    //     // git_commit_free(source_commit);
    //     return result;
    // }

    return 0;
}

}
