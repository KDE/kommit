#include "strarray.h"

namespace Git
{

StrArray::StrArray(size_t initialSize)
    : capacity{}
{
    char **newStrings = static_cast<char **>(malloc(initialSize * sizeof(char *)));
    if (newStrings) {
        strarray.count = initialSize;
        strarray.strings = newStrings;
    }
}

StrArray::StrArray(const QString &item)
{
    strarray.strings = new char *[1];
    strarray.strings[0] = strdup(item.toUtf8().constData());
    strarray.count = 1;
}

StrArray::StrArray(const QStringList &list)
{
    fromQStringList(list);
}

StrArray::~StrArray()
{
    clear();
}

void StrArray::reserve(size_t size)
{
    if (size > capacity) {
        char **newStrings = static_cast<char **>(realloc(strarray.strings, size * sizeof(char *)));
        if (newStrings) {
            strarray.count = size;
            strarray.strings = newStrings;
            capacity = size;
        }
    }
}

void StrArray::add(const QString &str)
{
    if (strarray.count < capacity) {
        reserve(strarray.count + 1); // Reserve exactly the needed size
    }
    strarray.strings[strarray.count] = strdup(str.toUtf8().constData());
    capacity++;
}

void StrArray::fromQStringList(const QStringList &list)
{
    clear();
    reserve(list.size());
    for (int i = 0; i < list.size(); ++i) {
        add(list[i]);
    }
}

QStringList StrArray::toQStringList() const
{
    QStringList list;
    for (size_t i = 0; i < strarray.count; ++i) {
        list.append(QString::fromUtf8(strarray.strings[i]));
    }
    return list;
}

StrArray::operator git_strarray *()
{
    return &strarray;
}

const git_strarray *StrArray::operator&() const
{
    return &strarray;
}

const git_strarray StrArray::operator*() const
{
    return strarray;
}

void StrArray::clear()
{
    if (strarray.strings) {
        for (size_t i = 0; i < strarray.count; ++i) {
            free(strarray.strings[i]);
        }
        free(strarray.strings);
        strarray.strings = nullptr;
        strarray.count = 0;
        capacity = 0;
    }
}

}
