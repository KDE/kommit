#include "credentialdialog.h"

CredentialDialog::CredentialDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

QString CredentialDialog::password() const
{
    return lineEditPassword->text();
}

void CredentialDialog::setPassword(const QString &password)
{
    lineEditPassword->setText(password);
}

QString CredentialDialog::url() const
{
    return mUrl;
}

void CredentialDialog::setUrl(const QString &url)
{
    mUrl = url;
    labelCredentialsUrl->setText(tr("Credential for %1").arg(url));
}

QString CredentialDialog::username() const
{
    return lineEditUsername->text();
}

void CredentialDialog::setUsername(const QString &username)
{
    lineEditUsername->setText(username);
}

#include "moc_credentialdialog.cpp"
