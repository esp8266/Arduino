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

namespace
{

    template<typename R, typename... P>
    R IRAM_ATTR vPtrToFunPtrExec(void* fn, P... args)
    {
        using target_type = R(P...);
        return reinterpret_cast<target_type*>(fn)(std::forward<P...>(args...));
    }

}

namespace delegate
{
    namespace detail
    {

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
                kind = FP;
                fn = nullptr;
            }

            DelegatePImpl(std::nullptr_t)
            {
                kind = FP;
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

            template<typename F> DelegatePImpl(F functional)
            {
                kind = FUNC;
                new (&this->functional) FunctionType(std::forward<F>(functional));
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

            template<typename F> DelegatePImpl& operator=(F functional)
            {
                if (FUNC == kind)
                {
                    this->functional.~FunctionType();
                }
                else if (FPA == kind)
                {
                    obj.~A();
                }
                kind = FUNC;
                new (&this->functional) FunctionType(std::forward<F>(functional));
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
            enum { FUNC, FP, FPA } kind;
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
                kind = FP;
                fn = nullptr;
            }

            DelegatePImpl(std::nullptr_t)
            {
                kind = FP;
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

            template<typename F> DelegatePImpl(F functional)
            {
                kind = FP;
                fn = std::forward<F>(functional);
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

            template<typename F> DelegatePImpl& operator=(F functional)
            {
                if (FPA == kind)
                {
                    obj = {};
                }
                kind = FP;
                fn = std::forward<F>(functional);
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
            enum { FP, FPA } kind;
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
                kind = FP;
                fn = nullptr;
            }

            DelegatePImpl(std::nullptr_t)
            {
                kind = FP;
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

            template<typename F> DelegatePImpl(F functional)
            {
                kind = FUNC;
                new (&this->functional) FunctionType(std::forward<F>(functional));
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

            template<typename F> DelegatePImpl& operator=(F functional)
            {
                if (FUNC == kind)
                {
                    this->functional.~FunctionType();
                }
                kind = FUNC;
                new (&this->functional) FunctionType(std::forward<F>(functional));
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
            enum { FUNC, FP } kind;
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

            template<typename F> DelegatePImpl(F fn)
            {
                DelegatePImpl::fn = std::forward<F>(fn);
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

            template<typename F> DelegatePImpl& operator=(F functional)
            {
                fn = std::forward<F>(functional);
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
                kind = FP;
                fn = nullptr;
            }

            DelegateImpl(std::nullptr_t)
            {
                kind = FP;
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

            template<typename F> DelegateImpl(F functional)
            {
                kind = FUNC;
                new (&this->functional) FunctionType(std::forward<F>(functional));
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

            template<typename F> DelegateImpl& operator=(F functional)
            {
                if (FUNC == kind)
                {
                    this->functional.~FunctionType();
                }
                else if (FPA == kind)
                {
                    obj.~A();
                }
                kind = FUNC;
                new (&this->functional) FunctionType(std::forward<F>(functional));
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
            enum { FUNC, FP, FPA } kind;
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
                kind = FP;
                fn = nullptr;
            }

            DelegateImpl(std::nullptr_t)
            {
                kind = FP;
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

            template<typename F> DelegateImpl(F fn)
            {
                kind = FP;
                DelegateImpl::fn = std::forward<F>(fn);
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

            template<typename F> DelegateImpl& operator=(F functional)
            {
                if (FPA == kind)
                {
                    obj.~A();
                }
                kind = FP;
                fn = std::forward<F>(functional);
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
            enum { FP, FPA } kind;
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
                kind = FP;
                fn = nullptr;
            }

            DelegateImpl(std::nullptr_t)
            {
                kind = FP;
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

            template<typename F> DelegateImpl(F functional)
            {
                kind = FUNC;
                new (&this->functional) FunctionType(std::forward<F>(functional));
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

            template<typename F> DelegateImpl& operator=(F functional)
            {
                if (FUNC == kind)
                {
                    this->functional.~FunctionType();
                }
                kind = FUNC;
                new (&this->functional) FunctionType(std::forward<F>(functional));
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
            enum { FUNC, FP } kind;
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

            template<typename F> DelegateImpl(F fn)
            {
                DelegateImpl::fn = std::forward<F>(fn);
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

            template<typename F> DelegateImpl& operator=(F functional)
            {
                fn = std::forward<F>(functional);
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

        template<typename A = void, typename R = void, typename... P>
        class Delegate : private detail::DelegatePImpl<A, R, P...>
        {
        public:
            using target_type = R(P...);
        protected:
            using FunPtr = target_type*;
            using FunAPtr = R(*)(A, P...);
            using FunVPPtr = R(*)(void*, P...);
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            using FunctionType = std::function<target_type>;
#endif
        public:
            using detail::DelegatePImpl<A, R, P...>::operator bool;
            using detail::DelegatePImpl<A, R, P...>::arg;
            using detail::DelegatePImpl<A, R, P...>::operator();

            operator FunVPPtr() { return detail::DelegatePImpl<A, R, P...>::operator FunVPPtr(); }
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            operator FunctionType() { return detail::DelegatePImpl<A, R, P...>::operator FunctionType(); }
#endif

            Delegate() : detail::DelegatePImpl<A, R, P...>::DelegatePImpl() {}

            Delegate(std::nullptr_t) : detail::DelegatePImpl<A, R, P...>::DelegatePImpl(nullptr) {}

            Delegate(const Delegate& del) : detail::DelegatePImpl<A, R, P...>::DelegatePImpl(
                static_cast<const detail::DelegatePImpl<A, R, P...>&>(del)) {}

            Delegate(Delegate&& del) : detail::DelegatePImpl<A, R, P...>::DelegatePImpl(
                std::move(static_cast<detail::DelegatePImpl<A, R, P...>&>(del))) {}

            Delegate(FunAPtr fnA, const A& obj) : detail::DelegatePImpl<A, R, P...>::DelegatePImpl(fnA, obj) {}

            Delegate(FunAPtr fnA, A&& obj) : detail::DelegatePImpl<A, R, P...>::DelegatePImpl(fnA, std::move(obj)) {}

            Delegate(FunPtr fn) : detail::DelegatePImpl<A, R, P...>::DelegatePImpl(fn) {}

            template<typename F> Delegate(F functional) : detail::DelegatePImpl<A, R, P...>::DelegatePImpl(std::forward<F>(functional)) {}

            Delegate& operator=(const Delegate& del) {
                detail::DelegatePImpl<A, R, P...>::operator=(del);
                return *this;
            }

            Delegate& operator=(Delegate&& del) {
                detail::DelegatePImpl<A, R, P...>::operator=(std::move(del));
                return *this;
            }

            Delegate& operator=(FunPtr fn) {
                detail::DelegatePImpl<A, R, P...>::operator=(fn);
                return *this;
            }

            template<typename F> Delegate& operator=(F functional) {
                detail::DelegatePImpl<A, R, P...>::operator=(std::forward<F>(functional));
                return *this;
            }

            Delegate& IRAM_ATTR operator=(std::nullptr_t) {
                detail::DelegatePImpl<A, R, P...>::operator=(nullptr);
                return *this;
            }
        };

        template<typename A, typename R, typename... P>
        class Delegate<A*, R, P...> : private detail::DelegatePImpl<A*, R, P...>
        {
        public:
            using target_type = R(P...);
        protected:
            using FunPtr = target_type*;
            using FunAPtr = R(*)(A*, P...);
            using FunVPPtr = R(*)(void*, P...);
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            using FunctionType = std::function<target_type>;
#endif
        public:
            using detail::DelegatePImpl<A*, R, P...>::operator bool;
            using detail::DelegatePImpl<A*, R, P...>::operator();

            operator FunVPPtr() const
            {
                if (detail::DelegatePImpl<A*, R, P...>::FPA == detail::DelegatePImpl<A*, R, P...>::kind)
                {
                    return reinterpret_cast<FunVPPtr>(detail::DelegatePImpl<A*, R, P...>::fnA);
                }
                else
                {
                    return detail::DelegatePImpl<A*, R, P...>::operator FunVPPtr();
                }
            }
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            operator FunctionType() { return detail::DelegatePImpl<A*, R, P...>::operator FunctionType(); }
#endif
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

            Delegate() : detail::DelegatePImpl<A*, R, P...>::DelegatePImpl() {}

            Delegate(std::nullptr_t) : detail::DelegatePImpl<A*, R, P...>::DelegatePImpl(nullptr) {}

            Delegate(const Delegate& del) : detail::DelegatePImpl<A*, R, P...>::DelegatePImpl(
                static_cast<const detail::DelegatePImpl<A*, R, P...>&>(del)) {}

            Delegate(Delegate&& del) : detail::DelegatePImpl<A*, R, P...>::DelegatePImpl(
                std::move(static_cast<detail::DelegatePImpl<A*, R, P...>&>(del))) {}

            Delegate(FunAPtr fnA, A* obj) : detail::DelegatePImpl<A*, R, P...>::DelegatePImpl(fnA, obj) {}

            Delegate(FunPtr fn) : detail::DelegatePImpl<A*, R, P...>::DelegatePImpl(fn) {}

            template<typename F> Delegate(F functional) : detail::DelegatePImpl<A*, R, P...>::DelegatePImpl(std::forward<F>(functional)) {}

            Delegate& operator=(const Delegate& del) {
                detail::DelegatePImpl<A*, R, P...>::operator=(del);
                return *this;
            }

            Delegate& operator=(Delegate&& del) {
                detail::DelegatePImpl<A*, R, P...>::operator=(std::move(del));
                return *this;
            }

            Delegate& operator=(FunPtr fn) {
                detail::DelegatePImpl<A*, R, P...>::operator=(fn);
                return *this;
            }

            template<typename F> Delegate& operator=(F functional) {
                detail::DelegatePImpl<A*, R, P...>::operator=(std::forward<F>(functional));
                return *this;
            }

            Delegate& IRAM_ATTR operator=(std::nullptr_t) {
                detail::DelegatePImpl<A*, R, P...>::operator=(nullptr);
                return *this;
            }
        };

        template<typename R, typename... P>
        class Delegate<void, R, P...> : private detail::DelegatePImpl<void, R, P...>
        {
        public:
            using target_type = R(P...);
        protected:
            using FunPtr = target_type*;
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            using FunctionType = std::function<target_type>;
#endif
            using FunVPPtr = R(*)(void*, P...);
        public:
            using detail::DelegatePImpl<void, R, P...>::operator bool;
            using detail::DelegatePImpl<void, R, P...>::arg;
            using detail::DelegatePImpl<void, R, P...>::operator();

            operator FunVPPtr() const { return detail::DelegatePImpl<void, R, P...>::operator FunVPPtr(); }
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            operator FunctionType() { return detail::DelegatePImpl<void, R, P...>::operator FunctionType(); }
#endif

            Delegate() : detail::DelegatePImpl<void, R, P...>::DelegatePImpl() {}

            Delegate(std::nullptr_t) : detail::DelegatePImpl<void, R, P...>::DelegatePImpl(nullptr) {}

            Delegate(const Delegate& del) : detail::DelegatePImpl<void, R, P...>::DelegatePImpl(
                static_cast<const detail::DelegatePImpl<void, R, P...>&>(del)) {}

            Delegate(Delegate&& del) : detail::DelegatePImpl<void, R, P...>::DelegatePImpl(
                std::move(static_cast<detail::DelegatePImpl<void, R, P...>&>(del))) {}

            Delegate(FunPtr fn) : detail::DelegatePImpl<void, R, P...>::DelegatePImpl(fn) {}

            template<typename F> Delegate(F functional) : detail::DelegatePImpl<void, R, P...>::DelegatePImpl(std::forward<F>(functional)) {}

            Delegate& operator=(const Delegate& del) {
                detail::DelegatePImpl<void, R, P...>::operator=(del);
                return *this;
            }

            Delegate& operator=(Delegate&& del) {
                detail::DelegatePImpl<void, R, P...>::operator=(std::move(del));
                return *this;
            }

            Delegate& operator=(FunPtr fn) {
                detail::DelegatePImpl<void, R, P...>::operator=(fn);
                return *this;
            }

            template<typename F> Delegate& operator=(F functional) {
                detail::DelegatePImpl<void, R, P...>::operator=(std::forward<F>(functional));
                return *this;
            }

            Delegate& IRAM_ATTR operator=(std::nullptr_t) {
                detail::DelegatePImpl<void, R, P...>::operator=(nullptr);
                return *this;
            }
        };

        template<typename A, typename R>
        class Delegate<A, R> : private detail::DelegateImpl<A, R>
        {
        public:
            using target_type = R();
        protected:
            using FunPtr = target_type*;
            using FunAPtr = R(*)(A);
            using FunVPPtr = R(*)(void*);
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            using FunctionType = std::function<target_type>;
#endif
        public:
            using detail::DelegateImpl<A, R>::operator bool;
            using detail::DelegateImpl<A, R>::arg;
            using detail::DelegateImpl<A, R>::operator();

            operator FunVPPtr() { return detail::DelegateImpl<A, R>::operator FunVPPtr(); }
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            operator FunctionType() { return detail::DelegateImpl<A, R>::operator FunctionType(); }
#endif

            Delegate() : detail::DelegateImpl<A, R>::DelegateImpl() {}

            Delegate(std::nullptr_t) : detail::DelegateImpl<A, R>::DelegateImpl(nullptr) {}

            Delegate(const Delegate& del) : detail::DelegateImpl<A, R>::DelegateImpl(
                static_cast<const detail::DelegateImpl<A, R>&>(del)) {}

            Delegate(Delegate&& del) : detail::DelegateImpl<A, R>::DelegateImpl(
                std::move(static_cast<detail::DelegateImpl<A, R>&>(del))) {}

            Delegate(FunAPtr fnA, const A& obj) : detail::DelegateImpl<A, R>::DelegateImpl(fnA, obj) {}

            Delegate(FunAPtr fnA, A&& obj) : detail::DelegateImpl<A, R>::DelegateImpl(fnA, std::move(obj)) {}

            Delegate(FunPtr fn) : detail::DelegateImpl<A, R>::DelegateImpl(fn) {}

            template<typename F> Delegate(F functional) : detail::DelegateImpl<A, R>::DelegateImpl(std::forward<F>(functional)) {}

            Delegate& operator=(const Delegate& del) {
                detail::DelegateImpl<A, R>::operator=(del);
                return *this;
            }

            Delegate& operator=(Delegate&& del) {
                detail::DelegateImpl<A, R>::operator=(std::move(del));
                return *this;
            }

            Delegate& operator=(FunPtr fn) {
                detail::DelegateImpl<A, R>::operator=(fn);
                return *this;
            }

            template<typename F> Delegate& operator=(F functional) {
                detail::DelegateImpl<A, R>::operator=(std::forward<F>(functional));
                return *this;
            }

            Delegate& IRAM_ATTR operator=(std::nullptr_t) {
                detail::DelegateImpl<A, R>::operator=(nullptr);
                return *this;
            }
        };

        template<typename A, typename R>
        class Delegate<A*, R> : private detail::DelegateImpl<A*, R>
        {
        public:
            using target_type = R();
        protected:
            using FunPtr = target_type*;
            using FunAPtr = R(*)(A*);
            using FunVPPtr = R(*)(void*);
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            using FunctionType = std::function<target_type>;
#endif
        public:
            using detail::DelegateImpl<A*, R>::operator bool;
            using detail::DelegateImpl<A*, R>::operator();

            operator FunVPPtr() const
            {
                if (detail::DelegateImpl<A*, R>::FPA == detail::DelegateImpl<A*, R>::kind)
                {
                    return reinterpret_cast<FunVPPtr>(detail::DelegateImpl<A*, R>::fnA);
                }
                else
                {
                    return detail::DelegateImpl<A*, R>::operator FunVPPtr();
                }
            }
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            operator FunctionType() { return detail::DelegateImpl<A*, R>::operator FunctionType(); }
#endif
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

            Delegate() : detail::DelegateImpl<A*, R>::DelegateImpl() {}

            Delegate(std::nullptr_t) : detail::DelegateImpl<A*, R>::DelegateImpl(nullptr) {}

            Delegate(const Delegate& del) : detail::DelegateImpl<A*, R>::DelegateImpl(
                static_cast<const detail::DelegateImpl<A*, R>&>(del)) {}

            Delegate(Delegate&& del) : detail::DelegateImpl<A*, R>::DelegateImpl(
                std::move(static_cast<detail::DelegateImpl<A*, R>&>(del))) {}

            Delegate(FunAPtr fnA, A* obj) : detail::DelegateImpl<A*, R>::DelegateImpl(fnA, obj) {}

            Delegate(FunPtr fn) : detail::DelegateImpl<A*, R>::DelegateImpl(fn) {}

            template<typename F> Delegate(F functional) : detail::DelegateImpl<A*, R>::DelegateImpl(std::forward<F>(functional)) {}

            Delegate& operator=(const Delegate& del) {
                detail::DelegateImpl<A*, R>::operator=(del);
                return *this;
            }

            Delegate& operator=(Delegate&& del) {
                detail::DelegateImpl<A*, R>::operator=(std::move(del));
                return *this;
            }

            Delegate& operator=(FunPtr fn) {
                detail::DelegateImpl<A*, R>::operator=(fn);
                return *this;
            }

            template<typename F> Delegate& operator=(F functional) {
                detail::DelegateImpl<A*, R>::operator=(std::forward<F>(functional));
                return *this;
            }

            Delegate& IRAM_ATTR operator=(std::nullptr_t) {
                detail::DelegateImpl<A*, R>::operator=(nullptr);
                return *this;
            }
        };

        template<typename R>
        class Delegate<void, R> : private detail::DelegateImpl<void, R>
        {
        public:
            using target_type = R();
        protected:
            using FunPtr = target_type*;
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            using FunctionType = std::function<target_type>;
#endif
            using FunVPPtr = R(*)(void*);
        public:
            using detail::DelegateImpl<void, R>::operator bool;
            using detail::DelegateImpl<void, R>::arg;
            using detail::DelegateImpl<void, R>::operator();

            operator FunVPPtr() const { return detail::DelegateImpl<void, R>::operator FunVPPtr(); }
#if !defined(ARDUINO) || defined(ESP8266) || defined(ESP32)
            operator FunctionType() { return detail::DelegateImpl<void, R>::operator FunctionType(); }
#endif

            Delegate() : detail::DelegateImpl<void, R>::DelegateImpl() {}

            Delegate(std::nullptr_t) : detail::DelegateImpl<void, R>::DelegateImpl(nullptr) {}

            Delegate(const Delegate& del) : detail::DelegateImpl<void, R>::DelegateImpl(
                static_cast<const detail::DelegateImpl<void, R>&>(del)) {}

            Delegate(Delegate&& del) : detail::DelegateImpl<void, R>::DelegateImpl(
                std::move(static_cast<detail::DelegateImpl<void, R>&>(del))) {}

            Delegate(FunPtr fn) : detail::DelegateImpl<void, R>::DelegateImpl(fn) {}

            template<typename F> Delegate(F functional) : detail::DelegateImpl<void, R>::DelegateImpl(std::forward<F>(functional)) {}

            Delegate& operator=(const Delegate& del) {
                detail::DelegateImpl<void, R>::operator=(del);
                return *this;
            }

            Delegate& operator=(Delegate&& del) {
                detail::DelegateImpl<void, R>::operator=(std::move(del));
                return *this;
            }

            Delegate& operator=(FunPtr fn) {
                detail::DelegateImpl<void, R>::operator=(fn);
                return *this;
            }

            template<typename F> Delegate& operator=(F functional) {
                detail::DelegateImpl<void, R>::operator=(std::forward<F>(functional));
                return *this;
            }

            Delegate& IRAM_ATTR operator=(std::nullptr_t) {
                detail::DelegateImpl<void, R>::operator=(nullptr);
                return *this;
            }
        };
    }
}

template<typename A = void, typename R = void, typename... P> class Delegate;
template<typename A, typename R, typename... P> class Delegate<R(P...), A> : public delegate::detail::Delegate<A, R, P...>
{
public:
    Delegate() : delegate::detail::Delegate<A, R, P...>::Delegate() {}

    Delegate(std::nullptr_t) : delegate::detail::Delegate<A, R, P...>::Delegate(nullptr) {}

    Delegate(const Delegate& del) : delegate::detail::Delegate<A, R, P...>::Delegate(
        static_cast<const delegate::detail::Delegate<A, R, P...>&>(del)) {}

    Delegate(Delegate&& del) : delegate::detail::Delegate<A, R, P...>::Delegate(
        std::move(static_cast<delegate::detail::Delegate<A, R, P...>&>(del))) {}

    Delegate(typename delegate::detail::Delegate<A, R, P...>::FunAPtr fnA, const A& obj) : delegate::detail::Delegate<A, R, P...>::Delegate(fnA, obj) {}

    Delegate(typename delegate::detail::Delegate<A, R, P...>::FunAPtr fnA, A&& obj) : delegate::detail::Delegate<A, R, P...>::Delegate(fnA, std::move(obj)) {}

    Delegate(typename delegate::detail::Delegate<A, R, P...>::FunPtr fn) : delegate::detail::Delegate<A, R, P...>::Delegate(fn) {}

    template<typename F> Delegate(F functional) : delegate::detail::Delegate<A, R, P...>::Delegate(std::forward<F>(functional)) {}

    Delegate& operator=(const Delegate& del) {
        delegate::detail::Delegate<A, R, P...>::operator=(del);
        return *this;
    }

    Delegate& operator=(Delegate&& del) {
        delegate::detail::Delegate<A, R, P...>::operator=(std::move(del));
        return *this;
    }

    Delegate& operator=(typename delegate::detail::Delegate<A, R, P...>::FunPtr fn) {
        delegate::detail::Delegate<A, R, P...>::operator=(fn);
        return *this;
    }

    template<typename F> Delegate& operator=(F functional) {
        delegate::detail::Delegate<A, R, P...>::operator=(std::forward<F>(functional));
        return *this;
    }

    Delegate& IRAM_ATTR operator=(std::nullptr_t) {
        delegate::detail::Delegate<A, R, P...>::operator=(nullptr);
        return *this;
    }
};

template<typename R, typename... P> class Delegate<R(P...)> : public delegate::detail::Delegate<void, R, P...>
{
public:
    Delegate() : delegate::detail::Delegate<void, R, P...>::Delegate() {}

    Delegate(std::nullptr_t) : delegate::detail::Delegate<void, R, P...>::Delegate(nullptr) {}

    Delegate(const Delegate& del) : delegate::detail::Delegate<void, R, P...>::Delegate(
        static_cast<const delegate::detail::Delegate<void, R, P...>&>(del)) {}

    Delegate(Delegate&& del) : delegate::detail::Delegate<void, R, P...>::Delegate(
        std::move(static_cast<delegate::detail::Delegate<void, R, P...>&>(del))) {}

    Delegate(typename delegate::detail::Delegate<void, R, P...>::FunPtr fn) : delegate::detail::Delegate<void, R, P...>::Delegate(fn) {}

    template<typename F> Delegate(F functional) : delegate::detail::Delegate<void, R, P...>::Delegate(std::forward<F>(functional)) {}

    Delegate& operator=(const Delegate& del) {
        delegate::detail::Delegate<void, R, P...>::operator=(del);
        return *this;
    }

    Delegate& operator=(Delegate&& del) {
        delegate::detail::Delegate<void, R, P...>::operator=(std::move(del));
        return *this;
    }

    Delegate& operator=(typename delegate::detail::Delegate<void, R, P...>::FunPtr fn) {
        delegate::detail::Delegate<void, R, P...>::operator=(fn);
        return *this;
    }

    template<typename F> Delegate& operator=(F functional) {
        delegate::detail::Delegate<void, R, P...>::operator=(std::forward<F>(functional));
        return *this;
    }

    Delegate& IRAM_ATTR operator=(std::nullptr_t) {
        delegate::detail::Delegate<void, R, P...>::operator=(nullptr);
        return *this;
    }
};

#endif // __Delegate_h
