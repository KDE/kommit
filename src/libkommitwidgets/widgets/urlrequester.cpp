#include "urlrequester.h"

#ifndef LIBKOMMIT_WIDGET_USE_KF

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>

UrlRequester::UrlRequester(QWidget *parent)
    : QWidget(parent)
    , mMode{Mode::Directory}
{
    mLineEdit = new QLineEdit{this};
    mToolButton = new QToolButton{this};
    auto layout = new QHBoxLayout{this};
    layout->addWidget(mLineEdit);
    layout->addWidget(mToolButton);
    setLayout(layout);

    mToolButton->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    connect(mLineEdit, &QLineEdit::textChanged, this, &UrlRequester::textChanged);
    connect(mToolButton, &QToolButton::clicked, this, &UrlRequester::slotButtonClicked);
}

QUrl UrlRequester::url() const
{
    return QUrl::fromLocalFile(text());
}

void UrlRequester::setUrl(const QUrl &url)
{
    setText(url.toLocalFile());
}

void UrlRequester::slotButtonClicked()
{
    QFileDialog d{this};

    switch (mMode) {
    case Mode::File:
        d.setFileMode(QFileDialog::ExistingFile);
        break;
    case Mode::Directory:
        d.setFileMode(QFileDialog::Directory);
        break;
    case Mode::Files:
        d.setFileMode(QFileDialog::ExistingFiles);
        break;
    case Mode::ExistingOnly:
        break;
    default:
        break;
    }
    d.setDirectory(mStartDir.toLocalFile());
    if (d.exec() == QDialog::Accepted) {
        auto list = d.selectedUrls();
        if (list.empty()) {
            mLineEdit->clear();
            Q_EMIT urlSelected(QUrl{});
        } else {
            mLineEdit->setText(list.first().toLocalFile());
            Q_EMIT urlSelected(list.first());
        }
    }
}

QString UrlRequester::text() const
{
    return mLineEdit->text();
}

void UrlRequester::setText(const QString &text)
{
    if (text == mLineEdit->text())
        return;
    mLineEdit->setText(text);
}

QUrl UrlRequester::startDir() const
{
    return mStartDir;
}

void UrlRequester::setStartDir(const QUrl &startDir)
{
    mStartDir = startDir;
}

UrlRequester::Mode UrlRequester::mode() const
{
    return mMode;
}

void UrlRequester::setMode(Mode mode)
{
    mMode = mode;
}

#else

UrlRequester::UrlRequester(QWidget *parent)
    : KUrlRequester{parent}
{
}

void UrlRequester::setMode(Mode mode)
{
    switch (mode) {
    case Mode::File:
        KUrlRequester::setMode(KFile::File | KFile::ExistingOnly);
        break;
    case Mode::Directory:
        KUrlRequester::setMode(KFile::Directory | KFile::ExistingOnly);
        break;
    }
}

#endif

#include "moc_urlrequester.cpp"
