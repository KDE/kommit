#pragma once

#include <QSharedPointer>

template<class T>
class AbstractCache
{
public:
    AbstractCache();

    void clear();

    int count() const;

    template<typename... Args>
    QSharedPointer<T> findOrCreate(const QString &key, Args... args);

private:
    QMap<QString, QSharedPointer<T>> mData;
};

template<class T>
template<typename... Args>
inline QSharedPointer<T> AbstractCache<T>::findOrCreate(const QString &key, Args... args)
{
    if (mData.contains(key))
        return mData.value(key);

    auto en = QSharedPointer<T>{new T{args...}};
    mData.insert(key, en);
    return en;
}

template<class T>
Q_OUTOFLINE_TEMPLATE void AbstractCache<T>::clear()
{
    mData.clear();
}

template<class T>
Q_OUTOFLINE_TEMPLATE int AbstractCache<T>::count() const
{
    return mData.size();
}
