/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedsubmodulesdialog.h"
#include "caches/submodulescache.h"
#include "dialogs/commitpushdialog.h"
#include "entities/submodule.h"
#include "repository.h"
#include "models/changedfilesmodel.h"

ChangedSubmodulesDialog::ChangedSubmodulesDialog(Git::Repository *git, QWidget *parent)
    : AppDialog(git, parent)
    , mModel(new ChangedFilesModel(git, true, this))
{
    setupUi(this);

    reload();
    connect(buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &QDialog::close);
}

void ChangedSubmodulesDialog::slotComitPushButtonClicked(const QString &submodule)
{
    auto g{mGit->submodules()->findByName(submodule)->open()};
    CommitPushDialog d{g};
    d.setWindowTitle(i18nc("@title:window", "Commit/Push submodule: %1", submodule));
    d.exec();
}

void ChangedSubmodulesDialog::reload()
{
    auto modules = mGit->submodules()->allSubmodules();

    for (auto const &submodule : std::as_const(modules)) {
        if ((submodule->status() & Git::Submodule::Status::WdWdModified) == 0)
            continue;

        auto w = new QWidget{this};

        auto hl = new QHBoxLayout{w};

        auto lbl = new QLabel{w};
        const QString submoduleName{submodule->name()};
        lbl->setText(submoduleName);
        hl->addWidget(lbl);
        auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hl->addItem(horizontalSpacer);

        auto btn = new QPushButton{i18nc("@action", "Commit/Push"), w};
        hl->addWidget(btn);

        scrollAreaVerticalLayout->addWidget(w);

        connect(btn, &QPushButton::clicked, this, [this, submoduleName]() {
            slotComitPushButtonClicked(submoduleName);
        });
    }

    auto extraSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    scrollAreaVerticalLayout->addItem(extraSpacer);
}

#include "moc_changedsubmodulesdialog.cpp"
