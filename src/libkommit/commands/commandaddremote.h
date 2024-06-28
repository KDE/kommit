/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractcommand.h"
#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT CommandAddRemote : public AbstractCommand
{
public:
    explicit CommandAddRemote(QObject *parent = nullptr);

    [[nodiscard]] QStringList generateArgs() const override;

    [[nodiscard]] const QString &remoteName() const;
    void setRemoteName(const QString &newRemoteName);
    [[nodiscard]] OptionalBool tags() const;
    void setTags(OptionalBool newTags);
    void setTags(Qt::CheckState newTags);
    [[nodiscard]] bool mirror() const;
    void setMirror(bool newMirror);
    [[nodiscard]] const QString &master() const;
    void setMaster(const QString &newMaster);
    [[nodiscard]] bool fetch() const;
    void setFetch(bool newFetch);

    [[nodiscard]] const QString &url() const;
    void setUrl(const QString &newUrl);

private:
    QString mRemoteName;
    OptionalBool mTags{OptionalBool::False};
    bool mMirror{false};
    QString mMaster;
    bool mFetch{false};
    QString mUrl;
};

}
