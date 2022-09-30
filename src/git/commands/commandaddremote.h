/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libgitklient_export.h"
#include "abstractcommand.h"

namespace Git
{

class LIBGITKLIENT_EXPORT CommandAddRemote : public AbstractCommand
{
public:
    explicit CommandAddRemote(QObject *parent = nullptr);

    QStringList generateArgs() const override;

    const QString &remoteName() const;
    void setRemoteName(const QString &newRemoteName);
    OptionalBool tags() const;
    void setTags(OptionalBool newTags);
    void setTags(Qt::CheckState newTags);
    bool mirror() const;
    void setMirror(bool newMirror);
    const QString &master() const;
    void setMaster(const QString &newMaster);
    bool fetch() const;
    void setFetch(bool newFetch);

    const QString &url() const;
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
