/*
MultiDelegate.h - A queue or event multiplexer based on the efficient Delegate
class
Copyright (c) 2019-2020 Dirk O. Kaar. All rights reserved.

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

#ifndef __MULTIDELEGATE_H
#define __MULTIDELEGATE_H

#include <iterator>
#if defined(ESP8266) || defined(ESP32) || !defined(ARDUINO)
#include <atomic>
#else
#include "circular_queue/ghostl.h"
#endif

#if defined(ESP8266)
#include <interrupts.h>
using esp8266::InterruptLock;
#elif defined(ARDUINO)
class InterruptLock {
public:
    InterruptLock() {
        noInterrupts();
    }
    ~InterruptLock() {
        interrupts();
    }
};
#else
#include <mutex>
#endif

namespace
{

    template< typename Delegate, typename R, bool ISQUEUE = false, typename... P>
    struct CallP
    {
        static R execute(Delegate& del, P... args)
        {
            return del(std::forward<P...>(args...));
        }
    };

    template< typename Delegate, bool ISQUEUE, typename... P>
    struct CallP<Delegate, void, ISQUEUE, P...>
    {
        static bool execute(Delegate& del, P... args)
        {
            del(std::forward<P...>(args...));
            return true;
        }
    };

    template< typename Delegate, typename R, bool ISQUEUE = false>
    struct Call
    {
        static R execute(Delegate& del)
        {
            return del();
        }
    };

    template< typename Delegate, bool ISQUEUE>
    struct Call<Delegate, void, ISQUEUE>
    {
        static bool execute(Delegate& del)
        {
            del();
            return true;
        }
    };

}

namespace delegate
{
    namespace detail
    {

        template< typename Delegate, typename R, bool ISQUEUE = false, size_t QUEUE_CAPACITY = 32, typename... P>
        class MultiDelegatePImpl
        {
        public:
            MultiDelegatePImpl() = default;
            ~MultiDelegatePImpl()
            {
                *this = nullptr;
            }

            MultiDelegatePImpl(const MultiDelegatePImpl&) = delete;
            MultiDelegatePImpl& operator=(const MultiDelegatePImpl&) = delete;

            MultiDelegatePImpl(MultiDelegatePImpl&& md)
            {
                first = md.first;
                last = md.last;
                unused = md.unused;
                nodeCount = md.nodeCount;
                md.first = nullptr;
                md.last = nullptr;
                md.unused = nullptr;
                md.nodeCount = 0;
            }

            MultiDelegatePImpl(const Delegate& del)
            {
                add(del);
            }

            MultiDelegatePImpl(Delegate&& del)
            {
                add(std::move(del));
            }

            MultiDelegatePImpl& operator=(MultiDelegatePImpl&& md)
            {
                first = md.first;
                last = md.last;
                unused = md.unused;
                nodeCount = md.nodeCount;
                md.first = nullptr;
                md.last = nullptr;
                md.unused = nullptr;
                md.nodeCount = 0;
                return *this;
            }

            MultiDelegatePImpl& operator=(std::nullptr_t)
            {
                if (last)
                    last->mNext = unused;
                if (first)
                    unused = first;
                while (unused)
                {
                    auto to_delete = unused;
                    unused = unused->mNext;
                    delete(to_delete);
                }
                return *this;
            }

            MultiDelegatePImpl& operator+=(const Delegate& del)
            {
                add(del);
                return *this;
            }

            MultiDelegatePImpl& operator+=(Delegate&& del)
            {
                add(std::move(del));
                return *this;
            }

        protected:
            struct Node_t
            {
                ~Node_t()
                {
                    mDelegate = nullptr; // special overload in Delegate
                }
                Node_t* mNext = nullptr;
                Delegate mDelegate;
            };

            Node_t* first = nullptr;
            Node_t* last = nullptr;
            Node_t* unused = nullptr;
            size_t nodeCount = 0;

            // Returns a pointer to an unused Node_t,
            // or if none are available allocates a new one,
            // or nullptr if limit is reached
            Node_t* IRAM_ATTR get_node_unsafe()
            {
                Node_t* result = nullptr;
                // try to get an item from unused items list
                if (unused)
                {
                    result = unused;
                    unused = unused->mNext;
                }
                // if no unused items, and count not too high, allocate a new one
                else if (nodeCount < QUEUE_CAPACITY)
                {
#if defined(ESP8266) || defined(ESP32)            	
                    result = new (std::nothrow) Node_t;
#else
                    result = new Node_t;
#endif
                    if (result)
                        ++nodeCount;
                }
                return result;
            }

            void recycle_node_unsafe(Node_t* node)
            {
                node->mDelegate = nullptr; // special overload in Delegate
                node->mNext = unused;
                unused = node;
            }

            void traverse(Node_t*& current) const
            {
                current = current->mNext;
            }
            void traverse(Node_t*& prev, Node_t*& current, bool remove = true)
            {
                if (remove && ISQUEUE)
                {
                    // remove callback from stack
#ifdef ARDUINO
                    InterruptLock lockAllInterruptsInThisScope;
#else
                    std::lock_guard<std::mutex> lock(mutex_unused);
#endif

                    auto to_recycle = current;

                    // removing last
                    if (last == current)
                        last = prev;

                    current = current->mNext;
                    if (prev)
                    {
                        prev->mNext = current;
                    }
                    else
                    {
                        first = current;
                    }

                    recycle_node_unsafe(to_recycle);
                }
                else
                {
                    prev = current;
                    traverse(current);
                }
            }

#ifndef ARDUINO
            std::mutex mutex_unused;
#endif
        public:
            class iterator : public std::iterator<std::forward_iterator_tag, Delegate>
            {
            protected:
                MultiDelegatePImpl& multiDel;
                Node_t* current = nullptr;
                Node_t* const stop = nullptr;
            public:
                iterator(MultiDelegatePImpl& md) : multiDel(md), current(md.first), stop(md.last) {}
                iterator(MultiDelegatePImpl& md, nullptr_t) : multiDel(md) {}
                iterator() = default;
                iterator(const iterator&) = default;
                iterator& operator=(const iterator&) = default;
                bool operator==(const iterator& rhs) const
                {
                    return &multiDel == &rhs.multiDel && current == rhs.current;
                }
                bool operator!=(const iterator& rhs) const {
                    return !operator==(rhs);
                }
                const Delegate& operator*() const
                {
                    return current->mDelegate;
                }
                const Delegate* const operator->() const
                {
                    return &current->mDelegate;
                }
                iterator& operator++() // prefix
                {
                    if (current && stop != current)
                        multiDel.traverse(current);
                    else
                        current = nullptr; // end
                    return *this;
                }
                iterator& operator++(int) // postfix
                {
                    iterator tmp(*this);
                    operator++();
                    return tmp;
                }
            };

            iterator begin() const {
                return iterator(*this);
            }
            iterator end() const {
                return iterator(*this, nullptr);
            }

            const Delegate* IRAM_ATTR add(const Delegate& del)
            {
                return add(Delegate(del));
            }

            const Delegate* IRAM_ATTR add(Delegate&& del)
            {
                if (!del)
                    return nullptr;

#ifdef ARDUINO
                InterruptLock lockAllInterruptsInThisScope;
#else
                std::lock_guard<std::mutex> lock(mutex_unused);
#endif

                Node_t* item = ISQUEUE ? get_node_unsafe() :
#if defined(ESP8266) || defined(ESP32)            	
                    new (std::nothrow) Node_t;
#else
                    new Node_t;
#endif
                if (!item)
                    return nullptr;

                item->mDelegate = std::move(del);
                item->mNext = nullptr;

                if (last)
                    last->mNext = item;
                else
                    first = item;
                last = item;

                return &item->mDelegate;
            }

            bool remove(const Delegate* del)
            {
                auto current = first;
                if (!current)
                    return false;

                Node_t* prev = nullptr;
                do
                {
                    if (del == &current->mDelegate)
                    {
                        // remove callback from stack
#ifdef ARDUINO
                        InterruptLock lockAllInterruptsInThisScope;
#else
                        std::lock_guard<std::mutex> lock(mutex_unused);
#endif

                        auto to_recycle = current;

                        // removing last
                        if (last == current)
                            last = prev;

                        current = current->mNext;
                        if (prev)
                        {
                            prev->mNext = current;
                        }
                        else
                        {
                            first = current;
                        }

                        if (ISQUEUE)
                            recycle_node_unsafe(to_recycle);
                        else
                            delete to_recycle;
                        return true;
                    }
                    else
                    {
                        prev = current;
                        current = current->mNext;
                    }
                } while (current);
                return false;
            }

            operator bool() const
            {
                return first;
            }

            R operator()(P... args)
            {
                auto current = first;
                if (!current)
                    return {};

                static std::atomic<bool> fence(false);
                // prevent recursive calls
#if defined(ARDUINO) && !defined(ESP32)
                if (fence.load()) return {};
                fence.store(true);
#else
                if (fence.exchange(true)) return {};
#endif

                Node_t* prev = nullptr;
                // prevent execution of new callbacks during this run
                auto stop = last;

                R result;
                bool done;
                do
                {
                    done = current == stop;
                    result = CallP<Delegate, R, ISQUEUE, P...>::execute(current->mDelegate, args...);
                    if (ISQUEUE)
                        traverse(prev, current, result);
                    else
                        traverse(current);
#if defined(ESP8266) || defined(ESP32)
                    // running callbacks might last too long for watchdog etc.
                    optimistic_yield(10000);
#endif
                } while (current && !done);

                fence.store(false);
                return result;
            }
        };

        template< typename Delegate, typename R = void, bool ISQUEUE = false, size_t QUEUE_CAPACITY = 32>
        class MultiDelegateImpl : public MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>
        {
        protected:
            using typename MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>::Node_t;
            using MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>::first;
            using MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>::last;
            using MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>::unused;
            using MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>::nodeCount;
            using MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>::recycle_node_unsafe;
#ifndef ARDUINO
            using MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>::mutex_unused;
#endif

        public:
            using MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>::MultiDelegatePImpl;

            R operator()()
            {
                auto current = first;
                if (!current)
                    return {};

                static std::atomic<bool> fence(false);
                // prevent recursive calls
#if defined(ARDUINO) && !defined(ESP32)
                if (fence.load()) return {};
                fence.store(true);
#else
                if (fence.exchange(true)) return {};
#endif

                Node_t* prev = nullptr;
                // prevent execution of new callbacks during this run
                auto stop = last;

                R result;
                bool done;
                do
                {
                    done = current == stop;
                    result = Call<Delegate, R, ISQUEUE>::execute(current->mDelegate);
                    if (ISQUEUE)
                        this->traverse(prev, current, result);
                    else
                        this->traverse(current);
#if defined(ESP8266) || defined(ESP32)
                    // running callbacks might last too long for watchdog etc.
                    optimistic_yield(10000);
#endif
                } while (current && !done);

                fence.store(false);
                return result;
            }
        };

        template< typename Delegate, typename R, bool ISQUEUE, size_t QUEUE_CAPACITY, typename... P> class MultiDelegate;

        template< typename Delegate, typename R, bool ISQUEUE, size_t QUEUE_CAPACITY, typename... P>
        class MultiDelegate<Delegate, R(P...), ISQUEUE, QUEUE_CAPACITY> : public MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY, P...>
        {
        public:
            using MultiDelegatePImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY, P...>::MultiDelegatePImpl;
        };

        template< typename Delegate, typename R, bool ISQUEUE, size_t QUEUE_CAPACITY>
        class MultiDelegate<Delegate, R(), ISQUEUE, QUEUE_CAPACITY> : public MultiDelegateImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>
        {
        public:
            using MultiDelegateImpl<Delegate, R, ISQUEUE, QUEUE_CAPACITY>::MultiDelegateImpl;
        };

        template< typename Delegate, bool ISQUEUE, size_t QUEUE_CAPACITY, typename... P>
        class MultiDelegate<Delegate, void(P...), ISQUEUE, QUEUE_CAPACITY> : public MultiDelegatePImpl<Delegate, void, ISQUEUE, QUEUE_CAPACITY, P...>
        {
        public:
            using MultiDelegatePImpl<Delegate, void, ISQUEUE, QUEUE_CAPACITY, P...>::MultiDelegatePImpl;
            using typename MultiDelegatePImpl<Delegate, void, ISQUEUE, QUEUE_CAPACITY, P...>::Node_t;
            using MultiDelegatePImpl<Delegate, void, ISQUEUE, QUEUE_CAPACITY, P...>::first;
            using MultiDelegatePImpl<Delegate, void, ISQUEUE, QUEUE_CAPACITY, P...>::last;

            void operator()(P... args)
            {
                auto current = first;
                if (!current)
                    return;

                static std::atomic<bool> fence(false);
                // prevent recursive calls
#if defined(ARDUINO) && !defined(ESP32)
                if (fence.load()) return;
                fence.store(true);
#else
                if (fence.exchange(true)) return;
#endif

                Node_t* prev = nullptr;
                // prevent execution of new callbacks during this run
                auto stop = last;

                bool done;
                do
                {
                    done = current == stop;
                    CallP<Delegate, void, ISQUEUE, P...>::execute(current->mDelegate, args...);
                    if (ISQUEUE)
                        this->traverse(prev, current);
                    else
                        traverse(current);
#if defined(ESP8266) || defined(ESP32)
                    // running callbacks might last too long for watchdog etc.
                    optimistic_yield(10000);
#endif
                } while (current && !done);

                fence.store(false);
            }
        };

        template< typename Delegate, bool ISQUEUE, size_t QUEUE_CAPACITY>
        class MultiDelegate<Delegate, void(), ISQUEUE, QUEUE_CAPACITY> : public MultiDelegateImpl<Delegate, void, ISQUEUE, QUEUE_CAPACITY>
        {
        public:
            using MultiDelegateImpl<Delegate, void, ISQUEUE, QUEUE_CAPACITY>::MultiDelegateImpl;
            using typename MultiDelegateImpl<Delegate, void, ISQUEUE, QUEUE_CAPACITY>::Node_t;
            using MultiDelegateImpl<Delegate, void, ISQUEUE, QUEUE_CAPACITY>::first;
            using MultiDelegateImpl<Delegate, void, ISQUEUE, QUEUE_CAPACITY>::last;

            void operator()()
            {
                auto current = first;
                if (!current)
                    return;

                static std::atomic<bool> fence(false);
                // prevent recursive calls
#if defined(ARDUINO) && !defined(ESP32)
                if (fence.load()) return;
                fence.store(true);
#else
                if (fence.exchange(true)) return;
#endif

                Node_t* prev = nullptr;
                // prevent execution of new callbacks during this run
                auto stop = last;

                bool done;
                do
                {
                    done = current == stop;
                    Call<Delegate, void, ISQUEUE>::execute(current->mDelegate);
                    if (ISQUEUE)
                        this->traverse(prev, current);
                    else
                        this->traverse(current);
#if defined(ESP8266) || defined(ESP32)
                    // running callbacks might last too long for watchdog etc.
                    optimistic_yield(10000);
#endif
                } while (current && !done);

                fence.store(false);
            }
        };

    }

}

/**
The MultiDelegate class template can be specialized to either a queue or an event multiplexer.
It is designed to be used with Delegate, the efficient runtime wrapper for C function ptr and C++ std::function.
@tparam Delegate specifies the concrete type that MultiDelegate bases the queue or event multiplexer on.
@tparam ISQUEUE modifies the generated MultiDelegate class in subtle ways. In queue mode (ISQUEUE == true),
               the value of QUEUE_CAPACITY enforces the maximum number of simultaneous items the queue can contain.
               This is exploited to minimize the use of new and delete by reusing already allocated items, thus
               reducing heap fragmentation. In event multiplexer mode (ISQUEUE = false), new and delete are
               used for allocation of the event handler items.
               If the result type of the function call operator of Delegate is void, calling a MultiDelegate queue
               removes each item after calling it; a Multidelegate event multiplexer keeps event handlers until
               explicitly removed.
               If the result type of the function call operator of Delegate is non-void, in a MultiDelegate queue
               the type-conversion to bool of that result determines if the item is immediately removed or kept
               after each call: if true is returned, the item is removed. A Multidelegate event multiplexer keeps event
               handlers until they are explicitly removed.
@tparam QUEUE_CAPACITY is only used if ISQUEUE == true. Then, it sets the maximum capacity that the queue dynamically
               allocates from the heap. Unused items are not returned to the heap, but are managed by the MultiDelegate
               instance during its own lifetime for efficiency.
*/
template< typename Delegate, bool ISQUEUE = false, size_t QUEUE_CAPACITY = 32>
class MultiDelegate : public delegate::detail::MultiDelegate<Delegate, typename Delegate::target_type, ISQUEUE, QUEUE_CAPACITY>
{
public:
    using delegate::detail::MultiDelegate<Delegate, typename Delegate::target_type, ISQUEUE, QUEUE_CAPACITY>::MultiDelegate;
};

#endif // __MULTIDELEGATE_H
