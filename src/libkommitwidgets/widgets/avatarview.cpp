#include "avatarview.h"

#include <QCryptographicHash>
#include <QDir>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>

AvatarView::AvatarView(QWidget *parent)
    : QLabel{parent}
{
    setScaledContents(true);
}

QString AvatarView::userEmail() const
{
    return mUserEmail;
}

void AvatarView::setUserEmail(const QString &userEmail)
{
    mUserEmail = userEmail;

    auto emailHash = QCryptographicHash::hash(userEmail.toLower().toUtf8(), QCryptographicHash::Md5).toHex().toLower();
    auto path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/avatars";

    QDir d;
    d.mkpath(path);

    mAvatarFileName = path + "/" + emailHash;

    if (QFile::exists(mAvatarFileName)) {
        setPixmap(QPixmap{mAvatarFileName});
    } else {
        auto avatarUrl = "https://www.gravatar.com/avatar/" + emailHash;

        QNetworkRequest request{QUrl{avatarUrl}};

        QNetworkReply *reply = mNet.get(request);
        connect(reply, &QNetworkReply::finished, this, &AvatarView::slotAvatarDownloadFinished);
    }
}

void AvatarView::slotAvatarDownloadFinished()
{
    auto reply = qobject_cast<QNetworkReply *>(sender());
    QFile avatarFile{mAvatarFileName};
    if (!avatarFile.open(QIODevice::WriteOnly))
        return;
    avatarFile.write(reply->readAll());
    avatarFile.close();

    setPixmap(QPixmap{mAvatarFileName});
}

#include "moc_avatarview.cpp"
