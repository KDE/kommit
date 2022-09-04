#include "initdialog.h"
#include <QPushButton>

InitDialog::InitDialog(Git::Manager *git, QWidget *parent, Qt::WindowFlags f)
    : AppDialog(git, parent, f)
{
    setupUi(this);
    connect(lineEditPath, &QLineEdit::textChanged, this, [this](const QString &text) {
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.trimmed().isEmpty());
    });
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

QString InitDialog::path() const
{
    return lineEditPath->text();
}

void InitDialog::setPath(const QString &path)
{
    lineEditPath->setText(path);
}
