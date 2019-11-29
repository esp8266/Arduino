/*
Delegate.h - An efficient interchangeable C function ptr and C++ std::function delegate
Copyright (c) 2019 Dirk O. Kaar. All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __Delegate_h
#define __Delegate_h

#if defined(ESP8266)
#include <c_types.h>
#elif defined(ESP32)
#include <esp_attr.h>
#else
#define ICACHE_RAM_ATTR
#define IRAM_ATTR
#endif

#include <functional>
#include <cstddef>

namespace detail
{

    template<typename A, typename R, typename... P>
    class DelegatePImpl {
    public:
        using FunPtr = R(*)(A, P...);
        using FunctionType = std::function<R(P...)>;

        DelegatePImpl()
        {
            fn = nullptr;
            obj = {};
        }

        DelegatePImpl(std::nullptr_t)
        {
            fn = nullptr;
            obj = {};
        }

        ~DelegatePImpl()
        {
            if (FUNC == type)
                functional.~FunctionType();
        }

        DelegatePImpl(const DelegatePImpl& del)
        {
            type = del.type;
            if (FUNC == del.type)
            {
                new (&functional) FunctionType();
                functional = del.functional;
            }
            else
            {
                fn = del.fn;
                obj = del.obj;
            }
        }

        DelegatePImpl(DelegatePImpl&& del)
        {
            type = del.type;
            if (FUNC == del.type)
            {
                new (&functional) FunctionType();
                functional = std::move(del.functional);
            }
            else
            {
                fn = del.fn;
                obj = del.obj;
            }
        }

        DelegatePImpl(FunPtr fn, const A& obj)
        {
            type = FP;
            DelegatePImpl::fn = fn;
            DelegatePImpl::obj = obj;
        }

        DelegatePImpl(FunPtr fn, A&& obj)
        {
            type = FP;
            DelegatePImpl::fn = fn;
            DelegatePImpl::obj = std::move(obj);
        }

        template<typename F> DelegatePImpl(const F& functional)
        {
            type = FUNC;
            new (&this->functional) FunctionType();
            DelegatePImpl::functional = functional;
        }

        template<typename F> DelegatePImpl(F&& functional)
        {
            type = FUNC;
            new (&this->functional) FunctionType();
            DelegatePImpl::functional = std::move(functional);
        }

        DelegatePImpl& operator=(const DelegatePImpl& del)
        {
            if (this == &del) return *this;
            if (FUNC == type && FUNC != del.type)
            {
                functional.~FunctionType();
            }
            else if (FUNC != type && FUNC == del.type)
            {
                new (&this->functional) FunctionType();
            }
            type = del.type;
            if (FUNC == del.type)
            {
                functional = del.functional;
            }
            else
            {
                fn = del.fn;
                obj = del.obj;
            }
            return *this;
        }

        DelegatePImpl& operator=(DelegatePImpl&& del)
        {
            if (this == &del) return *this;
            if (FUNC == type && FUNC != del.type)
            {
                functional.~FunctionType();
            }
            else if (FUNC != type && FUNC == del.type)
            {
                new (&this->functional) FunctionType();
            }
            type = del.type;
            if (FUNC == del.type)
            {
                functional = std::move(del.functional);
            }
            else
            {
                fn = del.fn;
                obj = del.obj;
            }
            return *this;
        }

        template<typename F> DelegatePImpl& operator=(const F& functional)
        {
            if (FUNC != type)
            {
                new (&this->functional) FunctionType();
                type = FUNC;
            }
            this->functional = functional;
            return *this;
        }

        template<typename F> DelegatePImpl& operator=(F&& functional)
        {
            if (FUNC != type)
            {
                new (&this->functional) FunctionType();
                type = FUNC;
            }
            this->functional = std::move(functional);
            return *this;
        }

        DelegatePImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            if (FUNC == type)
            {
                functional.~FunctionType();
            }
            type = FP;
            fn = nullptr;
            obj = {};
            return *this;
        }

        operator bool() const
        {
            if (FUNC == type)
            {
                return functional ? true : false;
            }
            else
            {
                return fn;
            }
        }

        R IRAM_ATTR operator()(P... args) const
        {
            if (FUNC == type)
            {
                return functional(std::forward<P...>(args...));
            }
            else
            {
                return fn(obj, std::forward<P...>(args...));
            }
        }

    protected:
        enum { FUNC, FP } type = FP;
        union {
            FunctionType functional;
            struct {
                FunPtr fn;
                A obj;
            };
        };
    };

    template<typename R, typename... P>
    class DelegatePImpl<void, R, P...> {
    public:
        using FunPtr = R(*)(P...);
        using FunctionType = std::function<R(P...)>;

        DelegatePImpl()
        {
            fn = nullptr;
        }

        ~DelegatePImpl()
        {
            if (FUNC == type)
                functional.~FunctionType();
        }

        DelegatePImpl(const DelegatePImpl& del)
        {
            type = del.type;
            if (FUNC == del.type)
            {
                new (&functional) FunctionType();
                functional = del.functional;
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegatePImpl(DelegatePImpl&& del)
        {
            type = del.type;
            if (FUNC == del.type)
            {
                new (&functional) FunctionType();
                functional = std::move(del.functional);
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegatePImpl(FunPtr fn)
        {
            type = FP;
            DelegatePImpl::fn = fn;
        }

        template<typename F> DelegatePImpl(const F& functional)
        {
            type = FUNC;
            new (&this->functional) FunctionType();
            DelegatePImpl::functional = functional;
        }

        template<typename F> DelegatePImpl(F&& functional)
        {
            type = FUNC;
            new (&this->functional) FunctionType();
            DelegatePImpl::functional = std::move(functional);
        }

        DelegatePImpl& operator=(const DelegatePImpl& del)
        {
            if (this == &del) return *this;
            if (FUNC == type && FUNC != del.type)
            {
                functional.~FunctionType();
            }
            else if (FUNC != type && FUNC == del.type)
            {
                new (&this->functional) FunctionType();
            }
            type = del.type;
            if (FUNC == del.type)
            {
                functional = del.functional;
            }
            else
            {
                fn = del.fn;
            }
            return *this;
        }

        DelegatePImpl& operator=(DelegatePImpl&& del)
        {
            if (this == &del) return *this;
            if (FUNC == type && FUNC != del.type)
            {
                functional.~FunctionType();
            }
            else if (FUNC != type && FUNC == del.type)
            {
                new (&this->functional) FunctionType();
            }
            type = del.type;
            if (FUNC == del.type)
            {
                functional = std::move(del.functional);
            }
            else
            {
                fn = del.fn;
            }
            return *this;
        }

        template<typename F> DelegatePImpl& operator=(const F& functional)
        {
            if (FUNC != type)
            {
                new (&this->functional) FunctionType();
                type = FUNC;
            }
            this->functional = functional;
            return *this;
        }

        template<typename F> DelegatePImpl& operator=(F&& functional)
        {
            if (FUNC != type)
            {
                new (&this->functional) FunctionType();
                type = FUNC;
            }
            this->functional = std::move(functional);
            return *this;
        }

        DelegatePImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            if (FUNC == type)
            {
                functional.~FunctionType();
            }
            type = FP;
            fn = nullptr;
            return *this;
        }

        operator bool() const
        {
            if (FUNC == type)
            {
                return functional ? true : false;
            }
            else
            {
                return fn;
            }
        }

        R IRAM_ATTR operator()(P... args) const
        {
            if (FUNC == type)
            {
                return functional(std::forward<P...>(args...));
            }
            else
            {
                return fn(std::forward<P...>(args...));
            }
        }

    protected:
        enum { FUNC, FP } type = FP;
        union {
            FunctionType functional;
            FunPtr fn;
        };
    };

    template<typename A, typename R>
    class DelegateImpl {
    public:
        using FunPtr = R(*)(A);
        using FunctionType = std::function<R()>;

        DelegateImpl()
        {
            fn = nullptr;
            obj = {};
        }

        DelegateImpl(std::nullptr_t)
        {
            fn = nullptr;
            obj = {};
        }

        ~DelegateImpl()
        {
            if (FUNC == type)
                functional.~FunctionType();
        }

        DelegateImpl(const DelegateImpl& del)
        {
            type = del.type;
            if (FUNC == del.type)
            {
                new (&functional) FunctionType();
                functional = del.functional;
            }
            else
            {
                fn = del.fn;
                obj = del.obj;
            }
        }

        DelegateImpl(DelegateImpl&& del)
        {
            type = del.type;
            if (FUNC == del.type)
            {
                new (&functional) FunctionType();
                functional = std::move(del.functional);
            }
            else
            {
                fn = del.fn;
                obj = del.obj;
            }
        }

        DelegateImpl(FunPtr fn, const A& obj)
        {
            type = FP;
            DelegateImpl::fn = fn;
            DelegateImpl::obj = obj;
        }

        DelegateImpl(FunPtr fn, A&& obj)
        {
            type = FP;
            DelegateImpl::fn = fn;
            DelegateImpl::obj = std::move(obj);
        }

        template<typename F> DelegateImpl(const F& functional)
        {
            type = FUNC;
            new (&this->functional) FunctionType();
            DelegateImpl::functional = functional;
        }

        template<typename F> DelegateImpl(F&& functional)
        {
            type = FUNC;
            new (&this->functional) FunctionType();
            DelegateImpl::functional = std::move(functional);
        }

        DelegateImpl& operator=(const DelegateImpl& del)
        {
            if (this == &del) return *this;
            if (FUNC == type && FUNC != del.type)
            {
                functional.~FunctionType();
            }
            else if (FUNC != type && FUNC == del.type)
            {
                new (&this->functional) FunctionType();
            }
            type = del.type;
            if (FUNC == del.type)
            {
                functional = del.functional;
            }
            else
            {
                fn = del.fn;
                obj = del.obj;
            }
            return *this;
        }

        DelegateImpl& operator=(DelegateImpl&& del)
        {
            if (this == &del) return *this;
            if (FUNC == type && FUNC != del.type)
            {
                functional.~FunctionType();
            }
            else if (FUNC != type && FUNC == del.type)
            {
                new (&this->functional) FunctionType();
            }
            type = del.type;
            if (FUNC == del.type)
            {
                functional = std::move(del.functional);
            }
            else
            {
                fn = del.fn;
                obj = del.obj;
            }
            return *this;
        }

        template<typename F> DelegateImpl& operator=(const F& functional)
        {
            if (FUNC != type)
            {
                new (&this->functional) FunctionType();
                type = FUNC;
            }
            this->functional = functional;
            return *this;
        }

        template<typename F> DelegateImpl& operator=(F&& functional)
        {
            if (FUNC != type)
            {
                new (&this->functional) FunctionType();
                type = FUNC;
            }
            this->functional = std::move(functional);
            return *this;
        }

        DelegateImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            if (FUNC == type)
            {
                functional.~FunctionType();
            }
            type = FP;
            fn = nullptr;
            obj = {};
            return *this;
        }

        operator bool() const
        {
            if (FUNC == type)
            {
                return functional ? true : false;
            }
            else
            {
                return fn;
            }
        }

        R IRAM_ATTR operator()() const
        {
            if (FUNC == type)
            {
                return functional();
            }
            else
            {
                return fn(obj);
            }
        }

    protected:
        enum { FUNC, FP } type = FP;
        union {
            FunctionType functional;
            struct {
                FunPtr fn;
                A obj;
            };
        };
    };

    template<typename R>
    class DelegateImpl<void, R> {
    public:
        using FunPtr = R(*)();
        using FunctionType = std::function<R()>;

        DelegateImpl()
        {
            fn = nullptr;
        }

        ~DelegateImpl()
        {
            if (FUNC == type)
                functional.~FunctionType();
        }

        DelegateImpl(const DelegateImpl& del)
        {
            type = del.type;
            if (FUNC == del.type)
            {
                new (&functional) FunctionType();
                functional = del.functional;
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegateImpl(DelegateImpl&& del)
        {
            type = del.type;
            if (FUNC == del.type)
            {
                new (&functional) FunctionType();
                functional = std::move(del.functional);
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegateImpl(FunPtr fn)
        {
            type = FP;
            DelegateImpl::fn = fn;
        }

        template<typename F> DelegateImpl(const F& functional)
        {
            type = FUNC;
            new (&this->functional) FunctionType();
            DelegateImpl::functional = functional;
        }

        template<typename F> DelegateImpl(F&& functional)
        {
            type = FUNC;
            new (&this->functional) FunctionType();
            DelegateImpl::functional = std::move(functional);
        }

        DelegateImpl& operator=(const DelegateImpl& del)
        {
            if (this == &del) return *this;
            if (FUNC == type && FUNC != del.type)
            {
                functional.~FunctionType();
            }
            else if (FUNC != type && FUNC == del.type)
            {
                new (&this->functional) FunctionType();
            }
            type = del.type;
            if (FUNC == del.type)
            {
                functional = del.functional;
            }
            else
            {
                fn = del.fn;
            }
            return *this;
        }

        DelegateImpl& operator=(DelegateImpl&& del)
        {
            if (this == &del) return *this;
            if (FUNC == type && FUNC != del.type)
            {
                functional.~FunctionType();
            }
            else if (FUNC != type && FUNC == del.type)
            {
                new (&this->functional) FunctionType();
            }
            type = del.type;
            if (FUNC == del.type)
            {
                functional = std::move(del.functional);
            }
            else
            {
                fn = del.fn;
            }
            return *this;
        }

        template<typename F> DelegateImpl& operator=(const F& functional)
        {
            if (FUNC != type)
            {
                new (&this->functional) FunctionType();
                type = FUNC;
            }
            this->functional = functional;
            return *this;
        }

        template<typename F> DelegateImpl& operator=(F&& functional)
        {
            if (FUNC != type)
            {
                new (&this->functional) FunctionType();
                type = FUNC;
            }
            this->functional = std::move(functional);
            return *this;
        }

        DelegateImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            if (FUNC == type)
            {
                functional.~FunctionType();
            }
            type = FP;
            fn = nullptr;
            return *this;
        }

        operator bool() const
        {
            if (FUNC == type)
            {
                return functional ? true : false;
            }
            else
            {
                return fn;
            }
        }

        R IRAM_ATTR operator()() const
        {
            if (FUNC == type)
            {
                return functional();
            }
            else
            {
                return fn();
            }
        }

    protected:
        enum { FUNC, FP } type = FP;
        union {
            FunctionType functional;
            FunPtr fn;
        };
    };

    template<typename R = void, typename A = void, typename... P>
    class Delegate : public detail::DelegatePImpl<A, R, P...>
    {
        using detail::DelegatePImpl<A, R, P...>::DelegatePImpl;
    };

    template<typename R, typename A>
    class Delegate<R, A> : public detail::DelegateImpl<A, R>
    {
        using detail::DelegateImpl<A, R>::DelegateImpl;
    };

}

template<typename R, typename A = void, typename... P> class Delegate;
template<typename R, typename A, typename... P> class Delegate<R(P...), A> : public detail::Delegate<R, A, P...>
{
    using detail::Delegate<R, A, P...>::Delegate;
};
template<typename R, typename... P> class Delegate<R(P...)> : public detail::Delegate<R, void, P...>
{
    using detail::Delegate<R, void, P...>::Delegate;
};

#endif // __Delegate_h
