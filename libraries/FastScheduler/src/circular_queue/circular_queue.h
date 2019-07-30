/*
circular_queue.h - Implementation of a lock-free circular queue for EspSoftwareSerial.
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

#ifndef __circular_queue_h
#define __circular_queue_h

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <atomic>
#include <memory>
#include <algorithm>
#include <functional>

#if !defined(ESP32) && !defined(ESP8266)
#define ICACHE_RAM_ATTR
#define IRAM_ATTR
#endif

using std::min;

/*!
    @brief	Instance class for a single-producer, single-consumer circular queue / ring buffer (FIFO).
            This implementation is lock-free between producer and consumer for the available(), peek(),
            pop(), and push() type functions.
*/
template< typename T >
class circular_queue
{
public:
    /*!
        @brief	Constructs a valid, but zero-capacity dummy queue.
    */
    circular_queue() : m_bufSize(1)
    {
        m_inPos.store(0);
        m_outPos.store(0);
    }
    /*!
        @brief  Constructs a queue of the given maximum capacity.
    */
    circular_queue(const size_t capacity) : m_bufSize(capacity + 1), m_buffer(new T[m_bufSize])
    {
        m_inPos.store(0);
        m_outPos.store(0);
    }
    circular_queue(circular_queue&& cq) :
        m_bufSize(cq.m_bufSize), m_buffer(cq.m_buffer), m_inPos(cq.m_inPos.load()), m_outPos(cq.m_outPos.load())
    {}
    ~circular_queue()
    {
        m_buffer.reset();
    }
    circular_queue(const circular_queue&) = delete;
    circular_queue& operator=(circular_queue&& cq)
    {
        m_bufSize = cq.m_bufSize;
        m_buffer = cq.m_buffer;
        m_inPos.store(cq.m_inPos.load());
        m_outPos.store(cq.m_outPos.load());
    }
    circular_queue& operator=(const circular_queue&) = delete;

    /*!
        @brief	Get the numer of elements the queue can hold at most.
    */
    size_t capacity() const
    {
        return m_bufSize - 1;
    }

    /*!
        @brief	Resize the queue. The available elements in the queue are preserved.
                This is not lock-free and concurrent producer or consumer access
                will lead to corruption.
        @return True if the new capacity could accommodate the present elements in
                the queue, otherwise nothing is done and false is returned.
    */
    bool capacity(const size_t cap);

    /*!
        @brief	Discard all data in the queue.
    */
    void flush()
    {
        m_outPos.store(m_inPos.load());
    }

    /*!
        @brief	Get a snapshot number of elements that can be retrieved by pop.
    */
    size_t available() const
    {
        int avail = static_cast<int>(m_inPos.load() - m_outPos.load());
        if (avail < 0) avail += m_bufSize;
        return avail;
    }

    /*!
        @brief	Get the remaining free elementes for pushing.
    */
    size_t available_for_push() const
    {
        int avail = static_cast<int>(m_outPos.load() - m_inPos.load()) - 1;
        if (avail < 0) avail += m_bufSize;
        return avail;
    }

    /*!
        @brief	Peek at the next element pop will return without removing it from the queue.
        @return An rvalue copy of the next element that can be popped. If the queue is empty,
                return an rvalue copy of the element that is pending the next push.
    */
    T peek() const
    {
        const auto outPos = m_outPos.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        return m_buffer[outPos];
    }

    /*!
        @brief	Peek at the next pending input value.
        @return A reference to the next element that can be pushed.
    */
    T& IRAM_ATTR pushpeek()
    {
        const auto inPos = m_inPos.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        return m_buffer[inPos];
    }

    /*!
        @brief	Release the next pending input value, accessible by pushpeek(), into the queue.
        @return true if the queue accepted the value, false if the queue
                was full.
    */
    bool IRAM_ATTR push();

    /*!
        @brief	Move the rvalue parameter into the queue.
        @return true if the queue accepted the value, false if the queue
                was full.
    */
    bool IRAM_ATTR push(T&& val);

    /*!
        @brief	Push a copy of the parameter into the queue.
        @return true if the queue accepted the value, false if the queue
                was full.
    */
    bool IRAM_ATTR push(const T& val)
    {
        return push(T(val));
    }

    /*!
        @brief	Push copies of multiple elements from a buffer into the queue,
                in order, beginning at buffer's head.
        @return The number of elements actually copied into the queue, counted
                from the buffer head.
    */
    size_t push_n(const T* buffer, size_t size);

    /*!
        @brief	Pop the next available element from the queue.
        @return An rvalue copy of the popped element, or a default
                value of type T if the queue is empty.
    */
    T pop();

    /*!
        @brief	Pop multiple elements in ordered sequence from the queue to a buffer.
        @return The number of elements actually popped from the queue to
                buffer.
    */
    size_t pop_n(T* buffer, size_t size);

    /*!
        @brief	Iterate over and remove each available element from queue,
                calling back fun with an rvalue reference of every single element.
    */
    void for_each(const std::function<void(T&&)>& fun);

    /*!
        @brief	In reverse order, iterate over, pop and optionally requeue each available element from the queue,
                calling back fun with a reference of every single element.
                Requeuing is dependent on the return boolean of the callback function. If it
                returns true, the requeue occurs.
    */
    bool for_each_rev_requeue(const std::function<bool(T&)>& fun);

protected:
    const T defaultValue = {};
    unsigned m_bufSize;
    std::unique_ptr<T[] > m_buffer;
    std::atomic<unsigned> m_inPos;
    std::atomic<unsigned> m_outPos;
};

