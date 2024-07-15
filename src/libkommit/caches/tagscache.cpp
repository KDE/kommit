/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagscache.h"
#include "entities/tag.h"
#include "gitglobal_p.h"
#include "gitmanager.h"

#include <QDebug>

#include <git2/revparse.h>
#include <git2/tag.h>

namespace Git
{

TagsCache::TagsCache(Manager *parent)
    : OidCache<Tag, git_tag>{parent, git_tag_lookup}
{
}

QSharedPointer<Tag> TagsCache::find(const QString &key)
{
    git_tag *tag;
    git_object *tagObject;

    BEGIN
    STEP git_revparse_single(&tagObject, manager->repoPtr(), key.toLatin1().constData());
    STEP git_tag_lookup(&tag, manager->repoPtr(), git_object_id(tagObject));

    if (IS_OK)
        return Cache::findByPtr(tag);

    return QSharedPointer<Tag>{};
}

void TagsCache::forEach(std::function<void(QSharedPointer<Tag>)> cb)
{
    struct wrapper {
        git_repository *repo;
        std::function<void(QSharedPointer<Tag>)> cb;
        TagsCache *cache;
        Manager *manager;
    };

    wrapper w;
    w.cb = cb;
    w.repo = manager->repoPtr();
    w.cache = this;
    w.manager = manager;

    auto callback_c = [](const char *name, git_oid *oid_c, void *payload) {
        Q_UNUSED(name)
        auto w = reinterpret_cast<wrapper *>(payload);
        git_tag *t;

        BEGIN
        STEP git_tag_lookup(&t, w->repo, oid_c);

        QSharedPointer<Tag> tag;
        if (IS_OK) {
            auto tag = w->cache->findByPtr(t);
            w->cb(tag);
            return 0;
        } else {
            // TODO: find commit
            // auto cmt = w->manager->commitsCache()->find(oid_c);
            // if (!cmt)
            //     return 0;

            git_commit *commit;
            RESTART;
            STEP git_commit_lookup(&commit, w->repo, oid_c);

            PRINT_ERROR;
            RETURN_IF_ERR(0);

            if (w->cache->mTagsByCommit.contains(commit)) {
                auto tag = w->cache->mTagsByCommit.value(commit);
                w->cb(tag);
                return 0;
            }
            git_reference *ref;
            STEP git_reference_lookup(&ref, w->repo, name);
            RETURN_IF_ERR(0);

            auto lightTagName = QString{git_reference_shorthand(ref)};
            tag.reset(new Tag{commit, lightTagName});
            w->cb(tag);
            return 0;
        }

        return 0;
    };

    git_tag_foreach(manager->repoPtr(), callback_c, &w);
}

bool TagsCache::create(const QString &name, const QString &message)
{
    git_object *target = NULL;
    git_oid oid;
    git_signature *sign;
    git_reference *head;

    auto repo = manager->repoPtr();
    BEGIN
    STEP git_signature_default(&sign, repo);
    // STEP git_revparse_single(&target, repo, "HEAD^{commit}");
    STEP git_repository_head(&head, repo);

    if (IS_ERROR) {
        END;
        return false;
    }

    auto targetId = git_reference_target(head);
    STEP git_object_lookup(&target, repo, targetId, GIT_OBJECT_COMMIT);
    STEP git_tag_create(&oid, repo, name.toLatin1().data(), target, sign, message.toUtf8().data(), 0);

    PRINT_ERROR;

    if (IS_ERROR)
        return false;

    findByOid(&oid);
    END;

    return IS_OK;
}

bool TagsCache::remove(QSharedPointer<Tag> tag)
{
    BEGIN
    STEP git_tag_delete(manager->repoPtr(), tag->name().toLocal8Bit().constData());
    PRINT_ERROR;

    if (IS_ERROR)
        return false;

    removeFromList(tag->tagPtr());

    return IS_OK;
}

void TagsCache::clearChildData()
{
    mTagsByCommit.clear();
}

QList<QSharedPointer<Tag>> TagsCache::allTags()
{
    QList<QSharedPointer<Tag>> list;
    forEach([&list](QSharedPointer<Tag> tag) {
        list << tag;
    });
    return list;
}

QStringList TagsCache::allNames()
{
    QStringList list;

    forEach([&list](QSharedPointer<Tag> tag) {
        list << tag->name();
    });
    return list;
}
};
