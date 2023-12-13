/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "logdetailswidget.h"

#include <entities/commit.h>
#include <gitmanager.h>
#include <models/logsmodel.h>

#include <KLocalizedString>

#include <QCalendar>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QTextTable>

LogDetailsWidget::LogDetailsWidget(QWidget *parent)
    : QTextBrowser(parent)
{
    connect(this, &QTextBrowser::anchorClicked, this, &LogDetailsWidget::self_anchorClicked);
}

Git::Commit *LogDetailsWidget::log() const
{
    return mLog;
}

void LogDetailsWidget::setLog(Git::Commit *newLog)
{
    if (mLog == newLog)
        return;

    mLog = newLog;
    createText();
    // makeOutput();
}

void LogDetailsWidget::createText()
{
    if (!mLog) {
        clear();
        return;
    }
    // TODO: remove this singelton call
    const auto files = Git::Manager::instance()->changedFiles(mLog->commitHash());
    QStringList filesHtml;

    for (auto i = files.constBegin(); i != files.constEnd(); ++i) {
        QString color;
        switch (i.value()) {
        case Git::ChangeStatus::Modified:
            color = mChangedFilesColor.name();
            break;
        case Git::ChangeStatus::Added:
            color = mAddedFilesColor.name();
            break;
        case Git::ChangeStatus::Removed:
            color = mRemovedFilesColor.name();
            break;

        default:
            break;
        }
        if (mEnableFilesLinks)
            filesHtml.append(QStringLiteral("<li><font color=%1>%2</a></li>").arg(color, createFileLink(i.key())));
        else
            filesHtml.append(QStringLiteral("<li><font color=%1>%2</a></li>").arg(color, i.key()));
    }
    QStringList parentHashHtml;
    for (const auto &parent : mLog->parents())
        parentHashHtml.append(createHashLink(parent));

    QStringList childsHashHtml;
    for (const auto &child : mLog->children())
        childsHashHtml.append(createHashLink(child));

    QString date;
    QString commitDate;
    QString authDate;

    if (mCalendar.isValid()) {
        date = mLog->commitTime().toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"), mCalendar);
        commitDate = mLog->committer()->time().toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"), mCalendar);
        authDate = mLog->author()->time().toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"), mCalendar);
    } else {
        date = mLog->commitTime().toLocalTime().toString();
        commitDate = mLog->committer()->time().toLocalTime().toString();
        authDate = mLog->author()->time().toLocalTime().toString();
    }

    clear();
    QString html;
    appendHeading(html, mLog->subject());
    auto ref = mLog->reference();
    if (!ref.isNull()) {
        QString refName;
        if (ref->isBranch())
            refName = i18n("Branch: ");
        else if (ref->isNote())
            refName = i18n("Note: ");
        else if (ref->isRemote())
            refName = i18n("Remote: ");
        else if (ref->isTag())
            refName = i18n("Tag: ");
        refName.append(ref->shorthand());
        appendParagraph(html, i18n("Ref"), refName);
    }

    if (mEnableEmailsLinks) {
        appendParagraph(html,
                        i18n("Committer"),
                        QStringLiteral(R"(<a href="mailto:%2">%1 &lt;%2&gt;</a>)").arg(mLog->committer()->name(), mLog->committer()->email()));

        auto emailHash = QCryptographicHash::hash(mLog->author()->email().toUtf8(), QCryptographicHash::Sha3_256).toHex().toLower();
        auto avatarUrl = "https://www.gravatar.com/avatar/" + emailHash;
        html.append(QString{"<img src=\"%1\" />"}.arg(QString{avatarUrl}));
        appendParagraph(html, i18n("Author"), QStringLiteral(R"(<a href="mailto:%2">%1 &lt;%2&gt;</a>)").arg(mLog->author()->name(), mLog->author()->email()));
    } else {
        appendParagraph(html, i18n("Committer"), QStringLiteral(R"(%1 &lt;%2&gt;)").arg(mLog->committer()->name(), mLog->committer()->email()));
        appendParagraph(html, i18n("Author"), QStringLiteral(R"(%1 &lt;%2&gt;)").arg(mLog->author()->name(), mLog->author()->email()));
    }
    appendParagraph(html, i18n("time"), date);
    appendParagraph(html, i18n("Commit time"), commitDate);
    appendParagraph(html, i18n("Auth time"), authDate);
    appendParagraph(html, i18n("Hash"), mLog->commitHash());

    if (!mLog->parents().empty())
        appendParagraph(html, mLog->parents().size() == 1 ? i18n("Parent") : i18n("Parents"), parentHashHtml.join(QStringLiteral(", ")));
    if (!mLog->children().empty())
        appendParagraph(html, mLog->children().size() == 1 ? i18n("Child") : i18n("Children"), childsHashHtml.join(QStringLiteral(", ")));

    appendParagraph(html, i18n("Changed files"), filesHtml);

    setHtml(html);
}

void LogDetailsWidget::makeOutput()
{
    clear();
    if (!mLog) {
        return;
    }

    auto c = textCursor();

    QTextCharFormat titleFormat;
    auto f = titleFormat.font();
    f.setBold(true);
    f.setWeight(400);
    titleFormat.setFont(f);
    c.insertText(mLog->subject(), titleFormat);

    QTextTableFormat format;
    format.setBorder(0);
    format.setBorderStyle(QTextTableFormat::BorderStyle_None);
    auto table = c.insertTable(0, 2, format);
    appendRow(table, "Auth", mLog->author().data());
    appendRow(table, "Committer", mLog->committer().data());
}

