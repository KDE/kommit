/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QCalendar>
#include <QTextBrowser>

#include "libkommitwidgets_export.h"

namespace Git
{
class Commit;
class Signature;
}

class LIBKOMMITWIDGETS_EXPORT LogDetailsWidget : public QTextBrowser
{
    Q_OBJECT
    Q_PROPERTY(bool enableCommitsLinks READ enableCommitsLinks WRITE setEnableCommitsLinks NOTIFY enableCommitsLinksChanged)
    Q_PROPERTY(bool enableEmailsLinks READ enableEmailsLinks WRITE setEnableEmailsLinks NOTIFY enableEmailsLinksChanged)
    Q_PROPERTY(bool enableFilesLinks READ enableFilesLinks WRITE setEnableFilesLinks NOTIFY enableFilesLinksChanged)

    Q_PROPERTY(QColor addedFilesColor READ addedFilesColor WRITE setAddedFilesColor NOTIFY addedFilesColorChanged FINAL)
    Q_PROPERTY(QColor changedFilesColor READ changedFilesColor WRITE setChangedFilesColor NOTIFY changedFilesColorChanged FINAL)
    Q_PROPERTY(QColor removedFilesColor READ removedFilesColor WRITE setRemovedFilesColor NOTIFY removedFilesColorChanged FINAL)

public:
    explicit LogDetailsWidget(QWidget *parent = nullptr);
    Git::Commit *log() const;
    void setLog(Git::Commit *newLog);

    Q_REQUIRED_RESULT bool enableCommitsLinks() const;
    void setEnableCommitsLinks(bool newEnableCommitsLinks);

    Q_REQUIRED_RESULT bool enableEmailsLinks() const;
    void setEnableEmailsLinks(bool newEnableEmailsLinks);

    Q_REQUIRED_RESULT bool enableFilesLinks() const;
    void setEnableFilesLinks(bool newEnableFilesLinks);

    Q_REQUIRED_RESULT QColor addedFilesColor() const;
    void setAddedFilesColor(const QColor &addedFilesColor);

    Q_REQUIRED_RESULT QColor changedFilesColor() const;
    void setChangedFilesColor(const QColor &changedFilesColor);

    Q_REQUIRED_RESULT QColor removedFilesColor() const;
    void setRemovedFilesColor(const QColor &removedFilesColor);

    Q_REQUIRED_RESULT QCalendar calendar() const;
    void setCalendar(const QCalendar &calendar);

Q_SIGNALS:
    void hashClicked(const QString &hash);
    void fileClicked(const QString &file);
    void enableCommitsLinksChanged();
    void enableEmailsLinksChanged();
    void enableFilesLinksChanged();

    void addedFilesColorChanged();
    void changedFilesColorChanged();
    void removedFilesColorChanged();

private:
    void self_anchorClicked(const QUrl &url);
    void createText();
    void makeOutput();

    void appendRow(QTextTable *table, const QString &caption, Git::Signature *signature) const;
    static void appendHeading(QString &html, const QString &title, short level = 2);
    static void appendParagraph(QString &html, const QString &text);
    static void appendParagraph(QString &html, const QString &name, const QString &value);
    static void appendParagraph(QString &html, const QString &name, const QStringList &list);
    QString createHashLink(const QString &hash) const;
    static QString createFileLink(const QString &hash);

    Git::Commit *mLog = nullptr;

    bool mEnableCommitsLinks{false};
    bool mEnableEmailsLinks{true};
    bool mEnableFilesLinks{true};
    QColor mAddedFilesColor;
    QColor mChangedFilesColor;
    QColor mRemovedFilesColor;
    QCalendar mCalendar;
};
