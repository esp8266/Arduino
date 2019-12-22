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

#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
#include <functional>
#include <cstddef>
#else
#include "circular_queue/ghostl.h"
#endif

namespace detail
{
    template<typename R, typename... P>
    static R IRAM_ATTR vPtrToFunPtrExec(void* fn, P... args)
    {
        using target_type = R(P...);
        return reinterpret_cast<target_type*>(fn)(std::forward<P...>(args...));
    }

#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
    template<typename A, typename R, typename... P>
    class DelegatePImpl {
    public:
        using target_type = R(P...);
    protected:
        using FunPtr = target_type*;
        using FunAPtr = R(*)(A, P...);
        using FunVPPtr = R(*)(void*, P...);
        using FunctionType = std::function<target_type>;
    public:
        DelegatePImpl()
        {
            fn = nullptr;
        }

        DelegatePImpl(std::nullptr_t)
        {
            fn = nullptr;
        }

        ~DelegatePImpl()
        {
            if (FUNC == kind)
                functional.~FunctionType();
            else if (FPA == kind)
                obj.~A();
        }

        DelegatePImpl(const DelegatePImpl& del)
        {
            kind = del.kind;
            if (FUNC == del.kind)
            {
                new (&functional) FunctionType(del.functional);
            }
            else if (FPA == del.kind)
            {
                fnA = del.fnA;
                new (&obj) A(del.obj);
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegatePImpl(DelegatePImpl&& del)
        {
            kind = del.kind;
            if (FUNC == del.kind)
            {
                new (&functional) FunctionType(std::move(del.functional));
            }
            else if (FPA == del.kind)
            {
                fnA = del.fnA;
                new (&obj) A(std::move(del.obj));
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegatePImpl(FunAPtr fnA, const A& obj)
        {
            kind = FPA;
            DelegatePImpl::fnA = fnA;
            new (&this->obj) A(obj);
        }

        DelegatePImpl(FunAPtr fnA, A&& obj)
        {
            kind = FPA;
            DelegatePImpl::fnA = fnA;
            new (&this->obj) A(std::move(obj));
        }

        DelegatePImpl(FunPtr fn)
        {
            kind = FP;
            DelegatePImpl::fn = fn;
        }

        template<typename F> DelegatePImpl(const F& functional)
        {
            kind = FUNC;
            new (&this->functional) FunctionType(functional);
        }

        template<typename F> DelegatePImpl(F&& functional)
        {
            kind = FUNC;
            new (&this->functional) FunctionType(std::move(functional));
        }

        DelegatePImpl& operator=(const DelegatePImpl& del)
        {
            if (this == &del) return *this;
            if (kind != del.kind)
            {
                if (FUNC == kind)
                {
                    functional.~FunctionType();
                }
                else if (FPA == kind)
                {
                    obj.~A();
                }
                if (FUNC == del.kind)
                {
                    new (&this->functional) FunctionType();
                }
                else if (FPA == del.kind)
                {
                    new (&obj) A;
                }
                kind = del.kind;
            }
            if (FUNC == del.kind)
            {
                functional = del.functional;
            }
            else if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = del.obj;
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
            if (kind != del.kind)
            {
                if (FUNC == kind)
                {
                    functional.~FunctionType();
                }
                else if (FPA == kind)
                {
                    obj.~A();
                }
                if (FUNC == del.kind)
                {
                    new (&this->functional) FunctionType();
                }
                else if (FPA == del.kind)
                {
                    new (&obj) A;
                }
                kind = del.kind;
            }
            if (FUNC == del.kind)
            {
                functional = std::move(del.functional);
            }
            else if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = std::move(del.obj);
            }
            else
            {
                fn = del.fn;
            }
            return *this;
        }

        DelegatePImpl& operator=(FunPtr fn)
        {
            if (FUNC == kind)
            {
                functional.~FunctionType();
            }
            else if (FPA == kind)
            {
                obj.~A();
            }
            kind = FP;
            this->fn = fn;
            return *this;
        }

        template<typename F> DelegatePImpl& operator=(const F& functional)
        {
            if (FUNC != kind)
            {
                if (FPA == kind)
                {
                    obj.~A();
                }
                new (&this->functional) FunctionType();
                kind = FUNC;
            }
            this->functional = functional;
            return *this;
        }

        template<typename F> DelegatePImpl& operator=(F&& functional)
        {
            if (FUNC != kind)
            {
                if (FPA == kind)
                {
                    obj.~A();
                }
                new (&this->functional) FunctionType();
                kind = FUNC;
            }
            this->functional = std::move(functional);
            return *this;
        }

        DelegatePImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            if (FUNC == kind)
            {
                functional.~FunctionType();
            }
            else if (FPA == kind)
            {
                obj.~A();
            }
            kind = FP;
            fn = nullptr;
            return *this;
        }

        operator bool() const
        {
            if (FP == kind)
            {
                return fn;
            }
            else if (FPA == kind)
            {
                return fnA;
            }
            else
            {
                return functional ? true : false;
            }
        }

        static R IRAM_ATTR vPtrToFunAPtrExec(void* self, P... args)
        {
            return static_cast<DelegatePImpl*>(self)->fnA(
                static_cast<DelegatePImpl*>(self)->obj,
                std::forward<P...>(args...));
        };

        operator FunVPPtr() const
        {
            if (FP == kind)
            {
                return vPtrToFunPtrExec<R, P...>;
            }
            else if (FPA == kind)
            {
                return vPtrToFunAPtrExec;
            }
            else
            {
                return [](void* self, P... args) -> R
                {
                    return static_cast<DelegatePImpl*>(self)->functional(std::forward<P...>(args...));
                };
            }
        }

        void* arg() const
        {
            if (FP == kind)
            {
                return reinterpret_cast<void*>(fn);
            }
            else
            {
                return const_cast<DelegatePImpl*>(this);
            }
        }

        operator FunctionType() const
        {
            if (FP == kind)
            {
                return fn;
            }
            else if (FPA == kind)
            {
                return [this](P... args) { return fnA(obj, std::forward<P...>(args...)); };
            }
            else
            {
                return functional;
            }
        }

        R IRAM_ATTR operator()(P... args) const
        {
            if (FP == kind)
            {
                return fn(std::forward<P...>(args...));
            }
            else if (FPA == kind)
            {
                return fnA(obj, std::forward<P...>(args...));
            }
            else
            {
                return functional(std::forward<P...>(args...));
            }
        }

    protected:
        enum { FUNC, FP, FPA } kind = FP;
        union {
            FunctionType functional;
            FunPtr fn;
            struct {
                FunAPtr fnA;
                A obj;
            };
        };
    };
#else
    template<typename A, typename R, typename... P>
    class DelegatePImpl {
    public:
        using target_type = R(P...);
    protected:
        using FunPtr = target_type*;
        using FunAPtr = R(*)(A, P...);
        using FunVPPtr = R(*)(void*, P...);
    public:
        DelegatePImpl()
        {
            fn = nullptr;
        }

        DelegatePImpl(std::nullptr_t)
        {
            fn = nullptr;
        }

        DelegatePImpl(const DelegatePImpl& del)
        {
            kind = del.kind;
            if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = del.obj;
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegatePImpl(DelegatePImpl&& del)
        {
            kind = del.kind;
            if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = std::move(del.obj);
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegatePImpl(FunAPtr fnA, const A& obj)
        {
            kind = FPA;
            DelegatePImpl::fnA = fnA;
            this->obj = obj;
        }

        DelegatePImpl(FunAPtr fnA, A&& obj)
        {
            kind = FPA;
            DelegatePImpl::fnA = fnA;
            this->obj = std::move(obj);
        }

        DelegatePImpl(FunPtr fn)
        {
            kind = FP;
            DelegatePImpl::fn = fn;
        }

        template<typename F> DelegatePImpl(const F& fn)
        {
            kind = FP;
            DelegatePImpl::fn = fn;
        }

        DelegatePImpl& operator=(const DelegatePImpl& del)
        {
            if (this == &del) return *this;
            if (kind != del.kind)
            {
                if (FPA == kind)
                {
                    obj = {};
                }
                kind = del.kind;
            }
            if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = del.obj;
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
            if (kind != del.kind)
            {
                if (FPA == kind)
                {
                    obj = {};
                }
                kind = del.kind;
            }
            if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = std::move(del.obj);
            }
            else
            {
                fn = del.fn;
            }
            return *this;
        }

        DelegatePImpl& operator=(FunPtr fn)
        {
            if (FPA == kind)
            {
                obj = {};
            }
            kind = FP;
            this->fn = fn;
            return *this;
        }

        template<typename F> DelegatePImpl& operator=(const F& fn)
        {
            if (FPA == kind)
            {
                obj = {};
            }
            kind = FP;
            this->fn = fn;
            return *this;
        }

        DelegatePImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            if (FPA == kind)
            {
                obj = {};
            }
            kind = FP;
            fn = nullptr;
            return *this;
        }

        operator bool() const
        {
            if (FP == kind)
            {
                return fn;
            }
            else
            {
                return fnA;
            }
        }

        static R IRAM_ATTR vPtrToFunAPtrExec(void* self, P... args)
        {
            return static_cast<DelegatePImpl*>(self)->fnA(
                static_cast<DelegatePImpl*>(self)->obj,
                std::forward<P...>(args...));
        };

        operator FunVPPtr() const
        {
            if (FP == kind)
            {
                return vPtrToFunPtrExec<R, P...>;
            }
            else
            {
                return vPtrToFunAPtrExec;
            }
        }

        void* arg() const
        {
            if (FP == kind)
            {
                return reinterpret_cast<void*>(fn);
            }
            else
            {
                return const_cast<DelegatePImpl*>(this);
            }
        }

        R IRAM_ATTR operator()(P... args) const
        {
            if (FP == kind)
            {
                return fn(std::forward<P...>(args...));
            }
            else
            {
                return fnA(obj, std::forward<P...>(args...));
            }
        }

    protected:
        enum { FP, FPA } kind = FP;
        union {
            FunPtr fn;
            FunAPtr fnA;
        };
        A obj;
    };
#endif

#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
    template<typename R, typename... P>
    class DelegatePImpl<void, R, P...> {
    public:
        using target_type = R(P...);
    protected:
        using FunPtr = target_type*;
        using FunctionType = std::function<target_type>;
        using FunVPPtr = R(*)(void*, P...);
    public:
        DelegatePImpl()
        {
            fn = nullptr;
        }

        DelegatePImpl(std::nullptr_t)
        {
            fn = nullptr;
        }

        ~DelegatePImpl()
        {
            if (FUNC == kind)
                functional.~FunctionType();
        }

        DelegatePImpl(const DelegatePImpl& del)
        {
            kind = del.kind;
            if (FUNC == del.kind)
            {
                new (&functional) FunctionType(del.functional);
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegatePImpl(DelegatePImpl&& del)
        {
            kind = del.kind;
            if (FUNC == del.kind)
            {
                new (&functional) FunctionType(std::move(del.functional));
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegatePImpl(FunPtr fn)
        {
            kind = FP;
            DelegatePImpl::fn = fn;
        }

        template<typename F> DelegatePImpl(const F& functional)
        {
            kind = FUNC;
            new (&this->functional) FunctionType(functional);
        }

        template<typename F> DelegatePImpl(F&& functional)
        {
            kind = FUNC;
            new (&this->functional) FunctionType(std::move(functional));
        }

        DelegatePImpl& operator=(const DelegatePImpl& del)
        {
            if (this == &del) return *this;
            if (FUNC == kind && FUNC != del.kind)
            {
                functional.~FunctionType();
            }
            else if (FUNC != kind && FUNC == del.kind)
            {
                new (&this->functional) FunctionType();
            }
            kind = del.kind;
            if (FUNC == del.kind)
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
            if (FUNC == kind && FUNC != del.kind)
            {
                functional.~FunctionType();
            }
            else if (FUNC != kind && FUNC == del.kind)
            {
                new (&this->functional) FunctionType();
            }
            kind = del.kind;
            if (FUNC == del.kind)
            {
                functional = std::move(del.functional);
            }
            else
            {
                fn = del.fn;
            }
            return *this;
        }

        DelegatePImpl& operator=(FunPtr fn)
        {
            if (FUNC == kind)
            {
                functional.~FunctionType();
                kind = FP;
            }
            DelegatePImpl::fn = fn;
            return *this;
        }

        template<typename F> DelegatePImpl& operator=(const F& functional)
        {
            if (FUNC != kind)
            {
                new (&this->functional) FunctionType();
                kind = FUNC;
            }
            this->functional = functional;
            return *this;
        }

        template<typename F> DelegatePImpl& operator=(F&& functional)
        {
            if (FUNC != kind)
            {
                new (&this->functional) FunctionType();
                kind = FUNC;
            }
            this->functional = std::move(functional);
            return *this;
        }

        DelegatePImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            if (FUNC == kind)
            {
                functional.~FunctionType();
            }
            kind = FP;
            fn = nullptr;
            return *this;
        }

        operator bool() const
        {
            if (FP == kind)
            {
                return fn;
            }
            else
            {
                return functional ? true : false;
            }
        }

        operator FunVPPtr() const
        {
            if (FP == kind)
            {
                return vPtrToFunPtrExec<R, P...>;
            }
            else
            {
                return [](void* self, P... args) -> R
                {
                    return static_cast<DelegatePImpl*>(self)->functional(std::forward<P...>(args...));
                };
            }
        }

        void* arg() const
        {
            if (FP == kind)
            {
                return reinterpret_cast<void*>(fn);
            }
            else
            {
                return const_cast<DelegatePImpl*>(this);
            }
        }

        operator FunctionType() const
        {
            if (FP == kind)
            {
                return fn;
            }
            else
            {
                return functional;
            }
        }

        R IRAM_ATTR operator()(P... args) const
        {
            if (FP == kind)
            {
                return fn(std::forward<P...>(args...));
            }
            else
            {
                return functional(std::forward<P...>(args...));
            }
        }

    protected:
        enum { FUNC, FP } kind = FP;
        union {
            FunctionType functional;
            FunPtr fn;
        };
    };
#else
    template<typename R, typename... P>
    class DelegatePImpl<void, R, P...> {
    public:
        using target_type = R(P...);
    protected:
        using FunPtr = target_type*;
        using FunVPPtr = R(*)(void*, P...);
    public:
        DelegatePImpl()
        {
            fn = nullptr;
        }

        DelegatePImpl(std::nullptr_t)
        {
            fn = nullptr;
        }

        DelegatePImpl(const DelegatePImpl& del)
        {
            fn = del.fn;
        }

        DelegatePImpl(DelegatePImpl&& del)
        {
            fn = std::move(del.fn);
        }

        DelegatePImpl(FunPtr fn)
        {
            DelegatePImpl::fn = fn;
        }

        template<typename F> DelegatePImpl(const F& fn)
        {
            DelegatePImpl::fn = fn;
        }

        DelegatePImpl& operator=(const DelegatePImpl& del)
        {
            if (this == &del) return *this;
            fn = del.fn;
            return *this;
        }

        DelegatePImpl& operator=(DelegatePImpl&& del)
        {
            if (this == &del) return *this;
            fn = std::move(del.fn);
            return *this;
        }

        DelegatePImpl& operator=(FunPtr fn)
        {
            DelegatePImpl::fn = fn;
            return *this;
        }

        template<typename F> DelegatePImpl& operator=(const F& fn)
        {
            DelegatePImpl::fn = fn;
            return *this;
        }

        DelegatePImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            fn = nullptr;
            return *this;
        }

        operator bool() const
        {
            return fn;
        }

        operator FunVPPtr() const
        {
            return vPtrToFunPtrExec<R, P...>;
        }

        void* arg() const
        {
            return reinterpret_cast<void*>(fn);
        }

        R IRAM_ATTR operator()(P... args) const
        {
            return fn(std::forward<P...>(args...));
        }

    protected:
        FunPtr fn;
    };
#endif

#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
    template<typename A, typename R>
    class DelegateImpl {
    public:
        using target_type = R();
    protected:
        using FunPtr = target_type*;
        using FunAPtr = R(*)(A);
        using FunctionType = std::function<target_type>;
        using FunVPPtr = R(*)(void*);
    public:
        DelegateImpl()
        {
            fn = nullptr;
        }

        DelegateImpl(std::nullptr_t)
        {
            fn = nullptr;
        }

        ~DelegateImpl()
        {
            if (FUNC == kind)
                functional.~FunctionType();
            else if (FPA == kind)
                obj.~A();
        }

        DelegateImpl(const DelegateImpl& del)
        {
            kind = del.kind;
            if (FUNC == del.kind)
            {
                new (&functional) FunctionType(del.functional);
            }
            else if (FPA == del.kind)
            {
                fnA = del.fnA;
                new (&obj) A(del.obj);
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegateImpl(DelegateImpl&& del)
        {
            kind = del.kind;
            if (FUNC == del.kind)
            {
                new (&functional) FunctionType(std::move(del.functional));
            }
            else if (FPA == del.kind)
            {
                fnA = del.fnA;
                new (&obj) A(std::move(del.obj));
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegateImpl(FunAPtr fnA, const A& obj)
        {
            kind = FPA;
            DelegateImpl::fnA = fnA;
            new (&this->obj) A(obj);
        }

        DelegateImpl(FunAPtr fnA, A&& obj)
        {
            kind = FPA;
            DelegateImpl::fnA = fnA;
            new (&this->obj) A(std::move(obj));
        }

        DelegateImpl(FunPtr fn)
        {
            kind = FP;
            DelegateImpl::fn = fn;
        }

        template<typename F> DelegateImpl(const F& functional)
        {
            kind = FUNC;
            new (&this->functional) FunctionType(functional);
        }

        template<typename F> DelegateImpl(F&& functional)
        {
            kind = FUNC;
            new (&this->functional) FunctionType(std::move(functional));
        }

        DelegateImpl& operator=(const DelegateImpl& del)
        {
            if (this == &del) return *this;
            if (kind != del.kind)
            {
                if (FUNC == kind)
                {
                    functional.~FunctionType();
                }
                else if (FPA == kind)
                {
                    obj.~A();
                }
                if (FUNC == del.kind)
                {
                    new (&this->functional) FunctionType();
                }
                else if (FPA == del.kind)
                {
                    new (&obj) A;
                }
                kind = del.kind;
            }
            if (FUNC == del.kind)
            {
                functional = del.functional;
            }
            else if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = del.obj;
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
            if (kind != del.kind)
            {
                if (FUNC == kind)
                {
                    functional.~FunctionType();
                }
                else if (FPA == kind)
                {
                    obj.~A();
                }
                if (FUNC == del.kind)
                {
                    new (&this->functional) FunctionType();
                }
                else if (FPA == del.kind)
                {
                    new (&obj) A;
                }
                kind = del.kind;
            }
            if (FUNC == del.kind)
            {
                functional = std::move(del.functional);
            }
            else if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = std::move(del.obj);
            }
            else
            {
                fn = del.fn;
            }
            return *this;
        }

        DelegateImpl& operator=(FunPtr fn)
        {
            if (FUNC == kind)
            {
                functional.~FunctionType();
            }
            else if (FPA == kind)
            {
                obj.~A();
            }
            kind = FP;
            this->fn = fn;
            return *this;
        }

        template<typename F> DelegateImpl& operator=(const F& functional)
        {
            if (FUNC != kind)
            {
                if (FPA == kind)
                {
                    obj.~A();
                }
                new (&this->functional) FunctionType();
                kind = FUNC;
            }
            this->functional = functional;
            return *this;
        }

        template<typename F> DelegateImpl& operator=(F&& functional)
        {
            if (FUNC != kind)
            {
                if (FPA == kind)
                {
                    obj.~A();
                }
                new (&this->functional) FunctionType();
                kind = FUNC;
            }
            this->functional = std::move(functional);
            return *this;
        }

        DelegateImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            if (FUNC == kind)
            {
                functional.~FunctionType();
            }
            else if (FPA == kind)
            {
                obj.~A();
            }
            kind = FP;
            fn = nullptr;
            return *this;
        }

        operator bool() const
        {
            if (FP == kind)
            {
                return fn;
            }
            else if (FPA == kind)
            {
                return fnA;
            }
            else
            {
                return functional ? true : false;
            }
        }

        static R IRAM_ATTR vPtrToFunAPtrExec(void* self)
        {
            return static_cast<DelegateImpl*>(self)->fnA(
                static_cast<DelegateImpl*>(self)->obj);
        };

        operator FunVPPtr() const
        {
            if (FP == kind)
            {
                return reinterpret_cast<FunVPPtr>(fn);
            }
            else if (FPA == kind)
            {
                return vPtrToFunAPtrExec;
            }
            else
            {
                return [](void* self) -> R
                {
                    return static_cast<DelegateImpl*>(self)->functional();
                };
            }
        }

        void* arg() const
        {
            if (FP == kind)
            {
                return nullptr;
            }
            else
            {
                return const_cast<DelegateImpl*>(this);
            }
        }

        operator FunctionType() const
        {
            if (FP == kind)
            {
                return fn;
            }
            else if (FPA == kind)
            {
                return [this]() { return fnA(obj); };
            }
            else
            {
                return functional;
            }
        }

        R IRAM_ATTR operator()() const
        {
            if (FP == kind)
            {
                return fn();
            }
            else if (FPA == kind)
            {
                return fnA(obj);
            }
            else
            {
                return functional();
            }
        }

    protected:
        enum { FUNC, FP, FPA } kind = FP;
        union {
            FunctionType functional;
            FunPtr fn;
            struct {
                FunAPtr fnA;
                A obj;
            };
        };
    };
#else
    template<typename A, typename R>
    class DelegateImpl {
    public:
        using target_type = R();
    protected:
        using FunPtr = target_type*;
        using FunAPtr = R(*)(A);
        using FunVPPtr = R(*)(void*);
    public:
        DelegateImpl()
        {
            fn = nullptr;
        }

        DelegateImpl(std::nullptr_t)
        {
            fn = nullptr;
        }

        DelegateImpl(const DelegateImpl& del)
        {
            kind = del.kind;
            if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = del.obj;
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegateImpl(DelegateImpl&& del)
        {
            kind = del.kind;
            if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = std::move(del.obj);
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegateImpl(FunAPtr fnA, const A& obj)
        {
            kind = FPA;
            DelegateImpl::fnA = fnA;
            this->obj = obj;
        }

        DelegateImpl(FunAPtr fnA, A&& obj)
        {
            kind = FPA;
            DelegateImpl::fnA = fnA;
            this->obj = std::move(obj);
        }

        DelegateImpl(FunPtr fn)
        {
            kind = FP;
            DelegateImpl::fn = fn;
        }

        template<typename F> DelegateImpl(const F& fn)
        {
            kind = FP;
            DelegateImpl::fn = fn;
        }

        DelegateImpl& operator=(const DelegateImpl& del)
        {
            if (this == &del) return *this;
            if (kind != del.kind)
            {
                if (FPA == kind)
                {
                    obj = {};
                }
                kind = del.kind;
            }
            if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = del.obj;
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
            if (kind != del.kind)
            {
                if (FPA == kind)
                {
                    obj = {};
                }
                kind = del.kind;
            }
            if (FPA == del.kind)
            {
                fnA = del.fnA;
                obj = std::move(del.obj);
            }
            else
            {
                fn = del.fn;
            }
            return *this;
        }

        DelegateImpl& operator=(FunPtr fn)
        {
            if (FPA == kind)
            {
                obj = {};
            }
            kind = FP;
            this->fn = fn;
            return *this;
        }

        template<typename F> DelegateImpl& operator=(const F& fn)
        {
            if (FPA == kind)
            {
                obj = {};
            }
            kind = FP;
            this->fn = fn;
            return *this;
        }

        DelegateImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            if (FPA == kind)
            {
                obj = {};
            }
            kind = FP;
            fn = nullptr;
            return *this;
        }

        operator bool() const
        {
            if (FP == kind)
            {
                return fn;
            }
            else
            {
                return fnA;
            }
        }

        static R IRAM_ATTR vPtrToFunAPtrExec(void* self)
        {
            return static_cast<DelegateImpl*>(self)->fnA(
                static_cast<DelegateImpl*>(self)->obj);
        };

        operator FunVPPtr() const
        {
            if (FP == kind)
            {
                return reinterpret_cast<FunVPPtr>(fn);
            }
            else
            {
                return vPtrToFunAPtrExec;
            }
        }

        void* arg() const
        {
            if (FP == kind)
            {
                return nullptr;
            }
            else
            {
                return const_cast<DelegateImpl*>(this);
            }
        }

        R IRAM_ATTR operator()() const
        {
            if (FP == kind)
            {
                return fn();
            }
            else
            {
                return fnA(obj);
            }
        }

    protected:
        enum { FP, FPA } kind = FP;
        union {
            FunPtr fn;
            FunAPtr fnA;
        };
        A obj;
    };
#endif

#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
    template<typename R>
    class DelegateImpl<void, R> {
    public:
        using target_type = R();
    protected:
        using FunPtr = target_type*;
        using FunctionType = std::function<target_type>;
        using FunVPPtr = R(*)(void*);
    public:
        DelegateImpl()
        {
            fn = nullptr;
        }

        DelegateImpl(std::nullptr_t)
        {
            fn = nullptr;
        }

        ~DelegateImpl()
        {
            if (FUNC == kind)
                functional.~FunctionType();
        }

        DelegateImpl(const DelegateImpl& del)
        {
            kind = del.kind;
            if (FUNC == del.kind)
            {
                new (&functional) FunctionType(del.functional);
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegateImpl(DelegateImpl&& del)
        {
            kind = del.kind;
            if (FUNC == del.kind)
            {
                new (&functional) FunctionType(std::move(del.functional));
            }
            else
            {
                fn = del.fn;
            }
        }

        DelegateImpl(FunPtr fn)
        {
            kind = FP;
            DelegateImpl::fn = fn;
        }

        template<typename F> DelegateImpl(const F& functional)
        {
            kind = FUNC;
            new (&this->functional) FunctionType(functional);
        }

        template<typename F> DelegateImpl(F&& functional)
        {
            kind = FUNC;
            new (&this->functional) FunctionType(std::move(functional));
        }

        DelegateImpl& operator=(const DelegateImpl& del)
        {
            if (this == &del) return *this;
            if (FUNC == kind && FUNC != del.kind)
            {
                functional.~FunctionType();
            }
            else if (FUNC != kind && FUNC == del.kind)
            {
                new (&this->functional) FunctionType();
            }
            kind = del.kind;
            if (FUNC == del.kind)
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
            if (FUNC == kind && FUNC != del.kind)
            {
                functional.~FunctionType();
            }
            else if (FUNC != kind && FUNC == del.kind)
            {
                new (&this->functional) FunctionType();
            }
            kind = del.kind;
            if (FUNC == del.kind)
            {
                functional = std::move(del.functional);
            }
            else
            {
                fn = del.fn;
            }
            return *this;
        }

        DelegateImpl& operator=(FunPtr fn)
        {
            if (FUNC == kind)
            {
                functional.~FunctionType();
                kind = FP;
            }
            DelegateImpl::fn = fn;
            return *this;
        }

        template<typename F> DelegateImpl& operator=(const F& functional)
        {
            if (FUNC != kind)
            {
                new (&this->functional) FunctionType();
                kind = FUNC;
            }
            this->functional = functional;
            return *this;
        }

        template<typename F> DelegateImpl& operator=(F&& functional)
        {
            if (FUNC != kind)
            {
                new (&this->functional) FunctionType();
                kind = FUNC;
            }
            this->functional = std::move(functional);
            return *this;
        }

        DelegateImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            if (FUNC == kind)
            {
                functional.~FunctionType();
            }
            kind = FP;
            fn = nullptr;
            return *this;
        }

        operator bool() const
        {
            if (FP == kind)
            {
                return fn;
            }
            else
            {
                return functional ? true : false;
            }
        }

        operator FunVPPtr() const
        {
            if (FP == kind)
            {
                return reinterpret_cast<FunVPPtr>(fn);
            }
            else
            {
                return [](void* self) -> R
                {
                    return static_cast<DelegateImpl*>(self)->functional();
                };
            }
        }

        void* arg() const
        {
            if (FP == kind)
            {
                return nullptr;
            }
            else
            {
                return const_cast<DelegateImpl*>(this);
            }
        }

        operator FunctionType() const
        {
            if (FP == kind)
            {
                return fn;
            }
            else
            {
                return functional;
            }
        }

        R IRAM_ATTR operator()() const
        {
            if (FP == kind)
            {
                return fn();
            }
            else
            {
                return functional();
            }
        }

    protected:
        enum { FUNC, FP } kind = FP;
        union {
            FunctionType functional;
            FunPtr fn;
        };
    };
#else
    template<typename R>
    class DelegateImpl<void, R> {
    public:
        using target_type = R();
    protected:
        using FunPtr = target_type*;
        using FunVPPtr = R(*)(void*);
    public:
        DelegateImpl()
        {
            fn = nullptr;
        }

        DelegateImpl(std::nullptr_t)
        {
            fn = nullptr;
        }

        DelegateImpl(const DelegateImpl& del)
        {
            fn = del.fn;
        }

        DelegateImpl(DelegateImpl&& del)
        {
            fn = std::move(del.fn);
        }

        DelegateImpl(FunPtr fn)
        {
            DelegateImpl::fn = fn;
        }

        template<typename F> DelegateImpl(const F& fn)
        {
            DelegateImpl::fn = fn;
        }

        DelegateImpl& operator=(const DelegateImpl& del)
        {
            if (this == &del) return *this;
            fn = del.fn;
            return *this;
        }

        DelegateImpl& operator=(DelegateImpl&& del)
        {
            if (this == &del) return *this;
            fn = std::move(del.fn);
            return *this;
        }

        DelegateImpl& operator=(FunPtr fn)
        {
            DelegateImpl::fn = fn;
            return *this;
        }

        template<typename F> DelegateImpl& operator=(const F& fn)
        {
            DelegateImpl::fn = fn;
            return *this;
        }

        DelegateImpl& IRAM_ATTR operator=(std::nullptr_t)
        {
            fn = nullptr;
            return *this;
        }

        operator bool() const
        {
            return fn;
        }

        operator FunVPPtr() const
        {
            return reinterpret_cast<FunVPPtr>(fn);
        }

        void* arg() const
        {
            return nullptr;
        }

        R IRAM_ATTR operator()() const
        {
            return fn();
        }

    protected:
        FunPtr fn;
    };
#endif

    template<typename R = void, typename A = void, typename... P>
    class Delegate : private detail::DelegatePImpl<A, R, P...>
    {
    private:
        using typename detail::DelegatePImpl<A, R, P...>::FunVPPtr;
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
        using typename detail::DelegatePImpl<A, R, P...>::FunctionType;
#endif
    public:
        using detail::DelegatePImpl<A, R, P...>::target_type;
        using detail::DelegatePImpl<A, R, P...>::DelegatePImpl;
        using detail::DelegatePImpl<A, R, P...>::operator=;
        using detail::DelegatePImpl<A, R, P...>::operator bool;
        using detail::DelegatePImpl<A, R, P...>::operator FunVPPtr;
        using detail::DelegatePImpl<A, R, P...>::arg;
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
        using detail::DelegatePImpl<A, R, P...>::operator FunctionType;
#endif
        using detail::DelegatePImpl<A, R, P...>::operator();
    };

    template<typename R, typename A, typename... P>
    class Delegate<R, A*, P...> : private detail::DelegatePImpl<A*, R, P...>
    {
    private:
        using typename detail::DelegatePImpl<A*, R, P...>::FunVPPtr;
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
        using typename detail::DelegatePImpl<A*, R, P...>::FunctionType;
#endif
    public:
        using detail::DelegatePImpl<A*, R, P...>::target_type;
        using detail::DelegatePImpl<A*, R, P...>::DelegatePImpl;
        using detail::DelegatePImpl<A*, R, P...>::operator=;
        using detail::DelegatePImpl<A*, R, P...>::operator bool;
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
        using detail::DelegatePImpl<A*, R, P...>::operator FunctionType;
#endif
        using detail::DelegatePImpl<A*, R, P...>::operator();
        operator FunVPPtr() const
        {
            if (detail::DelegatePImpl<A*, R, P...>::FPA == detail::DelegatePImpl<A*, R, P...>::kind)
            {
                return reinterpret_cast<R(*)(void*, P...)>(detail::DelegatePImpl<A*, R, P...>::fnA);
            }
            else
            {
                return detail::DelegatePImpl<A*, R, P...>::operator FunVPPtr();
            }
        }
        void* arg() const
        {
            if (detail::DelegatePImpl<A*, R, P...>::FPA == detail::DelegatePImpl<A*, R, P...>::kind)
            {
                return detail::DelegatePImpl<A*, R, P...>::obj;
            }
            else
            {
                return detail::DelegatePImpl<A*, R, P...>::arg();
            }
        }
    };

    template<typename R, typename A>
    class Delegate<R, A> : private detail::DelegateImpl<A, R>
    {
    private:
        using typename detail::DelegateImpl<A, R>::FunVPPtr;
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
        using typename detail::DelegateImpl<A, R>::FunctionType;
#endif
    public:
        using detail::DelegateImpl<A, R>::target_type;
        using detail::DelegateImpl<A, R>::DelegateImpl;
        using detail::DelegateImpl<A, R>::operator=;
        using detail::DelegateImpl<A, R>::operator bool;
        using detail::DelegateImpl<A, R>::operator FunVPPtr;
        using detail::DelegateImpl<A, R>::arg;
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
        using detail::DelegateImpl<A, R>::operator FunctionType;
#endif
        using detail::DelegateImpl<A, R>::operator();
    };

    template<typename R, typename A>
    class Delegate<R, A*> : private detail::DelegateImpl<A*, R>
    {
    private:
        using typename detail::DelegateImpl<A*, R>::FunVPPtr;
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
        using typename detail::DelegateImpl<A*, R>::FunctionType;
#endif
    public:
        using detail::DelegateImpl<A*, R>::target_type;
        using detail::DelegateImpl<A*, R>::DelegateImpl;
        using detail::DelegateImpl<A*, R>::operator=;
        using detail::DelegateImpl<A*, R>::operator bool;
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
        using detail::DelegateImpl<A*, R>::operator FunctionType;
#endif
        using detail::DelegateImpl<A*, R>::operator();
        operator FunVPPtr() const
        {
            if (detail::DelegateImpl<A*, R>::FPA == detail::DelegateImpl<A*, R>::kind)
            {
                return reinterpret_cast<R(*)(void*)>(detail::DelegateImpl<A*, R>::fnA);
            }
            else
            {
                return detail::DelegateImpl<A*, R>::operator FunVPPtr();
            }
        }
        void* arg() const
        {
            if (detail::DelegateImpl<A*, R>::FPA == detail::DelegateImpl<A*, R>::kind)
            {
                return detail::DelegateImpl<A*, R>::obj;
            }
            else
            {
                return detail::DelegateImpl<A*, R>::arg();
            }
        }
    };

}

template<typename R, typename A = void, typename... P> class Delegate;
template<typename R, typename A, typename... P> class Delegate<R(P...), A> : public detail::Delegate<R, A, P...>
{
public:
    using detail::Delegate<R, A, P...>::Delegate;
    using detail::Delegate<R, A, P...>::operator=;
};
template<typename R, typename... P> class Delegate<R(P...)> : public detail::Delegate<R, void, P...>
{
public:
    using detail::Delegate<R, void, P...>::Delegate;
    using detail::Delegate<R, void, P...>::operator=;
};

#endif // __Delegate_h
