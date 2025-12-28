/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "libkommit_export.h"

#include <QStringList>
#include <git2/tree.h>

#include "types.h"

namespace Git
{

namespace Deleters
{

void deleteGitTree(git_tree *tree)
{
    git_tree_free(tree);
}

}

QString convertToQString(git_buf *buf)
{
    return QString(buf->ptr);
}

QStringList convert(git_strarray *arr)
{
    QStringList list;
    list.reserve(arr->count);
    for (size_t i = 0; i < arr->count; ++i) {
        list << QString{arr->strings[i]};
    }
    return list;
}

QString convertToString(const git_oid *id, const int len)
{
    QString result;
    QString s;
    for (int i = 0; i < len; i++) {
        s = QStringLiteral("%1").arg(id->id[i], 0, 16);

        if (s.length() == 1)
            result.append(QLatin1Char('0'));

        result.append(s);
    }

    return result;
}

void addToArray(git_strarray *arr, const QString &value)
{
    arr->strings = new char *[1];
    arr->strings[0] = value.toLocal8Bit().data();
    arr->count = 1;
}

void addToArray(git_strarray *arr, const QStringList &list)
{
    arr->strings = new char *[list.size()];
    for (int i = 0; i < list.size(); ++i) {
        arr->strings[i] = list.at(i).toLocal8Bit().data();
    }
    arr->count = 1;
}

LIBKOMMIT_EXPORT ChangeStatus toChangeStatus(StatusFlags flags)
{
    if (flags & StatusFlag::Modified)
        return ChangeStatus::Modified;
    if (flags & StatusFlag::New)
        return ChangeStatus::Added;
    if (flags & StatusFlag::Deleted)
        return ChangeStatus::Removed;
    if (flags & StatusFlag::Ignored)
        return ChangeStatus::Ignored;
    if (flags & StatusFlag::Current)
        return ChangeStatus::Unmodified;
    if (flags & StatusFlag::WtUnreadable)
        return ChangeStatus::Unreadable;
    if (flags & StatusFlag::Conflicted)
        return ChangeStatus::Conflicted;

    return ChangeStatus::Modified;
}

ChangeStatus toChangeStatus(DeltaFlag deltaStatus)
{
    switch (deltaStatus) {
    case DeltaFlag::Unmodified:
        return ChangeStatus::Unmodified;
    case DeltaFlag::Added:
        return ChangeStatus::Added;
    case DeltaFlag::Deleted:
        return ChangeStatus::Removed;
    case DeltaFlag::Modified:
        return ChangeStatus::Modified;
    case DeltaFlag::Renamed:
        return ChangeStatus::Renamed;
    case DeltaFlag::Copied:
        return ChangeStatus::Copied;
    case DeltaFlag::Ignored:
        return ChangeStatus::Ignored;
    case DeltaFlag::Untracked:
        return ChangeStatus::Untracked;
    case DeltaFlag::Typechange:
        return ChangeStatus::Modified;
    case DeltaFlag::Unreadable:
        return ChangeStatus::Unreadable;
    case DeltaFlag::Conflicted:
        return ChangeStatus::Conflicted;
    }

    return ChangeStatus::Unknown;
}
}
