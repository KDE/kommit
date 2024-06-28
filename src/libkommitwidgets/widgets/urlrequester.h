#pragma once

#include <kurlrequester.h>
#ifdef LIBKOMMIT_WIDGET_USE_KF
#define URL_REQUESTER_BASE KUrlRequester

class UrlRequester : public KUrlRequester
{
    Q_OBJECT

public:
    UrlRequester(QWidget *parent = nullptr);

    enum class Mode { File = 1, Directory = 2 };
    Q_DECLARE_FLAGS(Modes, Mode)
    Q_FLAG(Modes)

    void setMode(Mode mode);
};

#else
#include <QUrl>
#include <QWidget>

class QLineEdit;
class QToolButton;
class UrlRequester : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY textChanged USER true)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

Q_SIGNALS:
    void urlSelected(const QUrl &);

    void textChanged(const QString &);

private Q_SLOTS:
    void mSlotButtonClicked();

public:
    enum class Mode {
        File = 1,
        Directory = 2,
        Files = 4,
        ExistingOnly = 8,
        LocalOnly = 16,
        ModeMax = 65536,
    };
    UrlRequester(QWidget *parent = nullptr);
    [[nodiscard]] QUrl url() const;
    void setUrl(const QUrl &url);

    [[nodiscard]] QString text() const;
    void setText(const QString &text);

    [[nodiscard]] QUrl startDir() const;
    void setStartDir(const QUrl &startDir);

    [[nodiscard]] Mode mode() const;
    void setMode(Mode mode);

private:
    QLineEdit *mLineEdit;
    QToolButton *mToolButton;
    QUrl mStartDir;
    Mode mMode;
};

#endif
