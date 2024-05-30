/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedsubmodulesdialog.h"
#include "dialogs/commitpushdialog.h"
#include "entities/submodule.h"
#include "gitmanager.h"
#include <models/changedfilesmodel.h>

ChangedSubmodulesDialog::ChangedSubmodulesDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    mModel = new ChangedFilesModel(git, true, this);
    reload();
    connect(buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &QDialog::close);
}

void ChangedSubmodulesDialog::mSlotComitPushButtonClicked()
{
    auto pushButton = qobject_cast<QPushButton *>(sender());

    auto submodule = mButtonsMap.value(pushButton);
    auto g = mGit->submodule(submodule)->open();
    CommitPushDialog d{g};
    d.setWindowTitle(tr("Commit/Push submodule: %1").arg(submodule));
    d.exec();
}

void ChangedSubmodulesDialog::reload()
{
    auto modules = mGit->submodules();

    for (auto const &submodule : std::as_const(modules)) {
        if ((submodule->status() & Git::Submodule::Status::WdWdModified) == 0)
            continue;

        auto w = new QWidget{this};

        auto hl = new QHBoxLayout{w};

        auto lbl = new QLabel{w};
        lbl->setText(submodule->name());
        hl->addWidget(lbl);
        auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hl->addItem(horizontalSpacer);

        auto btn = new QPushButton{w};
        btn->setText("Commit/Push");
        hl->addWidget(btn);

        scrollAreaVerticalLayout->addWidget(w);

        mButtonsMap.insert(btn, submodule->name());
        connect(btn, &QPushButton::clicked, this, &ChangedSubmodulesDialog::mSlotComitPushButtonClicked);
    }

    auto extraSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    scrollAreaVerticalLayout->addItem(extraSpacer);
}

#include "moc_changedsubmodulesdialog.cpp"
