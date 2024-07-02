/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitdetails.h"
#include "avatarview.h"
#include "kommitwidgetsglobaloptions.h"

#include <entities/commit.h>
#include <gitmanager.h>
#include <models/logsmodel.h>

#include <KLocalizedString>
#include <QDesktopServices>
#include <QLocale>
#include <QUrl>

namespace
{

void showSignature(QSharedPointer<Git::Signature> sign, AvatarView *avatarView, QLabel *nameLabel, QLabel *timeLabel, bool createLink)
{
    if (!sign)
        return;

    avatarView->setUserEmail(sign->email());

    QString label;
    if (createLink)
        label = QStringLiteral("<a href=\"mailto:%2\">%1 &lt;%2&gt;</a>");
    else
        label = QStringLiteral("%1 <%2>");

    nameLabel->setText(label.arg(sign->name(), sign->email()));

    auto cal = KommitWidgetsGlobalOptions::instance()->calendar();
    if (cal.isValid())
        timeLabel->setText(QLocale().toString(sign->time(), QStringLiteral("yyyy-MM-dd HH:mm:ss"), cal));
    else
        timeLabel->setText(QLocale().toString(sign->time()));
}
}

CommitDetails::CommitDetails(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(labelChangedFiles, &QLabel::linkActivated, this, &CommitDetails::fileClicked);
    connect(labelParent, &QLabel::linkActivated, this, &CommitDetails::hashClicked);
    connect(labelChildren, &QLabel::linkActivated, this, &CommitDetails::hashClicked);

    connect(labelAuthor, &QLabel::linkActivated, this, &CommitDetails::slotEmailLinkClicked);
    connect(labelCommitter, &QLabel::linkActivated, this, &CommitDetails::slotEmailLinkClicked);

    stackedWidget->setCurrentIndex(0);
}

Git::Commit *CommitDetails::commit() const
{
    return mCommit;
}

void CommitDetails::setCommit(Git::Commit *commit)
{
    mCommit = commit;

    stackedWidget->setCurrentIndex(commit ? 1 : 0);
    if (!commit)
        return;

    labelCommitHash->setText(commit->commitHash());
    labelCommitSubject->setText(commit->message());

    showSignature(commit->author(), labelAuthorAvatar, labelAuthor, labelAuthTime, mEnableEmailsLinks);
    showSignature(commit->committer(), labelCommiterAvatar, labelCommitter, labelCommitTime, mEnableEmailsLinks);

    widgetCommitterInfo->setVisible(commit && commit->author()->email() != commit->committer()->email());
    labelCommitterText->setVisible(widgetCommitterInfo->isVisible());

    labelChangedFiles->setText(createChangedFiles());

    auto ref = commit->reference();
    if (!ref.isNull()) {
        if (ref->isBranch())
            labelRefType->setText(i18n("Branch: "));
        else if (ref->isNote())
            labelRefType->setText(i18n("Note: "));
        else if (ref->isRemote())
            labelRefType->setText(i18n("Remote: "));
        else if (ref->isTag())
            labelRefType->setText(i18n("Tag: "));

        labelRefName->setText(ref->shorthand());

        labelRefType->setVisible(true);
        labelRefName->setVisible(true);
    } else {
        labelRefType->setVisible(false);
        labelRefName->setVisible(false);
    }

    auto parents = generateCommitsLink(commit->parents());
    auto children = generateCommitsLink(commit->children());

    labelParentsText->setVisible(!parents.isEmpty());
    labelParent->setVisible(!parents.isEmpty());
    labelParentsText->setText(i18np("Parent:", "Parents:", commit->parents().size()));
    labelParent->setText(parents);

    labelChildrenText->setVisible(!children.isEmpty());
    labelChildren->setVisible(!children.isEmpty());
    labelChildrenText->setText(i18np("Child:", "Children:", commit->children().size()));
    labelChildren->setText(children);
}

bool CommitDetails::enableCommitsLinks() const
{
    return mEnableCommitsLinks;
}

void CommitDetails::setEnableCommitsLinks(bool enableCommitsLinks)
{
    if (mEnableCommitsLinks == enableCommitsLinks)
        return;
    mEnableCommitsLinks = enableCommitsLinks;
    Q_EMIT enableCommitsLinksChanged();
}

bool CommitDetails::enableEmailsLinks() const
{
    return mEnableEmailsLinks;
}

void CommitDetails::setEnableEmailsLinks(bool enableEmailsLinks)
{
    if (mEnableEmailsLinks == enableEmailsLinks)
        return;
    mEnableEmailsLinks = enableEmailsLinks;
    Q_EMIT enableEmailsLinksChanged();
}

bool CommitDetails::enableFilesLinks() const
{
    return mEnableFilesLinks;
}

void CommitDetails::setEnableFilesLinks(bool enableFilesLinks)
{
    if (mEnableFilesLinks == enableFilesLinks)
        return;
    mEnableFilesLinks = enableFilesLinks;
    Q_EMIT enableFilesLinksChanged();
}

void CommitDetails::slotEmailLinkClicked(const QString &link)
{
    QDesktopServices::openUrl(QUrl{link});
}

QString CommitDetails::createChangedFiles()
{
    const auto files = Git::Manager::instance()->changedFiles(mCommit->commitHash());
    QStringList filesHtml;

    for (auto i = files.constBegin(); i != files.constEnd(); ++i) {
        QString color;
        // switch (i.value()) {
        // case Git::ChangeStatus::Modified:
        //     color = mChangedFilesColor.name();
        //     break;
        // case Git::ChangeStatus::Added:
        //     color = mAddedFilesColor.name();
        //     break;
        // case Git::ChangeStatus::Removed:
        //     color = mRemovedFilesColor.name();
        //     break;

        // default:
        //     break;
        // }
        if (mEnableFilesLinks)
            filesHtml.append(QStringLiteral("<font color=%1><a href=\"%2\">%2</a></a>").arg(color, i.key()));
        else
            filesHtml.append(QStringLiteral("<font color=%1>%2</a>").arg(color, i.key()));
    }

    return filesHtml.join(QStringLiteral("<br />"));
}

QString CommitDetails::generateCommitLink(const QString &hash)
{
    QString subject = hash;
    if (mLogsModel) {
        auto commit = mLogsModel->findLogByHash(hash);
        if (commit)
            subject = commit->message();
    }
    if (mEnableCommitsLinks)
        return QStringLiteral(R"(<a href="%1">%2</a> )").arg(hash, subject);

    return subject;
}

QString CommitDetails::generateCommitsLink(const QStringList &hashes)
{
    QStringList ret;
    ret.reserve(hashes.count());
    for (auto const &hash : hashes)
        ret << generateCommitLink(hash);

    return ret.join(QStringLiteral(", "));
}

#include "moc_commitdetails.cpp"
