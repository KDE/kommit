/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"
#include "ui_commitdetails.h"

#include <Kommit/Commit>

class CommitsModel;

class LIBKOMMITWIDGETS_EXPORT CommitDetails : public QWidget, private Ui::CommitDetails
{
    Q_OBJECT

    Q_PROPERTY(bool enableCommitsLinks READ enableCommitsLinks WRITE setEnableCommitsLinks NOTIFY enableCommitsLinksChanged FINAL)
    Q_PROPERTY(bool enableEmailsLinks READ enableEmailsLinks WRITE setEnableEmailsLinks NOTIFY enableEmailsLinksChanged FINAL)
    Q_PROPERTY(bool enableFilesLinks READ enableFilesLinks WRITE setEnableFilesLinks NOTIFY enableFilesLinksChanged FINAL)

public:
    explicit CommitDetails(QWidget *parent = nullptr);

    [[nodiscard]] const Git::Commit &commit() const;
    void setCommit(const Git::Commit &commit);

    [[nodiscard]] bool enableCommitsLinks() const;
    void setEnableCommitsLinks(bool enableCommitsLinks);
    [[nodiscard]] bool enableEmailsLinks() const;
    void setEnableEmailsLinks(bool enableEmailsLinks);
    [[nodiscard]] bool enableFilesLinks() const;
    void setEnableFilesLinks(bool enableFilesLinks);

    Git::Repository *repo() const;
    void setRepo(Git::Repository *newRepo);

Q_SIGNALS:
    void hashClicked(const QString &hash);
    void fileClicked(const QString &file);

    void enableCommitsLinksChanged();
    void enableEmailsLinksChanged();
    void enableFilesLinksChanged();

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotEmailLinkClicked(const QString &link);
    LIBKOMMITWIDGETS_NO_EXPORT void slotMarkdownDisplayToggled(bool checked);

    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT QString createChangedFiles();
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT QString generateCommitLink(const QString &hash);
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT QString generateCommitsLink(const QStringList &hashes);

    CommitsModel *mLogsModel{nullptr};

    Git::Commit mCommit;
    Git::Repository *mRepo;
    bool mEnableCommitsLinks{true};
    bool mEnableEmailsLinks{true};
    bool mEnableFilesLinks{true};
};
