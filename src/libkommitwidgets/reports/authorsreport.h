/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"

#include "abstractreport.h"
#include <Kommit/Signature>
#include <QDateTime>
#include <QSharedPointer>

class LIBKOMMITWIDGETS_EXPORT AuthorsReport : public AbstractReport
{
    Q_OBJECT

public:
    explicit AuthorsReport(Git::Repository *git, QObject *parent = nullptr);
    ~AuthorsReport() override;

    void reload() override;
    [[nodiscard]] QString name() const override;

    [[nodiscard]] int columnCount() const override;
    [[nodiscard]] QStringList headerData() const override;

    [[nodiscard]] bool supportChart() const override;
    [[nodiscard]] QString axisXTitle() const override;
    [[nodiscard]] QString axisYTitle() const override;

    [[nodiscard]] int labelsAngle() const override;

private:
    struct DatesRange {
        int count = 0;
        QDateTime first;
        QDateTime last;

        void begin(const QDateTime &time);
        void increase(const QDateTime &time);
    };

    struct Author {
        QString email;
        QString name;

        DatesRange commits;
        DatesRange authoredCommits;
        DatesRange tags;
    };

    enum AuthorCreateReason {
        Commit,
        AuthoredCommit,
        Tag,
    };
    LIBKOMMITWIDGETS_NO_EXPORT Author *findOrCreate(const Git::Signature &signature, AuthorCreateReason reason);

    QList<Author *> mData;
};