template< typename T >
bool circular_queue<T>::capacity(const size_t cap)
{
    if (cap + 1 == m_bufSize) return true;
    else if (available() > cap) return false;
    std::unique_ptr<T[] > buffer(new T[cap + 1]);
    const auto available = pop_n(buffer, cap);
    m_buffer.reset(buffer);
    m_bufSize = cap + 1;
    std::atomic_thread_fence(std::memory_order_release);
    m_inPos.store(available, std::memory_order_relaxed);
    m_outPos.store(0, std::memory_order_release);
    return true;
}

template< typename T >
bool IRAM_ATTR circular_queue<T>::push()
{
    const auto inPos = m_inPos.load(std::memory_order_acquire);
    const unsigned next = (inPos + 1) % m_bufSize;
    if (next == m_outPos.load(std::memory_order_relaxed)) {
        return false;
    }

    std::atomic_thread_fence(std::memory_order_acquire);

    m_inPos.store(next, std::memory_order_release);
    return true;
}

template< typename T >
bool IRAM_ATTR circular_queue<T>::push(T&& val)
{
    const auto inPos = m_inPos.load(std::memory_order_acquire);
    const unsigned next = (inPos + 1) % m_bufSize;
    if (next == m_outPos.load(std::memory_order_relaxed)) {
        return false;
    }

    std::atomic_thread_fence(std::memory_order_acquire);

    m_buffer[inPos] = std::move(val);

    std::atomic_thread_fence(std::memory_order_release);

    m_inPos.store(next, std::memory_order_release);
    return true;
}

template< typename T >
size_t circular_queue<T>::push_n(const T* buffer, size_t size)
{
    const auto inPos = m_inPos.load(std::memory_order_acquire);
    const auto outPos = m_outPos.load(std::memory_order_relaxed);

    size_t blockSize = (outPos > inPos) ? outPos - 1 - inPos : (outPos == 0) ? m_bufSize - 1 - inPos : m_bufSize - inPos;
    blockSize = min(size, blockSize);
    if (!blockSize) return 0;
    int next = (inPos + blockSize) % m_bufSize;

    std::atomic_thread_fence(std::memory_order_acquire);

    auto dest = m_buffer.get() + inPos;
    std::copy_n(std::make_move_iterator(buffer), blockSize, dest);
    size = min(size - blockSize, outPos > 1 ? static_cast<size_t>(outPos - next - 1) : 0);
    next += size;
    dest = m_buffer.get();
    std::copy_n(std::make_move_iterator(buffer + blockSize), size, dest);

    std::atomic_thread_fence(std::memory_order_release);

    m_inPos.store(next, std::memory_order_release);
    return blockSize + size;
}

template< typename T >
T circular_queue<T>::pop()
{
    const auto outPos = m_outPos.load(std::memory_order_acquire);
    if (m_inPos.load(std::memory_order_relaxed) == outPos) return defaultValue;

    std::atomic_thread_fence(std::memory_order_acquire);

    auto val = std::move(m_buffer[outPos]);

    std::atomic_thread_fence(std::memory_order_release);

    m_outPos.store((outPos + 1) % m_bufSize, std::memory_order_release);
    return val;
}

template< typename T >
size_t circular_queue<T>::pop_n(T* buffer, size_t size) {
    size_t avail = size = min(size, available());
    if (!avail) return 0;
    const auto outPos = m_outPos.load(std::memory_order_acquire);
    size_t n = min(avail, static_cast<size_t>(m_bufSize - outPos));

    std::atomic_thread_fence(std::memory_order_acquire);

    buffer = std::copy_n(std::make_move_iterator(m_buffer.get() + outPos), n, buffer);
    avail -= n;
    std::copy_n(std::make_move_iterator(m_buffer.get()), avail, buffer);

    std::atomic_thread_fence(std::memory_order_release);

    m_outPos.store((outPos + size) % m_bufSize, std::memory_order_release);
    return size;
}

template< typename T >
void circular_queue<T>::for_each(const std::function<void(T&&)>& fun)
{
    auto outPos = m_outPos.load(std::memory_order_acquire);
    const auto inPos = m_inPos.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    while (outPos != inPos)
    {
        fun(std::move(m_buffer[outPos]));
        std::atomic_thread_fence(std::memory_order_release);
        outPos = (outPos + 1) % m_bufSize;
        m_outPos.store(outPos, std::memory_order_release);
    }
}

template< typename T >
bool circular_queue<T>::for_each_rev_requeue(const std::function<bool(T&)>& fun)
{
    auto inPos0 = circular_queue<T>::m_inPos.load(std::memory_order_acquire);
    auto outPos = circular_queue<T>::m_outPos.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (outPos == inPos0) return false;
    auto pos = inPos0;
    auto outPos1 = inPos0;
    const auto posDecr = circular_queue<T>::m_bufSize - 1;
    do {
        pos = (pos + posDecr) % circular_queue<T>::m_bufSize;
        T&& val = std::move(circular_queue<T>::m_buffer[pos]);
        if (fun(val))
        {
            outPos1 = (outPos1 + posDecr) % circular_queue<T>::m_bufSize;
            if (outPos1 != pos) circular_queue<T>::m_buffer[outPos1] = std::move(val);
        }
    } while (pos != outPos);
    circular_queue<T>::m_outPos.store(outPos1, std::memory_order_release);
    return true;
}

#endif // __circular_queue_h
