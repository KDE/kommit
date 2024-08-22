#pragma once

#include "ui_fetchresultwidget.h"

namespace Git
{
class FetchObserver;
class FetchTransferStat;
class Reference;
class Oid;
class Credential;
class PackProgress;
}
class FetchResultWidget : public QWidget, private Ui::FetchResultWidget
{
    Q_OBJECT

public:
    explicit FetchResultWidget(QWidget *parent = nullptr);

    [[nodiscard]] Git::FetchObserver *observer() const;
    void setObserver(Git::FetchObserver *newObserver);

private:
    void slotMessage(const QString &message);
    void slotCredentialRequeted(const QString &url, Git::Credential *cred);
    void slotTransferProgress(const Git::FetchTransferStat *stat);
    void slotPackProgress(const Git::PackProgress *progress);
    void slotUpdateRef(QSharedPointer<Git::Reference> reference, QSharedPointer<Git::Oid> a, QSharedPointer<Git::Oid> b);
    void slotFinished();

    Git::FetchObserver *mObserver{nullptr};
};
