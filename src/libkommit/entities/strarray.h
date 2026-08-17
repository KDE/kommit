#pragma once

#include <QByteArray>
#include <QList>
#include <QString>
#include <QStringList>

#include <git2.h>

namespace Git
{

/**
 * A list of strings in the form libgit2 reads them, a git_strarray of UTF-8 pointers.
 *
 * It holds the UTF-8 bytes of each string, and the pointers into those bytes, for as long as
 * the call that reads them lasts. Qt owns the memory, so there is nothing here to allocate or
 * free by hand.
 */
class StrArray
{
public:
    explicit StrArray(const QStringList &strings = {});
    explicit StrArray(const QString &string);

    // What libgit2 is given points into this object, so it stays where it was made.
    StrArray(const StrArray &) = delete;
    StrArray &operator=(const StrArray &) = delete;

    void setStrings(const QStringList &strings);
    [[nodiscard]] QStringList strings() const;

    explicit operator git_strarray *();
    const git_strarray *operator*() const;

private:
    QList<QByteArray> mUtf8;
    QList<char *> mPointers;
    git_strarray mArray{nullptr, 0};
};

}
