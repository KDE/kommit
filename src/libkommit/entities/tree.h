#pragma once

#include "file.h"
#include "libkommit_export.h"

#include <git2/types.h>

#include <QDebug>
#include <QMultiMap>
#include <QString>

namespace Git
{

class Commit;
class LIBKOMMIT_EXPORT Tree
{
public:
    enum class EntryType { Unknown, File, Dir, All = Unknown };

    struct Entry {
        QString name;
        EntryType type;
    };

    Tree(git_tree *tree);
    ~Tree();

    QList<Entry> entries(const QString &path) const;
    QStringList entries(const QString &path, EntryType filter) const;
    QSharedPointer<File> file(const QString &path);

    git_tree *gitTree() const;

private:
    git_tree *ptr{nullptr};
    void initTree();
    QMultiMap<QString, Entry> mTreeData;
};

}

template<>
class LIBKOMMIT_EXPORT QListSpecialMethods<Git::Tree::Entry>
{
public:
    Git::Tree::EntryType type(const QString &entryName) const;
    bool comtains(const QString &entryName) const;
};

QDebug LIBKOMMIT_EXPORT operator<<(QDebug d, const QList<Git::Tree::Entry> &list);
