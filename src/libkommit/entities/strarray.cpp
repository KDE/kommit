#include "strarray.h"

namespace Git
{

StrArray::StrArray(const QStringList &strings)
{
    setStrings(strings);
}

StrArray::StrArray(const QString &string)
    : StrArray{QStringList{string}}
{
}

void StrArray::setStrings(const QStringList &strings)
{
    mUtf8.clear();
    mPointers.clear();
    mUtf8.reserve(strings.size());
    mPointers.reserve(strings.size());

    for (const auto &string : strings)
        mUtf8 << string.toUtf8();

    // Filled after every byte array is in place: appending to mUtf8 can move what is in it,
    // and a pointer taken before that would be left behind.
    for (auto &bytes : mUtf8)
        mPointers << bytes.data();

    mArray.strings = mPointers.data();
    mArray.count = static_cast<size_t>(mPointers.size());
}

QStringList StrArray::strings() const
{
    QStringList strings;
    strings.reserve(mUtf8.size());

    for (const auto &bytes : mUtf8)
        strings << QString::fromUtf8(bytes);

    return strings;
}

StrArray::operator git_strarray *()
{
    return &mArray;
}

const git_strarray *StrArray::operator*() const
{
    return &mArray;
}

}
