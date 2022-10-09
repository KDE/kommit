install(PROGRAMS gitklientdiff.sh DESTINATION bin RENAME gitklientdiff)
install(PROGRAMS gitklientmerge.sh DESTINATION bin RENAME gitklientmerge)
install(PROGRAMS org.kde.gitklient.desktop        DESTINATION ${KDE_INSTALL_APPDIR})
install(PROGRAMS org.kde.gitklient.diff.desktop   DESTINATION ${KDE_INSTALL_APPDIR})
install(PROGRAMS org.kde.gitklient.merge.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.gitklient.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

