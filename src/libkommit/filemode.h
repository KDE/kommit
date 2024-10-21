/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT FileMode
{
public:
    enum Permission : quint16 {
        Invalid = 0, // Invalid or unset mode
        OwnerRead = 1 << 8, // 0400
        OwnerWrite = 1 << 7, // 0200
        OwnerExecute = 1 << 6, // 0100
        GroupRead = 1 << 5, // 0040
        GroupWrite = 1 << 4, // 0020
        GroupExecute = 1 << 3, // 0010
        OthersRead = 1 << 2, // 0004
        OthersWrite = 1 << 1, // 0002
        OthersExecute = 1 << 0 // 0001
    };
    Q_DECLARE_FLAGS(Permissions, Permission)

    // enum class Permission2 : quint16 {
    //     Execute =  0,
    //     Write =  1,
    //     Read =  2,
    // };

    // enum class Cat : quint16 { Others = 0, Group = 1, Owner = 2 };

    // Constructor with optional default permissions
    FileMode();
    explicit FileMode(Permissions permissions);
    explicit FileMode(quint16 mode);

    FileMode &operator=(quint16 mode);
    // Setters for permissions
    void setOwnerRead(bool enable);
    void setOwnerWrite(bool enable);
    void setOwnerExecute(bool enable);

    void setGroupRead(bool enable);
    void setGroupWrite(bool enable);
    void setGroupExecute(bool enable);

    void setOthersRead(bool enable);
    void setOthersWrite(bool enable);
    void setOthersExecute(bool enable);

    // Getters for permissions
    [[nodiscard]] bool ownerRead() const;
    [[nodiscard]] bool ownerWrite() const;
    [[nodiscard]] bool ownerExecute() const;

    [[nodiscard]] bool groupRead() const;
    [[nodiscard]] bool groupWrite() const;
    [[nodiscard]] bool groupExecute() const;

    [[nodiscard]] bool othersRead() const;
    [[nodiscard]] bool othersWrite() const;
    [[nodiscard]] bool othersExecute() const;

    [[nodiscard]] QString toString() const;

    [[nodiscard]] quint16 value() const;

private:
    Permissions mPermissions;

    LIBKOMMIT_NO_EXPORT void setPermission(Permission perm, bool enable);
    [[nodiscard]] LIBKOMMIT_NO_EXPORT bool hasPermission(Permission perm) const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FileMode::Permissions)

}
