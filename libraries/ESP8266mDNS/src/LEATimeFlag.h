/*
 * LEATimeFlag.h
 */
#ifndef __LEATIMEFLAG_H
#define __LEATIMEFLAG_H


#include <Arduino.h>


/**
 * clsLEATimeFlag
 */
class clsLEATimeFlag {
public:
    // constructor
    clsLEATimeFlag(unsigned long p_ulTimeout = (unsigned long)(-1))
    :   m_ulStart(millis()),
        m_ulTimeout(p_ulTimeout) {
    }
    // operator bool
    operator bool(void) const {
        return flagged();
    }
    // flagged
    bool flagged(void) const {
        return ((millis() - m_ulStart) > m_ulTimeout);
    }
    // restart
    void restart(unsigned long p_ulNewTimeout = (unsigned long)(-1)) {
        if ((unsigned long)(-1) != p_ulNewTimeout) {
            m_ulTimeout = p_ulNewTimeout;
        }
        m_ulStart = millis();
    }
    void reset(void) {
        m_ulTimeout = (unsigned long)(-1);
        m_ulStart = millis();
    }
    
    unsigned long start(void) const {
        return m_ulStart;
    }
    unsigned long timeout(void) const {
        return m_ulTimeout;
    }
    bool hypotheticalTimeout(unsigned long p_ulHypotheticalTimeout) const {
        return ((millis() - m_ulStart) > p_ulHypotheticalTimeout);
    }

protected:
    unsigned long   m_ulStart;
    unsigned long   m_ulTimeout;
};

#endif  // __LEATIMEFLAG_H






