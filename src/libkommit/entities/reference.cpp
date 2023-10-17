/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "reference.h"
#include <git2/refs.h>

namespace Git
{

Reference::Reference()
{
}

Reference::Reference(git_reference *ref)
    : ptr{ref}
{
    mIsNote = git_reference_is_note(ref);
    mIsBranch = git_reference_is_branch(ref);
    mIsRemote = git_reference_is_remote(ref);
    mIsTag = git_reference_is_tag(ref);
    mName = git_reference_name(ref);
    mShorthand = git_reference_shorthand(ref);
}

Reference::~Reference()
{
    git_reference_free(ptr);
    ptr = nullptr;
}

bool Reference::isNote() const
{
    return mIsNote;
}

bool Reference::isBranch() const
{
    return mIsBranch;
}

bool Reference::isTag() const
{
    return mIsTag;
}

bool Reference::isRemote() const
{
    return mIsRemote;
}

QString Reference::name() const
{
    return mName;
}

QString Reference::shorthand() const
{
    return mShorthand;
}

}
