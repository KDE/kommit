/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QTextBrowser>

namespace Git
{
class Log;
}

class LogDetailsWidget : public QTextBrowser
{
    Q_OBJECT
    Q_PROPERTY(bool enableCommitsLinks READ enableCommitsLinks WRITE setEnableCommitsLinks NOTIFY enableCommitsLinksChanged)
    Q_PROPERTY(bool enableEmailsLinks READ enableEmailsLinks WRITE setEnableEmailsLinks NOTIFY enableEmailsLinksChanged)
    Q_PROPERTY(bool enableFilesLinks READ enableFilesLinks WRITE setEnableFilesLinks NOTIFY enableFilesLinksChanged)

public:
    explicit LogDetailsWidget(QWidget *parent = nullptr);
    Git::Log *log() const;
    void setLog(Git::Log *newLog);

    Q_REQUIRED_RESULT bool enableCommitsLinks() const;
    void setEnableCommitsLinks(bool newEnableCommitsLinks);

    Q_REQUIRED_RESULT bool enableEmailsLinks() const;
    void setEnableEmailsLinks(bool newEnableEmailsLinks);

    Q_REQUIRED_RESULT bool enableFilesLinks() const;
    void setEnableFilesLinks(bool newEnableFilesLinks);

Q_SIGNALS:
    void hashClicked(const QString &hash);
    void fileClicked(const QString &file);
    void enableCommitsLinksChanged();
    void enableEmailsLinksChanged();
    void enableFilesLinksChanged();

private:
    void self_anchorClicked(const QUrl &url);
    void createText();
    static void appendHeading(QString &html, const QString &title, short level = 2);
    static void appendParagraph(QString &html, const QString &text);
    static void appendParagraph(QString &html, const QString &name, const QString &value);
    static void appendParagraph(QString &html, const QString &name, const QStringList &list);
    QString createHashLink(const QString &hash) const;
    static QString createFileLink(const QString &hash);

    Git::Log *mLog = nullptr;

    bool mEnableCommitsLinks{false};
    bool mEnableEmailsLinks{true};
    bool mEnableFilesLinks{true};
};
