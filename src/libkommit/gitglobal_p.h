/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QDebug>
#include <QSharedPointer>
#include <git2/errors.h>
#include <type_traits>

#define LIBGIT_ERR_CHECK_VARIABLE __err

#define BEGIN int LIBGIT_ERR_CHECK_VARIABLE = 0;
#define RESTART LIBGIT_ERR_CHECK_VARIABLE = 0;
#define STEP LIBGIT_ERR_CHECK_VARIABLE = LIBGIT_ERR_CHECK_VARIABLE ? LIBGIT_ERR_CHECK_VARIABLE:

#ifdef QDEBUG_H
#define END                                                                                                                                                    \
    do {                                                                                                                                                       \
        if (LIBGIT_ERR_CHECK_VARIABLE) {                                                                                                                       \
            auto __git_err = git_error_last();                                                                                                                 \
            auto __git_err_class = __git_err->klass;                                                                                                           \
            auto __git_err_msg = QString{__git_err->message};                                                                                                  \
            qDebug().noquote().nospace() << "libgit2 error: " << LIBGIT_ERR_CHECK_VARIABLE << ", class: " << __git_err_class                                   \
                                         << ", Message: " << __git_err_msg;                                                                                    \
        }                                                                                                                                                      \
    } while (false)
#else
#define END
#endif

#define PRINT_ERROR END
#define RETURN_COND(ok, error)                                                                                                                                 \
    do {                                                                                                                                                       \
        return LIBGIT_ERR_CHECK_VARIABLE ? error : ok;                                                                                                         \
    } while (false)

#define RETURN_IF_ERR(value)                                                                                                                                   \
    do {                                                                                                                                                       \
        if (LIBGIT_ERR_CHECK_VARIABLE)                                                                                                                         \
            return value;                                                                                                                                      \
    } while (false)

#define RETURN_IF_ERR_BOOL                                                                                                                                     \
    do {                                                                                                                                                       \
        if (LIBGIT_ERR_CHECK_VARIABLE)                                                                                                                         \
            return false;                                                                                                                                      \
    } while (false)

#define RETURN_IF_ERR_VOID                                                                                                                                     \
    do {                                                                                                                                                       \
        if (LIBGIT_ERR_CHECK_VARIABLE)                                                                                                                         \
            return;                                                                                                                                            \
    } while (false)

#define IS_OK !LIBGIT_ERR_CHECK_VARIABLE
#define IS_ERROR LIBGIT_ERR_CHECK_VARIABLE

class SequenceRunner
{
    int _n{0};
    bool _printError{true};

public:
    inline constexpr explicit SequenceRunner(bool printError = true)
        : _printError{printError}
    {
    }

    inline ~SequenceRunner()
    {
        printError();
    }

    template<typename Func, typename... Args>
    inline int run(Func *func, Args &&...args)
    {
        if (!_n) {
            _n = func(std::forward<Args>(args)...);
            if (_n) {
                printError();
            }
        }
        return _n;
    }

    template<typename Func, typename... Args>
    static bool runSingle(Func *func, Args &&...args)
    {
        int n = func(std::forward<Args>(args)...);
        if (n) {
            auto __git_err = git_error_last();
            auto __git_err_class = __git_err->klass;
            auto __git_err_msg = QString{__git_err->message};
            qDebug().noquote().nospace() << "libgit2 error: " << n << ", class: " << __git_err_class << ", Message: " << __git_err_msg;
        }

        return !n;
    }

    inline bool isSuccess()
    {
        printError();
        return !_n;
    }

    inline bool isError()
    {
        printError();
        return _n;
    }

    inline void printError()
    {
        if (_n && _printError) {
            auto _git_err = git_error_last();
            auto _git_err_class = _git_err->klass;
            auto _git_err_msg = QString{_git_err->message};
            qDebug().noquote().nospace() << "libgit2 error: " << _n << ", class: " << _git_err_class << ", Message: " << _git_err_msg;
            _printError = false;
        }
    }
};

template<typename T>
class Peeler
{
public:
    constexpr explicit Peeler(T *initialObj)
        : ptr(initialObj)
        , sharedPtr(nullptr)
    {
    }

    constexpr explicit Peeler(QSharedPointer<T> initialObj)
        : ptr(nullptr)
        , sharedPtr(initialObj)
    {
    }

    template<typename Method, typename Ret = decltype((std::declval<T>().*std::declval<Method>())()), std::enable_if_t<std::is_pointer_v<Ret>, int> = 0>
    constexpr auto operator()(Method method) const -> Peeler<std::remove_pointer_t<Ret>>
    {
        if (isNull()) {
            return Peeler<std::remove_pointer_t<Ret>>(nullptr);
        }

        auto result = (get()->*method)();
        return Peeler<std::remove_pointer_t<Ret>>(result);
    }

    template<typename Method,
             typename Ret = decltype((std::declval<T>().*std::declval<Method>())()),
             std::enable_if_t<std::is_same_v<Ret, QSharedPointer<std::remove_pointer_t<decltype(Ret().data())>>>, int> = 0>
    constexpr auto operator()(Method method) const -> Peeler<std::remove_pointer_t<decltype(Ret().data())>>
    {
        if (isNull()) {
            return Peeler<std::remove_pointer_t<decltype(Ret().data())>>(nullptr);
        }

        auto result = (get()->*method)();
        return Peeler<std::remove_pointer_t<decltype(result.data())>>(result);
    }

    constexpr T *get() const
    {
        return ptr ? ptr : sharedPtr.data();
    }
    constexpr auto operator()() const -> T *
    {
        return ptr ? ptr : sharedPtr.data();
    }

    [[nodiscard]] constexpr bool isNull() const
    {
        return (!ptr && sharedPtr.isNull());
    }

private:
    T *ptr;
    QSharedPointer<T> sharedPtr;
};
