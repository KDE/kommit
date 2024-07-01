#include "commitscache.h"
#include "entities/commit.h"
#include "gitglobal_p.h"

#include <git2/commit.h>
#include <git2/revparse.h>

namespace Git
{

CommitsCache::CommitsCache(git_repository *repo)
    : AbstractCache<Commit>{repo}
{
}

QList<QSharedPointer<Commit>> CommitsCache::commitsInBranch(QSharedPointer<Branch> branch)
{
    return {};
}

Commit *CommitsCache::lookup(const QString &key, git_repository *repo)
{
    git_commit *commit;
    git_object *commitObject;
    BEGIN
    STEP git_revparse_single(&commitObject, repo, key.toLatin1().constData());
    STEP git_commit_lookup(&commit, repo, git_object_id(commitObject));
    if (IS_OK)
        return new Commit{commit};
    return nullptr;
}

};
