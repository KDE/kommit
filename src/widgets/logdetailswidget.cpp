/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "logdetailswidget.h"
#include "GitKlientSettings.h"
#include "git/gitlog.h"
#include "git/gitmanager.h"
#include "git/models/logsmodel.h"
#include "gitklient_appdebug.h"

#include <KLocalizedString>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#include <QCalendar>
#endif

LogDetailsWidget::LogDetailsWidget(QWidget *parent)
    : QTextBrowser(parent)
{
    connect(this, &QTextBrowser::anchorClicked, this, &LogDetailsWidget::self_anchorClicked);
}

Git::Log *LogDetailsWidget::log() const
{
    return mLog;
}

void LogDetailsWidget::setLog(Git::Log *newLog)
{
    if (mLog == newLog)
        return;

    mLog = newLog;
    createText();
}

void LogDetailsWidget::createText()
{
    if (!mLog) {
        clear();
        return;
    }
    auto files = Git::Manager::instance()->changedFiles(mLog->commitHash());
    QStringList filesHtml;

    for (auto i = files.begin(); i != files.end(); ++i) {
        QString color;
        switch (i.value()) {
        case Git::Manager::Modified:
            color = GitKlientSettings::diffModifiedColor().name();
            break;
        case Git::Manager::Added:
            color = GitKlientSettings::diffAddedColor().name();
            break;
        case Git::Manager::Removed:
            color = GitKlientSettings::diffRemovedColor().name();
            break;

        case Git::Manager::Unknown:
        case Git::Manager::Unmodified:
        case Git::Manager::Renamed:
        case Git::Manager::Copied:
        case Git::Manager::UpdatedButInmerged:
        case Git::Manager::Ignored:
        case Git::Manager::Untracked:
            break;
        }
        filesHtml.append(QStringLiteral("<li><font color=%1>%2</a></li>").arg(color, createFileLink(i.key())));
    }
    QStringList parentHashHtml;
    for (const auto &parent : mLog->parents())
        parentHashHtml.append(createHashLink(parent));

    QStringList childsHashHtml;
    for (const auto &child : mLog->childs())
        childsHashHtml.append(createHashLink(child));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QString date;
    qCDebug(GITKLIENT_LOG) << "cal=" << GitKlientSettings::calendarType();
    QCalendar cal(GitKlientSettings::calendarType());
    /*switch (GitKlientSettings::calendarType()) {
    case SettingsHelper::CalendarType::Gregorian:
        cal = QCalendar(QCalendar::System::Gregorian);
        break;
    case SettingsHelper::CalendarType::Jalali:
        cal = QCalendar(QCalendar::System::Julian);
        break;
    case SettingsHelper::CalendarType::Milankovic:
        cal = QCalendar(QCalendar::System::Milankovic);
        break;
    case SettingsHelper::CalendarType::IslamicCivil:
        cal = QCalendar(QCalendar::System::IslamicCivil);
        break;
    }*/
    if (cal.isValid())
        date = mLog->commitDate().toLocalTime().toString("yyyy-MM-dd HH:mm:ss", cal);
    else
        date = mLog->commitDate().toLocalTime().toString();
#else
    auto date = mLog->commitDate().toLocalTime().toString();
#endif

    clear();
    QString html;
    appendHeading(html, mLog->subject());
    if (!mLog->refLog().isEmpty())
        appendParagraph(html, i18n("Ref"), mLog->refLog());
    appendParagraph(html, i18n("Committer"), QStringLiteral("%1 &lt;%2&gt;").arg(mLog->committerName(), mLog->committerEmail()));
    appendParagraph(html, i18n("Author"), QStringLiteral("%1 &lt;%2&gt;").arg(mLog->authorName(), mLog->authorEmail()));
    appendParagraph(html, i18n("Date"), date);
    appendParagraph(html, i18n("Hash"), mLog->commitHash());

    if (!mLog->parents().empty())
        appendParagraph(html, mLog->parents().size() == 1 ? i18n("Parent") : i18n("Parents"), parentHashHtml.join(", "));
    if (!mLog->childs().empty())
        appendParagraph(html, mLog->childs().size() == 1 ? i18n("Child") : i18n("Children"), childsHashHtml.join(", "));

    appendParagraph(html, i18n("Changed files"), filesHtml);

    setHtml(html);
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
    auto log = Git::Manager::instance()->logsModel()->findLogByHash(hash);
    if (!log)
        return {};

    if (m_enableCommitsLinks)
        return QStringLiteral(R"(<a href="hash:%1">%2</a> )").arg(log->commitHash(), log->subject());

    return log->subject();
}

QString LogDetailsWidget::createFileLink(const QString &file)
{
    return QStringLiteral(R"(<a href="file:%1">%1</a> )").arg(file);
}

void LogDetailsWidget::self_anchorClicked(const QUrl &url)
{
    const auto scheme = url.scheme().toLower();

    if (scheme == QStringLiteral("hash"))
        Q_EMIT hashClicked(url.path());
    else if (scheme == QStringLiteral("file"))
        Q_EMIT fileClicked(url.path());
}

bool LogDetailsWidget::enableCommitsLinks() const
{
    return m_enableCommitsLinks;
}

void LogDetailsWidget::setEnableCommitsLinks(bool newEnableCommitsLinks)
{
    if (m_enableCommitsLinks == newEnableCommitsLinks)
        return;
    m_enableCommitsLinks = newEnableCommitsLinks;
    Q_EMIT enableCommitsLinksChanged();
}
