#pragma once

#include "ui_fetchresultwidget.h"

#include <Kommit/Credential>
#include <Kommit/Oid>
#include <Kommit/Reference>

namespace Git
{
class FetchObserver;
class FetchTransferStat;
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
    void slotUpdateRef(const Git::Reference &reference, const Git::Oid &a, const Git::Oid &b);
    void slotFinished();

    Git::FetchObserver *mObserver{nullptr};
};
