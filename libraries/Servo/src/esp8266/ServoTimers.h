/*
  Copyright (c) 2015 Michael C. Miller. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
  */

//
// Defines for timer abstractions used with  Servo library
//
// ServoTimerSequence enumerates the sequence that the timers should be allocated
// ServoTimerSequence_COUNT indicates how many timers are available.
//
enum ServoTimerSequence {

#if !defined (SERVO_EXCLUDE_TIMER0)
    ServoTimerSequence_Timer0,
#endif

#if !defined (SERVO_EXCLUDE_TIMER1)
    ServoTimerSequence_Timer1,
#endif

    ServoTimerSequence_COUNT
};


#if !defined (SERVO_EXCLUDE_TIMER0)

struct ServoTimer0
{
public:
    ServoTimer0()
    {
        setEndOfCycle();
    }


    uint32_t usToTicks(uint32_t us) const
    {
        return (clockCyclesPerMicrosecond() * us);     // converts microseconds to tick
    }
    uint32_t ticksToUs(uint32_t ticks) const
    {
        return (ticks / clockCyclesPerMicrosecond()); // converts from ticks back to microseconds
    }

    void InitInterrupt(timercallback handler)
    {
        timer0_isr_init();
        timer0_attachInterrupt(handler);
    }

    void ResetInterrupt() {}; // timer0 doesn't have a clear interrupt

    void StopInterrupt()
    {
        timer0_detachInterrupt();
    }

    void SetPulseCompare(uint32_t value)
    {
        timer0_write(ESP.getCycleCount() + value);
    }

    void SetCycleCompare(uint32_t value)
    {
        timer0_write(_cycleStart + value);
    }

    uint32_t GetCycleCount() const
    {
        return ESP.getCycleCount() - _cycleStart;
    }


    void StartCycle()
    {
        _cycleStart = ESP.getCycleCount();
        _currentChannel = 0;
    }

    int8_t getCurrentChannel() const
    {
        return _currentChannel;
    }

    void nextChannel()
    {
        _currentChannel++;
    }

    void setEndOfCycle()
    {
        _currentChannel = -1;
    }

    bool isEndOfCycle() const
    {
        return (_currentChannel == -1);
    }

    ServoTimerSequence timerId() const
    {
        return ServoTimerSequence_Timer0;
    }

private:
    volatile uint32_t _cycleStart;
    volatile int8_t _currentChannel;
};

#endif


#if !defined (SERVO_EXCLUDE_TIMER1)

#define TIMER1_TICKS_PER_US (APB_CLK_FREQ / 1000000L)

struct ServoTimer1
{
public:
    ServoTimer1()
    {
        setEndOfCycle();
    }


    uint32_t usToTicks(uint32_t us) const
    {
        return (TIMER1_TICKS_PER_US / 16 * us);     // converts microseconds to tick
    }
    uint32_t ticksToUs(uint32_t ticks) const
    {
        return (ticks / TIMER1_TICKS_PER_US * 16); // converts from ticks back to microseconds
    }

    void InitInterrupt(timercallback handler)
    {
        timer1_isr_init();
        timer1_attachInterrupt(handler);
        timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
        timer1_write(usToTicks(REFRESH_INTERVAL));
    }

    void ResetInterrupt() {}; // timer1 doesn't have a clear interrupt

    void StopInterrupt()
    {
        timer1_detachInterrupt();
    }

    void SetPulseCompare(uint32_t value)
    {
        _cycleTicks += value;
        timer1_write(value);
    }

    void SetCycleCompare(uint32_t value)
    {
        if (value <= _cycleTicks)
        {
            value = 1;
        }
        else
        {
            value -= _cycleTicks;
        }
        timer1_write(value);
    }

    uint32_t GetCycleCount() const
    {
        return _cycleTicks;
    }


    void StartCycle()
    {
        _cycleTicks = 0;
        _currentChannel = 0;
    }

    int8_t getCurrentChannel() const
    {
        return _currentChannel;
    }

    void nextChannel()
    {
        _currentChannel++;
    }

    void setEndOfCycle()
    {
        _currentChannel = -1;
    }

    bool isEndOfCycle() const
    {
        return (_currentChannel == -1);
    }

    ServoTimerSequence timerId() const
    {
        return ServoTimerSequence_Timer1;
    }

private:
    volatile uint32_t _cycleTicks;
    volatile int8_t _currentChannel;
};

#endif
