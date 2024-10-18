/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filemode.h"

namespace Git
{

FileMode::FileMode()
    : mPermissions{Permission::Invalid}
{
}

FileMode::FileMode(Permissions permissions)
    : mPermissions(permissions)
{
}

FileMode::FileMode(quint16 mode)
    : mPermissions{mode}
{
}

FileMode &FileMode::operator=(quint16 mode)
{
    mPermissions = static_cast<Permissions>(mode);
    return *this;
}

void FileMode::setOwnerRead(bool enable)
{
    setPermission(Permission::OwnerRead, enable);
}

void FileMode::setOwnerWrite(bool enable)
{
    setPermission(Permission::OwnerWrite, enable);
}

void FileMode::setOwnerExecute(bool enable)
{
    setPermission(Permission::OwnerExecute, enable);
}

void FileMode::setGroupRead(bool enable)
{
    setPermission(Permission::GroupRead, enable);
}

void FileMode::setGroupWrite(bool enable)
{
    setPermission(Permission::GroupWrite, enable);
}

void FileMode::setGroupExecute(bool enable)
{
    setPermission(Permission::GroupExecute, enable);
}

void FileMode::setOthersRead(bool enable)
{
    setPermission(Permission::OthersRead, enable);
}

void FileMode::setOthersWrite(bool enable)
{
    setPermission(Permission::OthersWrite, enable);
}

void FileMode::setOthersExecute(bool enable)
{
    setPermission(Permission::OthersExecute, enable);
}

bool FileMode::ownerRead() const
{
    return hasPermission(Permission::OwnerRead);
}

bool FileMode::ownerWrite() const
{
    return hasPermission(Permission::OwnerWrite);
}

bool FileMode::ownerExecute() const
{
    return hasPermission(Permission::OwnerExecute);
}

bool FileMode::groupRead() const
{
    return hasPermission(Permission::GroupRead);
}

bool FileMode::groupWrite() const
{
    return hasPermission(Permission::GroupWrite);
}

bool FileMode::groupExecute() const
{
    return hasPermission(Permission::GroupExecute);
}

bool FileMode::othersRead() const
{
    return hasPermission(Permission::OthersRead);
}

bool FileMode::othersWrite() const
{
    return hasPermission(Permission::OthersWrite);
}

bool FileMode::othersExecute() const
{
    return hasPermission(Permission::OthersExecute);
}

QString FileMode::toString() const
{
    if (mPermissions == Permission::Invalid) {
        return QStringLiteral("Invalid");
    }

    QString modeString;
    modeString.reserve(9);

    modeString += (ownerRead() ? QStringLiteral("r") : QStringLiteral("-"));
    modeString += (ownerWrite() ? QStringLiteral("w") : QStringLiteral("-"));
    modeString += (ownerExecute() ? QStringLiteral("x") : QStringLiteral("-"));
    modeString += (groupRead() ? QStringLiteral("r") : QStringLiteral("-"));
    modeString += (groupWrite() ? QStringLiteral("w") : QStringLiteral("-"));
    modeString += (groupExecute() ? QStringLiteral("x") : QStringLiteral("-"));
    modeString += (othersRead() ? QStringLiteral("r") : QStringLiteral("-"));
    modeString += (othersWrite() ? QStringLiteral("w") : QStringLiteral("-"));
    modeString += (othersExecute() ? QStringLiteral("x") : QStringLiteral("-"));

    return modeString;
}

quint16 FileMode::value() const
{
    return static_cast<quint16>(mPermissions);
}

bool FileMode::isValid() const
{
    return mPermissions != Permission::Invalid;
}

void FileMode::setPermission(Permission perm, bool enable)
{
    if (enable) {
        mPermissions |= perm;
    } else {
        mPermissions &= ~perm;
    }
}

bool FileMode::hasPermission(Permission perm) const
{
    return mPermissions & perm;
}

}
