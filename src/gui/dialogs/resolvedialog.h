#pragma once

#include "ui_resolvedialog.h"

#include <Kommit/Index>

namespace Git
{
class Repository;
class ConflictIndexList;
}

class ResolveDialog : public QDialog, private Ui::ResolveDialog
{
    Q_OBJECT

public:
    explicit ResolveDialog(Git::Repository *git, QWidget *parent = nullptr);

private:
    void slotListWidgetItemDoubleClicked(QListWidgetItem *item);
    void updateConflictsCount();

    QSharedPointer<Git::ConflictIndexList> mConflicts;
    Git::Repository *mGit;
    void processRemovedConflict(Git::ConflictIndex *conflict);
};
