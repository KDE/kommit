#pragma once

#include <QLabel>
#include <QNetworkAccessManager>

class AvatarView : public QLabel
{
    Q_OBJECT

public:
    explicit AvatarView(QWidget *parent = nullptr);

    Q_REQUIRED_RESULT QString userEmail() const;
    void setUserEmail(const QString &userEmail);

private:
    void slotAvatarDownloadFinished();
    QString mUserEmail;
    QString mAvatarFileName;
    QNetworkAccessManager mNet;
};
