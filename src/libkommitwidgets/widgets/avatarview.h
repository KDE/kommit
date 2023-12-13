#pragma once

#include <QLabel>
#include <QNetworkAccessManager>

class AvatarView : public QLabel
{
    Q_OBJECT

public:
    AvatarView(QWidget *parent = nullptr);

    Q_REQUIRED_RESULT QString userEmail() const;
    void setUserEmail(const QString &userEmail);

private Q_SLOTS:
    void slotAvatarDownloadFinished();

private:
    QString mUserEmail;
    QString mAvatarFileName;
    QNetworkAccessManager mNet;
};
