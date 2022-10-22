// Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gitmanager.h"

#include "models/branchesmodel.h"
#include "models/logsmodel.h"
#include "models/remotesmodel.h"
#include "models/stashesmodel.h"
#include "models/submodulesmodel.h"
#include "models/tagsmodel.h"

#include "libgitklient_debug.h"
#include <QFile>
#include <QProcess>
#include <QtConcurrent>

namespace Git
{

const QString &Manager::path() const
{
    return mPath;
}

void Manager::setPath(const QString &newPath)
{
    if (mPath == newPath)
        return;

    QProcess p;
    p.setProgram(QStringLiteral("git"));
    p.setArguments({QStringLiteral("rev-parse"), QStringLiteral("--show-toplevel")});
    p.setWorkingDirectory(newPath);
    p.start();
    p.waitForFinished();
    auto ret = p.readAllStandardOutput() + p.readAllStandardError();

    if (ret.contains("fatal")) {
        mPath = QString();
        _isValid = false;
    } else {
        mPath = ret.replace("\n", "");
        _isValid = true;
        loadAsync();

        setIsMerging(QFile::exists(mPath + QStringLiteral("/.git/MERGE_HEAD")));
    }

    Q_EMIT pathChanged();
}

QMap<QString, Manager::ChangeStatus> Manager::changedFiles(const QString &hash) const
{
    QMap<QString, Manager::ChangeStatus> statuses;
    auto buffer = QString(runGit({QStringLiteral("show"), QStringLiteral("--name-status"), hash})).split(QLatin1Char('\n'));

    for (auto &line : buffer) {
        if (!line.trimmed().size())
            continue;

        const auto parts = line.split(QLatin1Char('\t'));
        if (parts.size() != 2)
            continue;

        const auto partFirst{parts.first()};
        if (partFirst == QLatin1Char('A'))
            statuses.insert(parts.at(1), Added);
        else if (partFirst == QLatin1Char('M'))
            statuses.insert(parts.at(1), Modified);
        else if (partFirst == QLatin1Char('D'))
            statuses.insert(parts.at(1), Removed);
        else
            qCDebug(GITKLIENTLIB_LOG) << "Unknown file status" << partFirst;
    }
    return statuses;
}

QStringList Manager::ignoredFiles() const
{
    return readAllNonEmptyOutput({QStringLiteral("check-ignore"), QStringLiteral("*")});
}

QList<FileStatus> Manager::repoFilesStatus() const
{
    const auto buffer = QString(runGit({QStringLiteral("status"),
                                        QStringLiteral("--untracked-files=all"),
                                        QStringLiteral("--ignored"),
                                        QStringLiteral("--short"),
                                        QStringLiteral("--ignore-submodules"),
                                        QStringLiteral("--porcelain")}))
                            .split(QLatin1Char('\n'));
    QList<FileStatus> files;
    for (const auto &item : buffer) {
        if (!item.trimmed().size())
            continue;
        FileStatus fs;
        fs.parseStatusLine(item);
        //        qCDebug(GITKLIENTLIB_LOG) << "[STATUS]" << fs.name() << fs.status();
        fs.setFullPath(mPath + QLatin1Char('/') + fs.name());
        files.append(fs);
    }
    return files;
}

QList<Manager::Log *> Manager::log(const QString &branch) const
{
    const auto lines = QString(runGit({QStringLiteral("--no-pager"), QStringLiteral("log"), branch})).split(QLatin1Char('\n'));

    QList<Log *> logs;
    Log *log{nullptr};
    for (const auto &line : lines) {
        if (line.startsWith(QStringLiteral("commit "))) {
            if (log)
                logs.append(log);
            log = new Log;

            log->hash = line.mid(7).trimmed();
        } else {
            if (!log)
                return {}; // There is an error
            if (line.startsWith(QStringLiteral("Author:"))) {
                log->author = line.mid(8).trimmed();
            } else if (line.startsWith(QStringLiteral("Date: "))) {
                log->date = line.mid(5).trimmed();
            } else {
                const auto l = line.trimmed();
                if (!l.isEmpty())
                    log->message.append(l + QLatin1Char('\n'));
            }
        }
    }

    if (log)
        logs.append(log);

    return logs;
}

bool Manager::isValid() const
{
    return _isValid;
}

bool Manager::addRemote(const QString &name, const QString &url) const
{
    runGit({QStringLiteral("remote"), QStringLiteral("add"), name, url});
    return true;
}

bool Manager::removeRemote(const QString &name) const
{
    runGit({QStringLiteral("remote"), QStringLiteral("remove"), name});
    return true;
}

bool Manager::renameRemote(const QString &name, const QString &newName) const
{
    runGit({QStringLiteral("remote"), QStringLiteral("rename"), name, newName});
    return true;
}

bool Manager::isIgnored(const QString &path)
{
    auto tmp = readAllNonEmptyOutput({QStringLiteral("check-ignore"), path});
    qCDebug(GITKLIENTLIB_LOG) << Q_FUNC_INFO << tmp;
    return !tmp.empty();
}

QPair<int, int> Manager::uniqueCommiteOnBranches(const QString &branch1, const QString &branch2) const
{
    if (branch1 == branch2)
        return qMakePair(0, 0);

    auto ret = readAllNonEmptyOutput(
        {QStringLiteral("rev-list"), QStringLiteral("--left-right"), QStringLiteral("--count"), branch1 + QStringLiteral("...") + branch2});

    if (ret.size() != 1)
        return qMakePair(-1, -1);

    const auto parts = ret.first().split(QLatin1Char('\t'));
    if (parts.size() != 2)
        return qMakePair(-1, -1);

    return qMakePair(parts.at(0).toInt(), parts.at(1).toInt());
}

QStringList Manager::fileLog(const QString &fileName) const
{
    return readAllNonEmptyOutput({QStringLiteral("log"), QStringLiteral("--format=format:%H"), QStringLiteral("--"), fileName});
}

QString Manager::diff(const QString &from, const QString &to) const
{
    return runGit({QStringLiteral("diff"), from, to});
}

QList<FileStatus> Manager::diffBranch(const QString &from) const
{
    auto buffer = QString(runGit({QStringLiteral("diff"), from, QStringLiteral("--name-status")})).split(QLatin1Char('\n'));
    QList<FileStatus> files;
    for (auto &item : buffer) {
        if (!item.trimmed().size())
            continue;
        auto parts = item.split(QStringLiteral("\t"));
        if (parts.size() != 2)
            continue;

        FileStatus fs;
        fs.setStatus(parts.at(0));
        fs.setName(parts.at(1));
        files.append(fs);
    }
    return files;
}

QList<FileStatus> Manager::diffBranches(const QString &from, const QString &to) const
{
    const auto buffer = QString(runGit({QStringLiteral("diff"), from + QStringLiteral("..") + to, QStringLiteral("--name-status")})).split(QLatin1Char('\n'));
    QList<FileStatus> files;
    for (const auto &item : buffer) {
        if (!item.trimmed().size())
            continue;
        const auto parts = item.split(QLatin1Char('\t'));
        if (parts.size() != 2)
            continue;

        FileStatus fs;
        fs.setStatus(parts.at(0));
        fs.setName(parts.at(1));
        files.append(fs);
    }
    return files;
}

QString Manager::config(const QString &name, ConfigType type) const
{
    QStringList cmd;
    switch (type) {
    case ConfigLocal:
        cmd = QStringList{QStringLiteral("config"), name};
        break;
    case ConfigGlobal:
        cmd = QStringList{QStringLiteral("config"), QStringLiteral("--global"), name};
        break;
    }
    const auto list = readAllNonEmptyOutput(cmd);
    if (!list.empty())
        return list.first();

    return {};
}

bool Manager::configBool(const QString &name, ConfigType type) const
{
    const auto buffer = config(name, type);
    return buffer == QStringLiteral("true") || buffer == QStringLiteral("yes") || buffer == QStringLiteral("on");
}

void Manager::setConfig(const QString &name, const QString &value, ConfigType type) const
{
    QStringList cmd;
    switch (type) {
    case ConfigLocal:
        cmd = QStringList{QStringLiteral("config"), name, value};
        break;
    case ConfigGlobal:
        cmd = QStringList{QStringLiteral("config"), QStringLiteral("--global"), name, value};
        break;
    }

    runGit(cmd);
}

void Manager::unsetConfig(const QString &name, ConfigType type) const
{
    QStringList cmd{QStringLiteral("config"), QStringLiteral("--unset")};

    if (type == ConfigGlobal)
        cmd.append(QStringLiteral("--global"));

    cmd.append(name);

    runGit(cmd);
}

QStringList Manager::readAllNonEmptyOutput(const QStringList &cmd) const
{
    QStringList list;
    const auto out = QString(runGit(cmd)).split(QLatin1Char('\n'));

    for (const auto &line : out) {
        const auto b = line.trimmed();
        if (b.isEmpty())
            continue;

        list.append(b.trimmed());
    }
    return list;
}

QString Manager::escapeFileName(const QString &filePath) const
{
    if (filePath.contains(QLatin1Char(' ')))
        return QLatin1Char('\'') + filePath + QLatin1Char('\'');
    return filePath;
}

bool load(AbstractGitItemsModel *cache)
{
    cache->load();
    return true;
}
void Manager::loadAsync()
{
    QList<AbstractGitItemsModel *> models;
    if (_loadFlags & LoadStashes)
        models << _stashesCache;
    if (_loadFlags & LoadRemotes)
        models << _remotesModel;
    if (_loadFlags & LoadSubmodules)
        models << _submodulesModel;
    if (_loadFlags & LoadBranches)
        models << _branchesModel;
    if (_loadFlags & LoadLogs)
        models << _logsCache;
    if (_loadFlags & LoadTags)
        models << _tagsModel;

    if (!models.empty())
        QtConcurrent::mapped(models, load);
}

TagsModel *Manager::tagsModel() const
{
    return _tagsModel;
}

StashesModel *Manager::stashesModel() const
{
    return _stashesCache;
}

LogsModel *Manager::logsModel() const
{
    return _logsCache;
}

BranchesModel *Manager::branchesModel() const
{
    return _branchesModel;
}

SubmodulesModel *Manager::submodulesModel() const
{
    return _submodulesModel;
}

RemotesModel *Manager::remotesModel() const
{
    return _remotesModel;
}

const LoadFlags &Manager::loadFlags() const
{
    return _loadFlags;
}

void Manager::setLoadFlags(Git::LoadFlags newLoadFlags)
{
    _loadFlags = newLoadFlags;
}

QString Manager::readNote(const QString &branchName) const
{
    return runGit({QStringLiteral("notes"), QStringLiteral("show"), branchName});
}

void Manager::saveNote(const QString &branchName, const QString &note) const
{
    runGit({QStringLiteral("notes"), QStringLiteral("add"), branchName, QStringLiteral("-f"), QStringLiteral("--message=") + note});
}

Manager::Manager()
    : QObject()
    , _remotesModel{new RemotesModel(this)}
    , _submodulesModel{new SubmodulesModel(this)}
    , _branchesModel{new BranchesModel(this)}
    , _logsCache{new LogsModel(this)}
    , _stashesCache{new StashesModel(this)}
    , _tagsModel{new TagsModel(this)}
{
}

Manager::Manager(const QString &path)
    : QObject()
    , _remotesModel{new RemotesModel(this)}
    , _submodulesModel{new SubmodulesModel(this)}
    , _branchesModel{new BranchesModel(this)}
    , _logsCache{new LogsModel(this)}
    , _stashesCache{new StashesModel(this)}
    , _tagsModel{new TagsModel(this)}
{
    setPath(path);
}

Manager *Manager::instance()
{
    static Manager instance;
    return &instance;
}

QString Manager::currentBranch() const
{
    const auto ret = QString(runGit({QStringLiteral("rev-parse"), QStringLiteral("--abbrev-ref"), QStringLiteral("HEAD")}))
                         .remove(QLatin1Char('\n'))
                         .remove(QLatin1Char('\r'));
    return ret;
}

QString Manager::run(const AbstractCommand &cmd) const
{
    return QString(runGit(cmd.generateArgs()));
}

void Manager::init(const QString &path)
{
    mPath = path;
    runGit({QStringLiteral("init")});
}

bool Manager::isGitDir() const
{
    auto out = runGit(QStringList() << QStringLiteral("status"));
    return !out.contains("fatal: not a git repository");
}

QByteArray Manager::runGit(const QStringList &args) const
{
    //    qCDebug(GITKLIENTLIB_LOG).noquote() << "Running: git " << args.join(" ");

    QProcess p;
    p.setProgram(QStringLiteral("git"));
    p.setArguments(args);
    p.setWorkingDirectory(mPath);
    p.start();
    p.waitForFinished();
    auto out = p.readAllStandardOutput();
    auto err = p.readAllStandardError();

    if (p.exitStatus() == QProcess::CrashExit) {
        qWarning() << "=== Crash on git process ===";
        qWarning() << "====\nERROR:\n====\n" << err;
        qWarning() << "====\nOUTPUR:\n====\n" << out;
    }
    return out; // + err;
}

QStringList Manager::ls(const QString &place) const
{
    auto buffer = readAllNonEmptyOutput({QStringLiteral("ls-tree"), QStringLiteral("--name-only"), QStringLiteral("-r"), place});
    QMutableListIterator<QString> it(buffer);
    while (it.hasNext()) {
        auto s = it.next();
        if (s.startsWith(QLatin1String("\"")) && s.endsWith(QLatin1String("\"")))
            it.setValue(s.mid(1, s.length() - 2));
    }
    return buffer;
}

QString Manager::fileContent(const QString &place, const QString &fileName) const
{
    return runGit({QStringLiteral("show"), place + QLatin1Char(':') + fileName});
}

void Manager::saveFile(const QString &place, const QString &fileName, const QString &localFile) const
{
    auto buffer = runGit({QStringLiteral("show"), place + QLatin1Char(':') + fileName});
    QFile f{localFile};
    if (!f.open(QIODevice::WriteOnly))
        return;
    f.write(buffer);
    f.close();
}

QStringList Manager::branches() const
{
    QStringList branchesList;
    auto out = QString(runGit({QStringLiteral("branch"), QStringLiteral("--list")})).split(QLatin1Char('\n'));

    for (auto &line : out) {
        auto b = line.trimmed();
        if (b.isEmpty())
            continue;
        if (b.startsWith(QLatin1String("* ")))
            b = b.mid(2);

        if (b.startsWith(QLatin1String("(HEAD detached at")))
            continue;

        branchesList.append(b.trimmed());
    }
    return branchesList;
}

QStringList Manager::remoteBranches() const
{
    QStringList branchesList;
    const auto out = QString(runGit({QStringLiteral("branch"), QStringLiteral("--remote"), QStringLiteral("--list")})).split(QLatin1Char('\n'));

    for (const auto &line : out) {
        auto b = line.trimmed();
        if (b.isEmpty())
            continue;
        if (b.startsWith(QStringLiteral("* ")))
            b = b.mid(2);

        if (!b.contains(QStringLiteral("->")))
            branchesList.append(b.trimmed());
    }
    return branchesList;
}

QStringList Manager::remotes() const
{
    return readAllNonEmptyOutput({QStringLiteral("remote")});
}

QStringList Manager::tags() const
{
    return readAllNonEmptyOutput({QStringLiteral("tag"), QStringLiteral("--list")});
}

void Manager::createTag(const QString &name, const QString &message) const
{
    runGit({QStringLiteral("tag"), QStringLiteral("-a"), name, QStringLiteral("--message"), message});
}

QList<Stash> Manager::stashes()
{
    QList<Stash> ret;
    const auto list = readAllNonEmptyOutput({QStringLiteral("stash"), QStringLiteral("list"), QStringLiteral("--format=format:%s%m%an%m%ae%m%aD")});
    int id{0};
    for (const auto &item : std::as_const(list)) {
        const auto parts = item.split(QStringLiteral(">"));
        if (parts.size() != 4)
            continue;

        const auto subject = parts.first();
        Stash stash(this, QStringLiteral("stash@{%1}").arg(id));

        stash.mSubject = subject;
        stash.mAuthorName = parts.at(1);
        stash.mAuthorEmail = parts.at(2);
        stash.mPushTime = QDateTime::fromString(parts.at(3), Qt::RFC2822Date);
        qCDebug(GITKLIENTLIB_LOG) << item << subject << stash.mPushTime;

        ret.append(stash);
        id++;
    }
    return ret;
}

void Manager::createStash(const QString &name) const
{
    QStringList args{QStringLiteral("stash"), QStringLiteral("push")};

    if (!name.isEmpty())
        args.append({QStringLiteral("--message"), name});

    const auto ret = runGit(args);
    qCDebug(GITKLIENTLIB_LOG) << ret;
}

bool Manager::removeStash(const QString &name) const
{
    runGit({QStringLiteral("stash"), QStringLiteral("drop"), name});
    return true;
}

bool Manager::applyStash(const QString &name) const
{
    runGit({QStringLiteral("stash"), QStringLiteral("apply"), name});
    return true;
}

Remote Manager::remoteDetails(const QString &remoteName)
{
    if (_remotes.contains(remoteName))
        return _remotes.value(remoteName);
    Remote r;
    auto ret = QString(runGit({QStringLiteral("remote"), QStringLiteral("show"), remoteName}));
    r.parse(ret);
    _remotes.insert(remoteName, r);
    return r;
}

bool Manager::removeBranch(const QString &branchName) const
{
    auto ret = readAllNonEmptyOutput({QStringLiteral("branch"), QStringLiteral("-D"), branchName});
    return true;
}

BlameData Manager::blame(const File &file)
{
    //    auto logList = logs();
    BlameData b;
    auto lines = readAllNonEmptyOutput({QStringLiteral("--no-pager"), QStringLiteral("blame"), QStringLiteral("-l"), file.fileName()});
    b.reserve(lines.size());

    for (auto &line : lines) {
        BlameDataRow row;
        row.commitHash = line.mid(0, 40);

        auto metaIndex = line.indexOf(QLatin1Char(')'));
        row.code = line.mid(metaIndex + 1);

        auto hash = row.commitHash;
        if (hash.startsWith(QLatin1Char('^')))
            hash = hash.remove(0, 1);
        auto log = _logsCache->findLogByHash(hash);
        //        if (!log)
        //            qCDebug(GITKLIENTLIB_LOG) << "Log not found" << hash;
        row.log = log;
        // const auto parts = line.split("\t");
        b.append(row);
    }

    return b;
}

void Manager::revertFile(const QString &filePath) const
{
    runGit({QStringLiteral("checkout"), QStringLiteral("--"), filePath});
}

QMap<QString, Manager::ChangeStatus> Manager::changedFiles() const
{
    // status --untracked-files=all --ignored --short --ignore-submodules --porcelain
    QMap<QString, Manager::ChangeStatus> statuses;
    const auto buffer = QString(runGit({QStringLiteral("status"), QStringLiteral("--short")})).split(QLatin1Char('\n'));

    for (const auto &line : buffer) {
        if (!line.trimmed().size())
            continue;

        auto status = line.mid(1, 2).trimmed();
        const auto fileName = line.mid(3);

        if (status == QLatin1Char('M'))
            statuses.insert(fileName, Modified);
        else if (status == QLatin1Char('A'))
            statuses.insert(fileName, Added);
        else if (status == QLatin1Char('D'))
            statuses.insert(fileName, Removed);
        else if (status == QLatin1Char('R'))
            statuses.insert(fileName, Renamed);
        else if (status == QLatin1Char('C'))
            statuses.insert(fileName, Copied);
        else if (status == QLatin1Char('U'))
            statuses.insert(fileName, UpdatedButInmerged);
        else if (status == QLatin1Char('?'))
            statuses.insert(fileName, Untracked);
        else if (status == QLatin1Char('!'))
            statuses.insert(fileName, Ignored);
        else
            statuses.insert(fileName, Unknown);
    }
    return statuses;
}

void Manager::commit(const QString &message) const
{
    runGit({QStringLiteral("commit"), QStringLiteral("-m"), message});
}

void Manager::push() const
{
    runGit({QStringLiteral("push"), QStringLiteral("origin"), QStringLiteral("master")});
}

void Manager::addFile(const QString &file) const
{
    runGit({QStringLiteral("add"), file});
}

void Manager::removeFile(const QString &file, bool cached) const
{
    QStringList args;
    args.append(QStringLiteral("rm"));
    if (cached)
        args.append(QStringLiteral("--cached"));
    args.append(file);
    runGit(args);
}

bool Manager::isMerging() const
{
    return m_isMerging;
}

void Manager::setIsMerging(bool newIsMerging)
{
    if (m_isMerging == newIsMerging)
        return;
    m_isMerging = newIsMerging;
    Q_EMIT isMergingChanged();
}

} // namespace Git
