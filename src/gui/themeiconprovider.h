#pragma once

#include <QIcon>

class ThemeIconProvider
{
public:
    ThemeIconProvider();

    static void reloadAll();
    static QIcon icon(const QString &name);

private:
    static QMap<QString, QIcon> icons;
};
