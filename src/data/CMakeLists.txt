# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
install(PROGRAMS kommitdiff.sh DESTINATION bin RENAME kommitdiff)
install(PROGRAMS kommitmerge.sh DESTINATION bin RENAME kommitmerge)
install(PROGRAMS org.kde.kommit.desktop        DESTINATION ${KDE_INSTALL_APPDIR})
install(PROGRAMS org.kde.kommit.diff.desktop   DESTINATION ${KDE_INSTALL_APPDIR})
install(PROGRAMS org.kde.kommit.merge.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kommit.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

