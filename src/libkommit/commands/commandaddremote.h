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

    Q_REQUIRED_RESULT QStringList generateArgs() const override;

    Q_REQUIRED_RESULT const QString &remoteName() const;
    void setRemoteName(const QString &newRemoteName);
    Q_REQUIRED_RESULT OptionalBool tags() const;
    void setTags(OptionalBool newTags);
    void setTags(Qt::CheckState newTags);
    Q_REQUIRED_RESULT bool mirror() const;
    void setMirror(bool newMirror);
    Q_REQUIRED_RESULT const QString &master() const;
    void setMaster(const QString &newMaster);
    Q_REQUIRED_RESULT bool fetch() const;
    void setFetch(bool newFetch);

    Q_REQUIRED_RESULT const QString &url() const;
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
