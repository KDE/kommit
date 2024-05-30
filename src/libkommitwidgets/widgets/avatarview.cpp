#include "avatarview.h"
// #include "gravatarcache.h"

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

    const auto emailHash = QCryptographicHash::hash(userEmail.toLower().toUtf8(), QCryptographicHash::Md5).toHex().toLower();
    const auto path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/avatars");

    QDir d;
    d.mkpath(path);

    mAvatarFileName = path + QLatin1Char('/') + emailHash;

    if (QFile::exists(mAvatarFileName)) {
        setPixmap(QPixmap{mAvatarFileName});
    } else {
        const auto avatarUrl = QStringLiteral("https://www.gravatar.com/avatar/%1").arg(emailHash);

        QNetworkRequest request{QUrl{avatarUrl}};

        QNetworkReply *reply = mNet.get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            QFile avatarFile{mAvatarFileName};
            if (avatarFile.open(QIODevice::WriteOnly)) {
                avatarFile.write(reply->readAll());
                avatarFile.close();

                setPixmap(QPixmap{mAvatarFileName});
            }
            reply->deleteLater();
        });
    }
}

#include "moc_avatarview.cpp"
