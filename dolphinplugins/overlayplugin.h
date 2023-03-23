/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "statuscache.h"
#include <KOverlayIconPlugin>
#include <QCache>

class OverlayPlugin : public KOverlayIconPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kfe.kommit.ovarlayiconplugin" FILE "kommitoverlayplugin.json")
public:
    explicit OverlayPlugin(QObject *parent = nullptr);
    QStringList getOverlays(const QUrl &url) override;

private:
    StatusCache mCache;
};
