/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>
#include <QString>

#include <git2/buffer.h>
#include <git2/remote.h>
#include <git2/status.h>
#include <git2/strarray.h>
#include <git2/types.h>
#include <git2/merge.h>

#include "libkommit_export.h"
#include "libkommit_global.h"

namespace Git
{

Q_NAMESPACE

enum class MergeAnalysis {
    None = GIT_MERGE_ANALYSIS_NONE ,
    Normal = GIT_MERGE_ANALYSIS_NORMAL,
    UpToDate = GIT_MERGE_ANALYSIS_UP_TO_DATE,
    FastForward = GIT_MERGE_ANALYSIS_FASTFORWARD,
    Unborn = GIT_MERGE_ANALYSIS_UNBORN,
};

enum class Redirect {
    None = GIT_REMOTE_REDIRECT_NONE,
    Initial = GIT_REMOTE_REDIRECT_INITIAL,
    All = GIT_REMOTE_REDIRECT_ALL
};
Q_ENUM_NS(Redirect)

enum class StatusFlag {
    Current = GIT_STATUS_CURRENT,

    IndexNew = GIT_STATUS_INDEX_NEW,
    IndexModified = GIT_STATUS_INDEX_MODIFIED,
    IndexDeleted = GIT_STATUS_INDEX_DELETED,
    IndexRenamed = GIT_STATUS_INDEX_RENAMED,
    IndexTypechange = GIT_STATUS_INDEX_TYPECHANGE,

    WtNew = GIT_STATUS_WT_NEW,
    WtModified = GIT_STATUS_WT_MODIFIED,
    WtDeleted = GIT_STATUS_WT_DELETED,
    WtTypechange = GIT_STATUS_WT_TYPECHANGE,
    WtRenamed = GIT_STATUS_WT_RENAMED,
    WtUnreadable = GIT_STATUS_WT_UNREADABLE,

    Ignored = GIT_STATUS_IGNORED,
    Conflicted = GIT_STATUS_CONFLICTED,

    New = IndexNew | WtNew,
    Modified = IndexModified | WtModified,
    Deleted = IndexDeleted | WtDeleted,
};
Q_DECLARE_FLAGS(StatusFlags, StatusFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(StatusFlags)

enum DeltaFlag {
    Unmodified = GIT_DELTA_UNMODIFIED,
    Added = GIT_DELTA_ADDED,
    Deleted = GIT_DELTA_DELETED,
    Modified = GIT_DELTA_MODIFIED,
    Renamed = GIT_DELTA_RENAMED,
    Copied = GIT_DELTA_COPIED,
    Ignored = GIT_DELTA_IGNORED,
    Untracked = GIT_DELTA_UNTRACKED,
    Typechange = GIT_DELTA_TYPECHANGE,
    Unreadable = GIT_DELTA_UNREADABLE,
    Conflicted = GIT_DELTA_CONFLICTED,
};
// Q_DECLARE_FLAGS(DeltaFlags,DeltaFlag)
// Q_DECLARE_OPERATORS_FOR_FLAGS(DeltaFlags)

class Branch;
class Commit;
class Tag;
class Remote;

QString convertToQString(git_buf *buf);
QStringList convert(git_strarray *arr);
QString convertToString(const git_oid *id, int len);
void addToArray(git_strarray *arr, const QString &value);

ChangeStatus toChangeStatus(StatusFlags flags);
ChangeStatus toChangeStatus(DeltaFlag flags);

#define toConstChars(s) s.toLocal8Bit().constData()
}
