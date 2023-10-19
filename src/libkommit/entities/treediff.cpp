#include "treediff.h"
#include "filestatus.h"

namespace Git
{

TreeDiff::TreeDiff()
{
}

bool TreeDiff::contains(const QString &entryPath) const
{
    auto i = std::find_if(begin(), end(), [&entryPath](const TreeDiffEntry &f) {
        return f.oldFile() == entryPath || f.newFile() == entryPath;
    });
    return i != end();
}

ChangeStatus TreeDiff::status(const QString &entryPath) const
{
    auto i = std::find_if(begin(), end(), [&entryPath](const TreeDiffEntry &f) {
        return f.oldFile() == entryPath || f.newFile() == entryPath;
    });
    if (i == end())
        return ChangeStatus::Unmodified;

    return (*i).status();
}

TreeDiffEntry::TreeDiffEntry()
{
}

TreeDiffEntry::TreeDiffEntry(const git_diff_delta *delta)
{
    mOldFile = delta->old_file.path;
    mNewFile = delta->new_file.path;

    switch (delta->status) {
    case GIT_DELTA_UNMODIFIED:
        mStatus = ChangeStatus::Unmodified;
        break;
    case GIT_DELTA_ADDED:
        mStatus = ChangeStatus::Added;
        break;
    case GIT_DELTA_DELETED:
        mStatus = ChangeStatus::Removed;
        break;
    case GIT_DELTA_MODIFIED:
        mStatus = ChangeStatus::Modified;
        break;
    case GIT_DELTA_RENAMED:
        mStatus = ChangeStatus::Renamed;
        break;
    case GIT_DELTA_COPIED:
        mStatus = ChangeStatus::Copied;
        break;
    case GIT_DELTA_IGNORED:
        mStatus = ChangeStatus::Ignored;
        break;
    case GIT_DELTA_UNTRACKED:
        mStatus = ChangeStatus::Untracked;
        break;
    case GIT_DELTA_TYPECHANGE:
        mStatus = ChangeStatus::Typechange;
        break;
    case GIT_DELTA_UNREADABLE:
        mStatus = ChangeStatus::Unreadable;
        break;
    case GIT_DELTA_CONFLICTED:
        mStatus = ChangeStatus::Conflicted;
        break;
    }
}

git_diff_delta *TreeDiffEntry::deltaPtr() const
{
    return mDeltaPtr;
}

QString TreeDiffEntry::oldFile() const
{
    return mOldFile;
}

QString TreeDiffEntry::newFile() const
{
    return mNewFile;
}

ChangeStatus TreeDiffEntry::status() const
{
    return mStatus;
}

bool TreeDiffEntry::operator==(const TreeDiffEntry &other)
{
    if (!mDeltaPtr)
        return false;
    return mDeltaPtr == other.mDeltaPtr;
}

} // namespace Git
