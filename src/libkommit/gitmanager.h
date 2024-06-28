// Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "blamedata.h"
#include "commands/abstractcommand.h"
#include "entities/file.h"
#include "entities/remote.h"
#include "entities/stash.h"
#include "entities/treediff.h"
#include "filestatus.h"
#include "gitglobal.h"
#include "libkommit_export.h"
#include "types.h"

#include <QObject>
#include <QString>

#include <git2.h>

namespace Git
{

class RemotesModel;
class Branch;
class SubmodulesModel;
class BranchesModel;
class LogsModel;
class StashesModel;
class TagsModel;
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

enum LoadFlag {
    LoadNone = 0,
    LoadStashes = 1,
    LoadRemotes = 2,
    LoadSubmodules = 4,
    LoadBranches = 8,
    LoadLogs = 16,
    LoadTags = 32,
    LoadAll = LoadStashes | LoadRemotes | LoadSubmodules | LoadBranches | LoadLogs | LoadTags
};
Q_DECLARE_FLAGS(LoadFlags, LoadFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(LoadFlags)

template<typename T>
class Expeced
{
public:
    Expeced(const T &data, bool success)
        : mData{data}
        , mSuccess{success}
    {
    }
    Expeced(const QString &msg, bool success)
        : mErrorMessage{msg}
        , mSuccess{success}
    {
    }

    operator bool()
    {
        return mSuccess;
    }

    [[nodiscard]] T data() const
    {
        return mData;
    }
    [[nodiscard]] bool success() const
    {
        return mSuccess;
    }
    [[nodiscard]] QString errorMessage() const
    {
        return mErrorMessage;
    }

private:
    T mData;
    bool mSuccess;
    QString mErrorMessage;

    friend class Manager;
};

struct Exception {
    const int id;
    const QString message;
};

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
    [[nodiscard]] const QString &path() const;
    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool isMerging() const;

    [[nodiscard]] const LoadFlags &loadFlags() const;
    void setLoadFlags(Git::LoadFlags newLoadFlags);

    // branches
    [[nodiscard]] Branch *branch(const QString &branchName) const;
    [[nodiscard]] QString currentBranch() const;
    bool createBranch(const QString &branchName) const;
    bool switchBranch(const QString &branchName) const;
    [[nodiscard]] QPair<int, int> uniqueCommitsOnBranches(const QString &branch1, const QString &branch2) const;
    [[nodiscard]] QStringList branchesNames(BranchType type);
    [[nodiscard]] PointerList<Branch> branches(BranchType type) const;
    [[nodiscard]] bool removeBranch(const QString &branchName) const;
    bool merge(const QString &branchName) const;

    // commits
    Commit *commitByHash(const QString &hash) const;
    [[nodiscard]] PointerList<Commit> commits(const QString &branchName = {}) const;

    // tags
    void forEachTags(std::function<void(QSharedPointer<Tag>)> cb);
    [[nodiscard]] QStringList tagsNames() const;
    [[nodiscard]] QList<QSharedPointer<Tag>> tags() const;
    bool createTag(const QString &name, const QString &message) const;
    bool removeTag(const QString &name) const;
    bool removeTag(QSharedPointer<Tag> tag) const;

    // stashes
    void forEachStash(std::function<void(QSharedPointer<Stash>)> cb);
    PointerList<Stash> stashes() const;
    [[nodiscard]] bool applyStash(QSharedPointer<Stash> stash) const;
    [[nodiscard]] bool popStash(QSharedPointer<Stash> stash) const;
    [[nodiscard]] bool removeStash(QSharedPointer<Stash> stash) const;
    [[nodiscard]] bool removeStash(const QString &name) const;
    [[nodiscard]] bool applyStash(const QString &name) const;
    [[nodiscard]] bool popStash(const QString &name) const;

    bool createStash(const QString &name = QString()) const;

    // remotes
    Remote *remote(const QString &name) const;
    [[nodiscard]] QStringList remotes() const;
    Remote remoteDetails(const QString &remoteName);
    [[nodiscard]] bool addRemote(const QString &name, const QString &url) const;
    [[nodiscard]] bool removeRemote(const QString &name) const;
    [[nodiscard]] bool renameRemote(const QString &name, const QString &newName) const;
    bool fetch(const QString &remoteName, FetchObserver *observer = nullptr);

    // config
    [[nodiscard]] QString config(const QString &name, ConfigType type = ConfigLocal) const;
    [[nodiscard]] bool configBool(const QString &name, ConfigType type = ConfigLocal) const;
    void setConfig(const QString &name, const QString &value, ConfigType type = ConfigLocal) const;
    void unsetConfig(const QString &name, ConfigType type = ConfigLocal) const;
    void forEachConfig(std::function<void(QString, QString)> cb);

    // files
    void addFile(const QString &file) const;
    [[nodiscard]] QStringList ls(const QString &place) const;
    [[nodiscard]] QString fileContent(const QString &place, const QString &fileName) const;
    void saveFile(const QString &place, const QString &fileName, const QString &localFile) const;
    bool revertFile(const QString &filePath) const;
    bool removeFile(const QString &file, bool cached) const;
    [[nodiscard]] QStringList fileLog(const QString &fileName) const;
    BlameData blame(const File &file);
    [[nodiscard]] QMap<QString, ChangeStatus> changedFiles() const;
    [[nodiscard]] QMap<QString, ChangeStatus> changedFiles(const QString &hash) const;
    [[nodiscard]] QStringList ignoredFiles() const;

    Q_DECL_DEPRECATED
    [[nodiscard]] QList<FileStatus> repoFilesStatus() const;

    // notes
    [[nodiscard]] QString readNote(const QString &branchName) const;
    void saveNote(const QString &branchName, const QString &note) const;
    QList<QSharedPointer<Note>> notes() const;

    // refs
    void forEachRefs(std::function<void(QSharedPointer<Reference>)> callback) const;

    // ignores
    bool isIgnored(const QString &path);

    // diffs
    [[nodiscard]] QString diff(const QString &from, const QString &to) const;
    [[nodiscard]] QList<FileStatus> diffBranch(const QString &from) const;
    [[nodiscard]] QList<FileStatus> diffBranches(const QString &from, const QString &to) const;
    [[nodiscard]] QList<FileStatus> diff(AbstractReference *from, AbstractReference *to) const;
    [[nodiscard]] TreeDiff diff(QSharedPointer<Tree> oldTree, QSharedPointer<Tree> newTree = {});

    void forEachCommits(std::function<void(QSharedPointer<Commit>)> callback, const QString &branchName) const;

    // submodules
    void forEachSubmodules(std::function<void(Submodule *)> callback);
    bool addSubmodule(const AddSubmoduleOptions &options) const;
    bool removeSubmodule(const QString &name) const;
    [[nodiscard]] PointerList<Submodule> submodules() const;
    QSharedPointer<Submodule> submodule(const QString &name) const;

    [[nodiscard]] QSharedPointer<Index> index() const;
    [[nodiscard]] QSharedPointer<Tree> headTree() const;

    // models
    [[nodiscard]] RemotesModel *remotesModel() const;
    [[nodiscard]] SubmodulesModel *submodulesModel() const;
    [[nodiscard]] BranchesModel *branchesModel() const;
    [[nodiscard]] LogsModel *logsModel() const;
    [[nodiscard]] StashesModel *stashesModel() const;
    [[nodiscard]] TagsModel *tagsModel() const;

    [[nodiscard]] bool isRebasing() const;
    [[nodiscard]] bool isDetached() const;

    [[nodiscard]] int errorCode() const;

    [[nodiscard]] QString errorMessage() const;

    [[nodiscard]] int errorClass() const;

    [[nodiscard]] git_repository *repoPtr() const;

Q_SIGNALS:
    void pathChanged();
    void reloadRequired();

private:
    ManagerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Manager);

    LIBKOMMIT_NO_EXPORT int findStashIndex(const QString &message) const;
    LIBKOMMIT_NO_EXPORT QStringList readAllNonEmptyOutput(const QStringList &cmd) const;
    LIBKOMMIT_NO_EXPORT QString escapeFileName(const QString &filePath) const;
    LIBKOMMIT_NO_EXPORT void loadAsync();

    friend class File;
    friend class Stash;
    friend class RemotesModel;
    friend class SubmodulesModel;
    friend class BranchesModel;
    friend class LogsModel;
    friend class StashesModel;
    friend class TagsModel;
};

} // namespace Git
