#include "themeiconprovider.h"

#include <QApplication>
#include <QColor>
#include <QDir>
#include <QIcon>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QSvgRenderer>

#include <KColorScheme>
#include <KConfigGroup>
#include <KSharedConfig>

QMap<QString, QIcon> ThemeIconProvider::icons;

namespace
{

QIcon generateIcon(const QString &path, const QColor &color)
{
    QSvgRenderer renderer(path);

    QPixmap pixmap(renderer.defaultSize());
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    renderer.render(&painter);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);

    return QIcon(pixmap);
}

bool isDarkTheme()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, "General");
    QString theme = group.readEntry("ColorScheme", QString());
    return theme.contains("dark", Qt::CaseInsensitive);
}

}

void ThemeIconProvider::reloadAll()
{
    QDir d{":/icons"};

    icons.clear();
    auto color = QApplication::palette().color(QPalette::ButtonText);

    auto iconNames = d.entryList({"*.svg"}, QDir::NoDotAndDotDot | QDir::Files);
    for (auto const &icon : iconNames) {
        auto iconPath = ":/icons/" + icon;
        icons.insert(iconPath, generateIcon(iconPath, color));
    }
}

QIcon ThemeIconProvider::icon(const QString &name)
{
#ifdef QT_DEBUG
    if (!icons.contains(name))
        qFatal() << "Icon" << name << "not found; list=" << icons.keys();
#endif
    return icons.value(name);
}
