#include "authorswidget.h"

#include "gitmanager.h"
#include "models/authorsmodel.h"

AuthorsWidget::AuthorsWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);
    tableViewAuthors->setModel(mGit->authorsModel());
}

void AuthorsWidget::saveState(QSettings &settings) const
{
    save(settings, tableViewAuthors);
}

void AuthorsWidget::restoreState(QSettings &settings)
{
    restore(settings, tableViewAuthors);
}
