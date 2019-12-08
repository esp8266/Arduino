#ifndef __MULTIDELEGATE_H
#define __MULTIDELEGATE_H

#include <interrupts.h>

namespace detail
{
    namespace
    {
        template< typename Delegate, typename R, bool ISQUEUE = false, typename... P>
        struct CallP
        {
            static R execute(Delegate& del, P... args)
            {
                return del(std::forward<P...>(args...)) ? !ISQUEUE : ISQUEUE;
            }
        };

        template< typename Delegate, bool ISQUEUE, typename... P>
        struct CallP<Delegate, void, ISQUEUE, P...>
        {
            static bool execute(Delegate& del, P... args)
            {
                del(std::forward<P...>(args...));
                return !ISQUEUE;
            }
        };

        template< typename Delegate, typename R, bool ISQUEUE = false>
        struct Call
        {
            static R execute(Delegate& del)
            {
                return del() ? !ISQUEUE : ISQUEUE;
            }
        };

        template< typename Delegate, bool ISQUEUE>
        struct Call<Delegate, void, ISQUEUE>
        {
            static bool execute(Delegate& del)
            {
                del();
                return !ISQUEUE;
            }
        };
    };

    template< typename Delegate, typename R = void, bool ISQUEUE = false, uint32_t MULTICALLBACK_MAX_COUNT = 32, typename... P>
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
        int nodeCount = 0;

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
            else if (nodeCount < MULTICALLBACK_MAX_COUNT)
            {
                result = new (std::nothrow) Node_t;
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

    public:
        const Delegate* IRAM_ATTR add(const Delegate& del)
        {
            return add(Delegate(del));
        }

        const Delegate* IRAM_ATTR add(Delegate&& del)
        {
            if (!del)
                return nullptr;

            esp8266::InterruptLock lockAllInterruptsInThisScope;

            Node_t* item = get_node_unsafe();
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
                    esp8266::InterruptLock lockAllInterruptsInThisScope;

                    auto to_recycle = current;

                    // removing rLast
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

        void operator()(P... args)
        {
            auto current = first;
            if (!current)
                return;

            static bool fence = false;
            {
                esp8266::InterruptLock lockAllInterruptsInThisScope;

                // prevent recursive calls
                if (fence)
                    return;
                fence = true;
            }

            Node_t* prev = nullptr;
            // prevent execution of new callbacks during this run
            auto stop = last;

            bool done;
            do
            {
                done = current == stop;
                if (!CallP<Delegate, R, ISQUEUE, P...>::execute(current->mDelegate, std::forward<P...>(args...)))
                {
                    // remove callback from stack
                    esp8266::InterruptLock lockAllInterruptsInThisScope;

                    auto to_recycle = current;

                    // removing rLast
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
                    current = current->mNext;
                }

                // running callbacks might last too long for watchdog etc.
                optimistic_yield(10000);
            } while (current && !done);

            fence = false;
        }
    };

    template< typename Delegate, typename R = void, bool ISQUEUE = false, uint32_t MULTICALLBACK_MAX_COUNT = 32>
    class MultiDelegateImpl : public MultiDelegatePImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT>
    {
    protected:
        using typename MultiDelegatePImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT>::Node_t;
        using MultiDelegatePImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT>::first;
        using MultiDelegatePImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT>::last;
        using MultiDelegatePImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT>::unused;
        using MultiDelegatePImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT>::nodeCount;
        using MultiDelegatePImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT>::recycle_node_unsafe;

    public:
        using MultiDelegatePImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT>::MultiDelegatePImpl;

        void operator()()
        {
            auto current = first;
            if (!current)
                return;

            static bool fence = false;
            {
                esp8266::InterruptLock lockAllInterruptsInThisScope;

                // prevent recursive calls
                if (fence)
                    return;
                fence = true;
            }

            Node_t* prev = nullptr;
            // prevent execution of new callbacks during this run
            auto stop = last;

            bool done;
            do
            {
                done = current == stop;
                if (!Call<Delegate, R, ISQUEUE>::execute(current->mDelegate))
                {
                    // remove callback from stack
                    esp8266::InterruptLock lockAllInterruptsInThisScope;

                    auto to_recycle = current;

                    // removing rLast
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
                    current = current->mNext;
                }

                // running callbacks might last too long for watchdog etc.
                optimistic_yield(10000);
            } while (current && !done);

            fence = false;
        }
    };

    template< typename Delegate, typename R, bool ISQUEUE, uint32_t MULTICALLBACK_MAX_COUNT, typename... P> class MultiDelegate;

    template< typename Delegate, typename R, bool ISQUEUE, uint32_t MULTICALLBACK_MAX_COUNT, typename... P>
    class MultiDelegate<Delegate, R(P...), ISQUEUE, MULTICALLBACK_MAX_COUNT> : public MultiDelegatePImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT, P...>
    {
    public:
        using MultiDelegatePImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT, P...>::MultiDelegatePImpl;
    };

    template< typename Delegate, typename R, bool ISQUEUE, uint32_t MULTICALLBACK_MAX_COUNT>
    class MultiDelegate<Delegate, R(), ISQUEUE, MULTICALLBACK_MAX_COUNT> : public MultiDelegateImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT>
    {
    public:
        using MultiDelegateImpl<Delegate, R, ISQUEUE, MULTICALLBACK_MAX_COUNT>::MultiDelegateImpl;
    };
};

template< typename Delegate, bool ISQUEUE = false, uint32_t MULTICALLBACK_MAX_COUNT = 32>
class MultiDelegate : public detail::MultiDelegate<Delegate, typename Delegate::target_type, ISQUEUE, MULTICALLBACK_MAX_COUNT>
{
public:
    using detail::MultiDelegate<Delegate, typename Delegate::target_type, ISQUEUE, MULTICALLBACK_MAX_COUNT>::MultiDelegatePImpl;
};

#endif // __MULTIDELEGATE_H
