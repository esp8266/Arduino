/*
circular_queue_mp.h - Implementation of a lock-free circular queue for EspSoftwareSerial.
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

#ifndef __circular_queue_mp_h
#define __circular_queue_mp_h

#include "circular_queue.h"

#ifdef ESP8266
#include "interrupts.h"
#else
#include <mutex>
#endif

/*!
    @brief	Instance class for a multi-producer, single-consumer circular queue / ring buffer (FIFO).
            This implementation is lock-free between producers and consumer for the available(), peek(),
            pop(), and push() type functions, but is guarded to safely allow only a single producer
            at any instant.
*/
template< typename T >
class circular_queue_mp : protected circular_queue<T>
{
public:
    circular_queue_mp() = default;
    circular_queue_mp(const size_t capacity) : circular_queue<T>(capacity)
    {}
    circular_queue_mp(circular_queue<T>&& cq) : circular_queue<T>(std::move(cq))
    {}
    using circular_queue<T>::operator=;
    using circular_queue<T>::capacity;
    using circular_queue<T>::flush;
    using circular_queue<T>::available;
    using circular_queue<T>::available_for_push;
    using circular_queue<T>::peek;
    using circular_queue<T>::pop;
    using circular_queue<T>::pop_n;
    using circular_queue<T>::for_each;
    using circular_queue<T>::for_each_rev_requeue;

    /*!
        @brief	Resize the queue. The available elements in the queue are preserved.
                This is not lock-free, but safe, concurrent producer or consumer access
                is guarded.
        @return True if the new capacity could accommodate the present elements in
                the queue, otherwise nothing is done and false is returned.
    */
    bool capacity(const size_t cap)
    {
#ifdef ESP8266
        esp8266::InterruptLock lock;
#else
        std::lock_guard<std::mutex> lock(m_pushMtx);
#endif
        return circular_queue<T>::capacity(cap);
    }

    bool IRAM_ATTR push(T&& val)
    {
#ifdef ESP8266
        esp8266::InterruptLock lock;
#else
        std::lock_guard<std::mutex> lock(m_pushMtx);
#endif
        return circular_queue<T>::push(std::move(val));
    }

    /*!
        @brief	Move the rvalue parameter into the queue, guarded
                for multiple concurrent producers.
        @return true if the queue accepted the value, false if the queue
                was full.
    */
    bool IRAM_ATTR push(const T& val)
    {
#ifdef ESP8266
        esp8266::InterruptLock lock;
#else
        std::lock_guard<std::mutex> lock(m_pushMtx);
#endif
        return circular_queue<T>::push(val);
    }

    /*!
        @brief	Push copies of multiple elements from a buffer into the queue,
                in order, beginning at buffer's head. This is guarded for
                multiple producers, push_n() is atomic.
        @return The number of elements actually copied into the queue, counted
                from the buffer head.
    */
    size_t push_n(const T* buffer, size_t size)
    {
#ifdef ESP8266
        esp8266::InterruptLock lock;
#else
        std::lock_guard<std::mutex> lock(m_pushMtx);
#endif
        return circular_queue<T>::push_n(buffer, size);
    }

    /*!
        @brief	Pops the next available element from the queue, requeues
                it immediately.
        @return A reference to the just requeued element, or the default
                value of type T if the queue is empty.
    */
    T& pop_requeue();

    /*!
        @brief	Iterate over, pop and optionally requeue each available element from the queue,
                calling back fun with a reference of every single element.
                Requeuing is dependent on the return boolean of the callback function. If it
                returns true, the requeue occurs.
    */
    bool for_each_requeue(const std::function<bool(T&)>& fun);

#ifndef ESP8266
protected:
    std::mutex m_pushMtx;
#endif
};

template< typename T >
T& circular_queue_mp<T>::pop_requeue()
{
#ifdef ESP8266
    esp8266::InterruptLock lock;
#else
    std::lock_guard<std::mutex> lock(m_pushMtx);
#endif
    const auto outPos = circular_queue<T>::m_outPos.load(std::memory_order_acquire);
    const auto inPos = circular_queue<T>::m_inPos.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (inPos == outPos) return circular_queue<T>::defaultValue;
    T& val = circular_queue<T>::m_buffer[inPos] = std::move(circular_queue<T>::m_buffer[outPos]);
    const auto bufSize = circular_queue<T>::m_bufSize;
    std::atomic_thread_fence(std::memory_order_release);
    circular_queue<T>::m_outPos.store((outPos + 1) % bufSize, std::memory_order_relaxed);
    circular_queue<T>::m_inPos.store((inPos + 1) % bufSize, std::memory_order_release);
    return val;
}

template< typename T >
bool circular_queue_mp<T>::for_each_requeue(const std::function<bool(T&)>& fun)
{
    auto inPos0 = circular_queue<T>::m_inPos.load(std::memory_order_acquire);
    auto outPos = circular_queue<T>::m_outPos.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (outPos == inPos0) return false;
    do {
        T&& val = std::move(circular_queue<T>::m_buffer[outPos]);
        if (fun(val))
        {
#ifdef ESP8266
            esp8266::InterruptLock lock;
#else
            std::lock_guard<std::mutex> lock(m_pushMtx);
#endif
            std::atomic_thread_fence(std::memory_order_release);
            auto inPos = circular_queue<T>::m_inPos.load(std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_acquire);
            circular_queue<T>::m_buffer[inPos] = std::move(val);
            std::atomic_thread_fence(std::memory_order_release);
            circular_queue<T>::m_inPos.store((inPos + 1) % circular_queue<T>::m_bufSize, std::memory_order_release);
        }
        else
        {
            std::atomic_thread_fence(std::memory_order_release);
        }
        outPos = (outPos + 1) % circular_queue<T>::m_bufSize;
        circular_queue<T>::m_outPos.store(outPos, std::memory_order_release);
    } while (outPos != inPos0);
    return true;
}

#endif // __circular_queue_mp_h
