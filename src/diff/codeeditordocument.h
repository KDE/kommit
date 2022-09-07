/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CODEEDITORDOCUMENT_H
#define CODEEDITORDOCUMENT_H

#include <QTextDocument>

class CodeEditorDocument : public QTextDocument
{
    Q_OBJECT

public:
    explicit CodeEditorDocument(QObject *parent = nullptr);

protected:
    QTextObject *createObject(const QTextFormat &f) override;
};

#endif // CODEEDITORDOCUMENT_H
