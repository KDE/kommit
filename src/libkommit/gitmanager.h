// Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "libkommit_export.h"

#include "types.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>

#include <git2.h>

namespace Git
{

class Branch;
class Tag;
class Manager;
class Submodule;
class FetchObserver;
class CloneObserver;
class PushObserver;
class Reference;
class AbstractReference;
class AddSubmoduleOptions;
class Index;
class Tree;
class Note;
class ManagerPrivate;
class Commit;
class CommitsCache;
class BranchesCache;
class TagsCache;
class RemotesCache;
class NotesCache;
class SubmodulesCache;
class StashesCache;
class ReferenceCache;
class AbstractCommand;
class BlameData;
class FileStatus;
class File;
class TreeDiff;

class LIBKOMMIT_EXPORT Manager : public QObject
{
    Q_OBJECT

public:
    enum class BranchType {
        LocalBranch,
        RemoteBranch,
        AllBranches,
    };
    enum ConfigType {
        ConfigGlobal,
        ConfigLocal,
    };

    Manager();
    explicit Manager(git_repository *repo);
    explicit Manager(const QString &path);
    static Manager *instance();
    virtual ~Manager();

    // run
    QString run(const AbstractCommand &cmd) const;
    QString runGit(const QStringList &args) const;

    // common actions
    bool init(const QString &path);
    bool clone(const QString &url, const QString &localPath, CloneObserver *observer = nullptr);
    void commit(const QString &message);
    void push(PushObserver *observer = nullptr) const;
    bool open(const QString &newPath);
    QSharedPointer<Reference> head() const;

    // properties
    Q_REQUIRED_RESULT const QString &path() const;
    Q_REQUIRED_RESULT bool isValid() const;
    Q_REQUIRED_RESULT bool isMerging() const;

    // branches
    bool switchBranch(const QString &branchName) const;
    Q_REQUIRED_RESULT QPair<int, int> uniqueCommitsOnBranches(const QString &branch1, const QString &branch2) const;

    // remotes
    bool fetch(const QString &remoteName, FetchObserver *observer = nullptr);

    // config
    Q_REQUIRED_RESULT QString config(const QString &name, ConfigType type = ConfigLocal) const;
    Q_REQUIRED_RESULT bool configBool(const QString &name, ConfigType type = ConfigLocal) const;
    void setConfig(const QString &name, const QString &value, ConfigType type = ConfigLocal) const;
    void unsetConfig(const QString &name, ConfigType type = ConfigLocal) const;
    void forEachConfig(std::function<void(QString, QString)> cb);

    // files
    void addFile(const QString &file) const;
    Q_REQUIRED_RESULT QStringList ls(const QString &place) const;
    Q_REQUIRED_RESULT QString fileContent(const QString &place, const QString &fileName) const;
    void saveFile(const QString &place, const QString &fileName, const QString &localFile) const;
    bool revertFile(const QString &filePath) const;
    bool removeFile(const QString &file, bool cached) const;
    Q_REQUIRED_RESULT QStringList fileLog(const QString &fileName) const;
    BlameData blame(QSharedPointer<File> file);
    Q_REQUIRED_RESULT QMap<QString, ChangeStatus> changedFiles() const;
    Q_REQUIRED_RESULT QMap<QString, ChangeStatus> changedFiles(const QString &hash) const;
    Q_REQUIRED_RESULT QStringList ignoredFiles() const;

    Q_DECL_DEPRECATED
    Q_REQUIRED_RESULT QList<FileStatus> repoFilesStatus() const;

    // ignores
    bool isIgnored(const QString &path);

    // diffs
    Q_REQUIRED_RESULT QString diff(const QString &from, const QString &to) const;
    Q_REQUIRED_RESULT QList<FileStatus> diffBranch(const QString &from) const;
    Q_REQUIRED_RESULT QList<FileStatus> diffBranches(const QString &from, const QString &to) const;
    Q_REQUIRED_RESULT QList<FileStatus> diff(AbstractReference *from, AbstractReference *to) const;
    Q_REQUIRED_RESULT TreeDiff diff(QSharedPointer<Tree> oldTree, QSharedPointer<Tree> newTree = {});

    void forEachCommits(std::function<void(QSharedPointer<Commit>)> callback, const QString &branchName) const;

    Q_REQUIRED_RESULT QSharedPointer<Index> index() const;
    Q_REQUIRED_RESULT QSharedPointer<Tree> headTree() const;

    Q_REQUIRED_RESULT bool isRebasing() const;
    Q_REQUIRED_RESULT bool isDetached() const;

    Q_REQUIRED_RESULT int errorCode() const;

    Q_REQUIRED_RESULT QString errorMessage() const;

    Q_REQUIRED_RESULT int errorClass() const;

    Q_REQUIRED_RESULT git_repository *repoPtr() const;
    Q_REQUIRED_RESULT static Manager *owner(git_repository *repo);

    Q_REQUIRED_RESULT CommitsCache *commits() const;
    Q_REQUIRED_RESULT SubmodulesCache *submodules() const;
    Q_REQUIRED_RESULT RemotesCache *remotes() const;
    Q_REQUIRED_RESULT BranchesCache *branches() const;
    Q_REQUIRED_RESULT TagsCache *tags() const;
    Q_REQUIRED_RESULT NotesCache *notes() const;
    Q_REQUIRED_RESULT StashesCache *stashes() const;
    Q_REQUIRED_RESULT ReferenceCache *references() const;

Q_SIGNALS:
    void pathChanged();
    void reloadRequired();

private:
    ManagerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Manager)

    LIBKOMMIT_NO_EXPORT QStringList readAllNonEmptyOutput(const QStringList &cmd) const;
};

} // namespace Git
