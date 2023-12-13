/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"
#include "ui_commitdetails.h"

namespace Git
{
class Commit;
class LogsModel;
}

class LIBKOMMITWIDGETS_EXPORT CommitDetails : public QWidget, private Ui::CommitDetails
{
    Q_OBJECT

    Q_PROPERTY(bool enableCommitsLinks READ enableCommitsLinks WRITE setEnableCommitsLinks NOTIFY enableCommitsLinksChanged FINAL)
    Q_PROPERTY(bool enableEmailsLinks READ enableEmailsLinks WRITE setEnableEmailsLinks NOTIFY enableEmailsLinksChanged FINAL)
    Q_PROPERTY(bool enableFilesLinks READ enableFilesLinks WRITE setEnableFilesLinks NOTIFY enableFilesLinksChanged FINAL)

public:
    explicit CommitDetails(QWidget *parent = nullptr);

    Q_REQUIRED_RESULT Git::Commit *commit() const;
    void setCommit(Git::Commit *commit);

    Q_REQUIRED_RESULT bool enableCommitsLinks() const;
    void setEnableCommitsLinks(bool enableCommitsLinks);
    Q_REQUIRED_RESULT bool enableEmailsLinks() const;
    void setEnableEmailsLinks(bool enableEmailsLinks);
    Q_REQUIRED_RESULT bool enableFilesLinks() const;
    void setEnableFilesLinks(bool enableFilesLinks);

Q_SIGNALS:
    void hashClicked(const QString &hash);
    void fileClicked(const QString &file);

    void enableCommitsLinksChanged();
    void enableEmailsLinksChanged();
    void enableFilesLinksChanged();

private:
    void mSlotEmailLinkClicked(const QString &link);

    QString createChangedFiles();
    QString generateCommitLink(const QString &hash);
    QString generateCommitsLink(const QStringList &hashes);

    Git::LogsModel *mLogsModel{nullptr};

    Git::Commit *mCommit{nullptr};
    bool mEnableCommitsLinks{true};
    bool mEnableEmailsLinks{true};
    bool mEnableFilesLinks{true};
};
