#ifndef ACTIONS_H
#define ACTIONS_H

#define PACK(...)                                                                                                                                              \
    {                                                                                                                                                          \
        __VA_ARGS__                                                                                                                                            \
    }

#define ACTIONS_FOR_EACH(f)                                                                                                                                    \
    ;                                                                                                                                                          \
    f(actionClone, "Clone", PACK(QStringLiteral("clone")), QLatin1String());                                                                                   \
    f(actionInit, "Init", PACK(QStringLiteral("init")), QLatin1String());                                                                                      \
    f(actionOpen, "Open", PACK(mPath), QLatin1String());                                                                                                       \
    f(actionPull, "Pull", PACK(QStringLiteral("pull"), mPath), QStringLiteral("git-pull"));                                                                    \
    f(actionPush, "Push", PACK(QStringLiteral("push"), mPath), QStringLiteral("git-push"));                                                                    \
    f(actionMerge, "Merge", PACK(QStringLiteral("merge"), mPath), QStringLiteral("git-merge"));                                                                \
    f(actionModifications, "Modifications", PACK(QStringLiteral("changes"), mPath), QStringLiteral("gitklient-changedfiles"));                                 \
    f(actionDiff, "Diff", PACK(QStringLiteral("diff"), mPath), QLatin1String());                                                                               \
    f(actionIgnoreFile, "Ignore file", PACK(QStringLiteral("ignore"), mPath), QStringLiteral("git-ignore"));                                                   \
    f(actionHistory, "History", PACK(QStringLiteral("history"), mPath), QLatin1String());                                                                      \
    f(actionBlame, "Blame", PACK(QStringLiteral("blame"), mPath), QLatin1String());                                                                            \
    f(actionAdd, "Add", PACK(QStringLiteral("add"), mPath), QLatin1String());                                                                                  \
    f(actionRemove, "Remove", PACK(QStringLiteral("remove"), mPath), QLatin1String());                                                                         \
    f(actionCreateTag, "Create tag", PACK(QStringLiteral("create-tag"), mPath), QLatin1String());                                                              \
    f(actionCleanup, "Cleanup", PACK(QStringLiteral("cleanup"), mPath), QLatin1String());

#endif // ACTIONS_H
