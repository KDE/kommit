#pragma once

#include <QList>

#include "filestatus.h"
#include "gitglobal.h"
#include "libkommit_export.h"

#include <git2/diff.h>

namespace Git
{

class FileStatus;

class LIBKOMMIT_EXPORT TreeDiffEntry
{
public:
    TreeDiffEntry(const git_diff_delta *delta);

    Q_REQUIRED_RESULT git_diff_delta *deltaPtr() const;
    Q_REQUIRED_RESULT QString oldFile() const;
    Q_REQUIRED_RESULT QString newFile() const;
    Q_REQUIRED_RESULT ChangeStatus status() const;

private:
    git_diff_delta *mDeltaPtr;

    QString mOldFile;
    QString mNewFile;
    ChangeStatus mStatus;
};

class LIBKOMMIT_EXPORT TreeDiff : public QList<TreeDiffEntry>
{
public:
    TreeDiff();

    bool contains(const QString &entryPath) const;
    ChangeStatus status(const QString &entryPath) const;
};

} // namespace Git
