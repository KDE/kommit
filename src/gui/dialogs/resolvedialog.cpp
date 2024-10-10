#include "resolvedialog.h"

#include "chooseconflictedfiledialog.h"

#include <entities/blob.h>
#include <entities/index.h>
#include <repository.h>
#include <windows/mergewindow.h>

ResolveDialog::ResolveDialog(Git::Repository *git, QWidget *parent)
    : QDialog(parent)
    , mGit{git}
{
    setupUi(this);

    mConflicts = mGit->index()->conflicts();

    for (auto &conflict : *mConflicts) {
        listWidget->addItem(conflict->ancestorIndex->path());
    }
    updateConflictsCount();

    connect(listWidget, &QListWidget::itemDoubleClicked, this, &ResolveDialog::slotListWidgetItemDoubleClicked);
}

void ResolveDialog::processRemovedConflict(Git::ConflictIndex *conflict)
{
    ChooseConflictedFileDialog d{conflict->ancestorIndex->blob(), conflict->ourIndex->blob(), conflict->theirIndex->blob(), this};
    QString fileName;
    if (!conflict->ancestorIndex->blob().isNull())
        fileName = conflict->ancestorIndex->blob()->filePath();
    else if (!conflict->ourIndex->blob().isNull())
        fileName = conflict->ourIndex->blob()->filePath();
    else if (!conflict->theirIndex->blob().isNull())
        fileName = conflict->theirIndex->blob()->filePath();
    else
        return;

    if (d.exec() == QDialog::Accepted) {
        switch (d.selectedSide()) {
        case ChooseConflictedFileDialog::Side::None:

            break;
        case ChooseConflictedFileDialog::Side::Base:

            break;

        case ChooseConflictedFileDialog::Side::Local:
            break;

        case ChooseConflictedFileDialog::Side::Remote:
            break;
        }
    }
}

void ResolveDialog::slotListWidgetItemDoubleClicked(QListWidgetItem *item)
{
    auto row = listWidget->indexFromItem(item).row();

    if (row == -1)
        return;
    auto conflict = mConflicts->at(row);

    if (conflict->ancestorIndex->blob().isNull() || conflict->ourIndex->blob().isNull() || conflict->theirIndex->blob().isNull()) {
        processRemovedConflict(conflict);
        return;
    }

    auto w = new MergeWindow{mGit, MergeWindow::NoParams};
    w->setBaseFile(conflict->ancestorIndex->blob());
    w->setLocalFile(conflict->ourIndex->blob());
    w->setRemoteFile(conflict->theirIndex->blob());
    w->compare();

    w->exec();

    updateConflictsCount();
}

void ResolveDialog::updateConflictsCount()
{
    if (mConflicts->size() == 0)
        labelConflictsCount->setText(i18n("All conflicts resolved"));
    else if (mConflicts->size() == 1)
        labelConflictsCount->setText(i18n("1 conflict remains"));
    else
        labelConflictsCount->setText(i18n("%1 conflicts remains", mConflicts->size()));
}
