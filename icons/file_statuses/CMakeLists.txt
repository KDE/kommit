set(KOMMIT_ICONS
  sc-actions-git-status-added.svg
  sc-actions-git-status-ignored.svg
  sc-actions-git-status-modified.svg
  sc-actions-git-status-removed.svg
  sc-actions-git-status-unknown.svg
  sc-actions-git-status-update.svg
  sc-actions-git-status-renamed.svg
)

ecm_install_icons(ICONS ${KOMMIT_ICONS} DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor)
