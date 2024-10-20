/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagscache.h"
#include "entities/oid.h"
#include "gitglobal_p.h"
#include "repository.h"

#include <QDebug>

#include <git2/revparse.h>
#include <git2/tag.h>

namespace Git
{

TagsCache::TagsCache(Repository *parent)
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
        Repository *manager;
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

bool TagsCache::create(const QString &name, const QString &message, QSharedPointer<Object> target, QSharedPointer<Signature> signature)
{
    git_object *targetObject = NULL;
    git_oid oid;
    git_signature *sign;

    auto repo = manager->repoPtr();
    BEGIN

    if (target.isNull()) {
        auto head = manager->head();
        if (head.isNull())
            return false;

        auto targetId = head->target()->oidPtr();
        STEP git_object_lookup(&targetObject, repo, targetId, GIT_OBJECT_COMMIT);
    } else {
        targetObject = target->objectPtr();
    }

    if (signature.isNull())
        STEP git_signature_default(&sign, repo);
    else
        sign = signature->signaturePtr();
    // STEP git_revparse_single(&targetObject, repo, "HEAD");
    // STEP git_repository_head(&head, repo);

    if (IS_ERROR) {
        END;
        return false;
    }

    // auto targetId = git_reference_target(head);
    // STEP git_object_lookup(&target, repo, targetId, GIT_OBJECT_COMMIT);
    STEP git_tag_create(&oid, repo, name.toUtf8().data(), targetObject, sign, message.toUtf8().data(), 0);

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
