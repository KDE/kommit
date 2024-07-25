#include "fetchresultwidget.h"
#include "dialogs/credentialdialog.h"

#include <credential.h>
#include <entities/oid.h>
#include <entities/reference.h>
#include <observers/fetchobserver.h>

FetchResultWidget::FetchResultWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

Git::FetchObserver *FetchResultWidget::observer() const
{
    return mObserver;
}

void FetchResultWidget::setObserver(Git::FetchObserver *newObserver)
{
    if (mObserver)
        mObserver->disconnect(this);

    mObserver = newObserver;

    connect(newObserver, &Git::FetchObserver::message, this, &FetchResultWidget::slotMessage);
    connect(newObserver, &Git::FetchObserver::credentialRequeted, this, &FetchResultWidget::slotCredentialRequeted);
    connect(newObserver, &Git::FetchObserver::transferProgress, this, &FetchResultWidget::slotTransferProgress);
    connect(newObserver, &Git::FetchObserver::updateRef, this, &FetchResultWidget::slotUpdateRef);
    connect(newObserver, &Git::FetchObserver::finished, this, &FetchResultWidget::slotFinished);
    connect(newObserver, &Git::FetchObserver::packProgress, this, &FetchResultWidget::slotPackProgress);
}

void FetchResultWidget::slotMessage(const QString &message)
{
    textBrowser->append(message);
}

void FetchResultWidget::slotCredentialRequeted(const QString &url, Git::Credential *cred)
{
    CredentialDialog d{this};
    d.setUrl(url);
    d.setUsername(cred->username());

    if (d.exec() == QDialog::DialogCode::Accepted) {
        cred->setUsername(d.username());
        cred->setPassword(d.password());
    }
}

void FetchResultWidget::slotTransferProgress(const Git::FetchTransferStat *stat)
{
    progressBar->setMaximum(stat->totalObjects);
    progressBar->setValue(stat->receivedObjects);
}

void FetchResultWidget::slotPackProgress(const Git::PackProgress *progress)
{
    progressBar->setValue(progress->current);
    progressBar->setMaximum(progress->total);
}

void FetchResultWidget::slotUpdateRef(QSharedPointer<Git::Reference> reference, QSharedPointer<Git::Oid> a, QSharedPointer<Git::Oid> b)
{
    auto item = new QTreeWidgetItem;
    item->setText(0, reference->name());
    item->setText(1, a->toString());
    item->setText(2, b->toString());

    treeWidget->addTopLevelItem(item);
}

void FetchResultWidget::slotFinished()
{
    textBrowser->append("Finished!");
}

#include "moc_fetchresultwidget.cpp"
