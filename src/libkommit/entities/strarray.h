#pragma once

#include <QString>
#include <QStringList>
#include <cstdlib> // for malloc, realloc, free
#include <cstring> // for strdup
#include <git2.h>

namespace Git
{

class StrArray
{
public:
    explicit StrArray(size_t initialSize = 0);
    explicit StrArray(const QString &item);
    explicit StrArray(const QStringList &list);

    ~StrArray();

    void reserve(size_t size);

    void add(const QString &str);

    void fromQStringList(const QStringList &list);
    QStringList toQStringList() const;

    operator git_strarray *();
    const git_strarray *operator*() const;

    void clear();

private:
    git_strarray strarray;
    size_t capacity;
};

}