void LogDetailsWidget::appendRow(QTextTable *table, const QString &caption, Git::Signature *signature) const
{
    table->appendRows(1);
    auto rowIndex = table->rows() - 1;
    table->cellAt(rowIndex, 0).lastCursorPosition().insertText(caption);
    auto f = table->cellAt(rowIndex, 0).format();
    auto font = f.font();
    font.setBold(true);
    f.setFont(font);
    table->cellAt(rowIndex, 0).setFormat(f);

    auto s = QStringLiteral(R"(%1 <%2>)").arg(signature->name(), signature->email());
    table->cellAt(rowIndex, 1).lastCursorPosition().insertText(s);
}

void LogDetailsWidget::appendHeading(QString &html, const QString &title, const short level)
{
    html.append(QStringLiteral("<h%2>%1</h%2>").arg(title).arg(level));
}

void LogDetailsWidget::appendParagraph(QString &html, const QString &text)
{
    html.append(QStringLiteral("<p>%1</p>").arg(text));
}

void LogDetailsWidget::appendParagraph(QString &html, const QString &name, const QString &value)
{
    html.append(QStringLiteral("<p><b>%1:</b> %2</p>").arg(name, value));
}

void LogDetailsWidget::appendParagraph(QString &html, const QString &name, const QStringList &list)
{
    if (list.isEmpty())
        return;

    html.append(QStringLiteral("<p><b>%1</b><ul>").arg(name));
    for (const auto &l : list)
        html.append(QStringLiteral("<li>%1</li>").arg(l));
    html.append(QStringLiteral("</ul>"));
}

QString LogDetailsWidget::createHashLink(const QString &hash) const
{
    // TODO: remove also this one
    auto log = Git::Manager::instance()->logsModel()->findLogByHash(hash);
    if (!log)
        return {};

    if (mEnableCommitsLinks)
        return QStringLiteral(R"(<a href="hash:%1">%2</a> )").arg(log->commitHash(), log->subject());

    return log->subject();
}

QString LogDetailsWidget::createFileLink(const QString &file)
{
    return QStringLiteral(R"(<a href="file:%1">%1</a> )").arg(file);
}

QCalendar LogDetailsWidget::calendar() const
{
    return mCalendar;
}

void LogDetailsWidget::setCalendar(const QCalendar &calendar)
{
    mCalendar = calendar;
}

void LogDetailsWidget::self_anchorClicked(const QUrl &url)
{
    const auto scheme = url.scheme().toLower();

    if (scheme == QStringLiteral("hash"))
        Q_EMIT hashClicked(url.path());
    else if (scheme == QStringLiteral("file"))
        Q_EMIT fileClicked(url.path());
    else if (scheme == QStringLiteral("mailto"))
        QDesktopServices::openUrl(url);
}

bool LogDetailsWidget::enableCommitsLinks() const
{
    return mEnableCommitsLinks;
}

void LogDetailsWidget::setEnableCommitsLinks(bool newEnableCommitsLinks)
{
    if (mEnableCommitsLinks == newEnableCommitsLinks)
        return;
    mEnableCommitsLinks = newEnableCommitsLinks;
    Q_EMIT enableCommitsLinksChanged();
}

bool LogDetailsWidget::enableEmailsLinks() const
{
    return mEnableEmailsLinks;
}

void LogDetailsWidget::setEnableEmailsLinks(bool newEnableEmailsLinks)
{
    if (mEnableEmailsLinks == newEnableEmailsLinks)
        return;
    mEnableEmailsLinks = newEnableEmailsLinks;
    Q_EMIT enableEmailsLinksChanged();
}

bool LogDetailsWidget::enableFilesLinks() const
{
    return mEnableFilesLinks;
}

void LogDetailsWidget::setEnableFilesLinks(bool newEnableFilesLinks)
{
    if (mEnableFilesLinks == newEnableFilesLinks)
        return;
    mEnableFilesLinks = newEnableFilesLinks;
    Q_EMIT enableFilesLinksChanged();
}

QColor LogDetailsWidget::addedFilesColor() const
{
    return mAddedFilesColor;
}

void LogDetailsWidget::setAddedFilesColor(const QColor &addedFilesColor)
{
    if (mAddedFilesColor == addedFilesColor)
        return;
    mAddedFilesColor = addedFilesColor;
    emit addedFilesColorChanged();
}

QColor LogDetailsWidget::changedFilesColor() const
{
    return mChangedFilesColor;
}

void LogDetailsWidget::setChangedFilesColor(const QColor &changedFilesColor)
{
    if (mChangedFilesColor == changedFilesColor)
        return;
    mChangedFilesColor = changedFilesColor;
    emit changedFilesColorChanged();
}

QColor LogDetailsWidget::removedFilesColor() const
{
    return mRemovedFilesColor;
}

void LogDetailsWidget::setRemovedFilesColor(const QColor &removedFilesColor)
{
    if (mRemovedFilesColor == removedFilesColor)
        return;
    mRemovedFilesColor = removedFilesColor;
    emit removedFilesColorChanged();
}

#include "moc_logdetailswidget.cpp"
