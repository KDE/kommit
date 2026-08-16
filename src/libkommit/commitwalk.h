/*
SPDX-FileCopyrightText: 2026 Méven Car <meven@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QList>
#include <QString>

#include <git2/oid.h>

namespace Git
{

/// The object ids a revision walk produced, newest first, and whether it stopped early.
struct LIBKOMMIT_EXPORT CommitWalk {
    QList<git_oid> oids;
    bool hasMore{false};
};

/**
 * The newest @p maxCount commits of the repository at @p path, or all of them when
 * @p maxCount is not positive. An empty @p branchRefName walks every branch.
 *
 * This opens a repository handle of its own and hands back plain object ids, touching no
 * shared state, so it is safe to call on a worker thread while another thread goes on using
 * its own Repository. A sorted walk loads and orders the whole reachable graph before it
 * yields the first commit, which on a long history takes seconds; that is the work this
 * exists to move off the thread drawing the window.
 */
[[nodiscard]] LIBKOMMIT_EXPORT CommitWalk walkCommits(const QString &path, const QString &branchRefName, int maxCount);

}
