#ifndef AUTHORSWIDGET_H
#define AUTHORSWIDGET_H

#include "ui_authorswidget.h"
#include "widgetbase.h"

class AuthorsWidget : public WidgetBase, private Ui::AuthorsWidget
{
    Q_OBJECT

public:
    explicit AuthorsWidget(Git::Manager *git, AppWindow *parent = nullptr);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;
};

#endif // AUTHORSWIDGET_H
