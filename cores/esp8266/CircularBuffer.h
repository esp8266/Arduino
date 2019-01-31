/*
 CircularBuffer.h - yet another circular buffer,
               compatible with Arduino Stream transfer API
 Copyright (c) 2019 david gauchard.  All right reserved.

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

#include <Stream.h>

// CONSTst (CONST-for-stream) should be 'const',
// Stream&Print should be updated accordingly (or Ardui-not).
#define CONSTst //const

template <int m_len, typename idx_t = uint16_t>
class CircularBuffer: public Stream
{
protected:

    idx_t m_posr, m_posw;
    uint8_t m_buffer[m_len];
    bool m_full;

    void inc_posr (idx_t contig = 1)
    {
        if ((m_posr += contig) == m_len)
            m_posr = 0;
        m_full = false;
    }

    void inc_posw (idx_t contig = 1)
    {
        if ((m_posw += contig) == m_len)
            m_posw = 0;
        if (m_posw == m_posr)
            m_full = true;
    }

    idx_t availableForWriteContig () const
    {
        if (full())
            return 0;
        if (m_posr > m_posw)
            return m_posr - m_posw;
        return m_len - m_posw;
    }

    idx_t availableForReadContig () const
    {
        if (full())
            return m_len;
        if (m_posr > m_posw)
            return m_len - m_posr;
        return m_posw - m_posr;
    }

public:

    CircularBuffer (): m_posr(0), m_posw(0), m_full(false) { }

    size_t length () const
    {
        return m_len;
    }

    bool full () const
    {
        return m_full;
    }

    bool empty () const
    {
        return m_posw == m_posr && !m_full;
    }

    int availableForWrite () CONSTst override
    {
        if (empty())
            return m_len;
        if (m_posw > m_posr)
            return (m_len - m_posw) + (m_posr - 0);
        return m_posr - m_posw;
    }

    int available () CONSTst override
    {
        if (full())
            return m_len;
        if (m_posr > m_posw)
            return (m_len - m_posr) + (m_posw - 0);
        return m_posw - m_posr;
    }

    // char/byte oriented

    int peek () CONSTst override
    {
        return empty()? -1 : m_buffer[m_posr];
    }

    int read () override
    {
        if (empty())
            return -1;
        int c = m_buffer[m_posr];
        inc_posr();
        return c;
    }

    size_t write (uint8_t c) override
    {
        if (full())
            return 0;
        m_buffer[m_posw] = c;
        inc_posw();
        return 1;
    }

    // buffer oriented

    size_t peek (uint8_t* buffer, size_t len) const /*override*/
    {
        if (empty())
            return 0;
        idx_t chunk1 = availableForReadContig();
        if (chunk1 > len)
            chunk1 = len;
        memcpy(buffer, &m_buffer[m_posr], chunk1);
        return chunk1;
    }

    size_t peekMaxSize () const
    {
        return availableForReadContig();
    }

    const uint8_t* peekPtr () const
    {
        return &m_buffer[m_posr];
    }

    size_t read (char* buffer, size_t maxLen) override
    {
        idx_t copied = 0;
        while (copied < maxLen)
        {
            idx_t contig = availableForReadContig();
            if (!contig)
                break;
            if (contig > maxLen - copied)
                contig = maxLen - copied;
            memcpy(buffer + copied, &m_buffer[m_posr], contig);
            copied += contig;
            inc_posr(contig);
        }
        return copied;
    }

    size_t write (const uint8_t* buffer, size_t maxLen) override
    {
        idx_t copied = 0;
        while (copied < maxLen)
        {
            idx_t contig = availableForWriteContig();
            if (!contig)
                break;
            if (contig > maxLen - copied)
                contig = maxLen - copied;
            memcpy(&m_buffer[m_posw], buffer + copied, contig);
            copied += contig;
            inc_posw(contig);
        }
        return copied;
    }
};
