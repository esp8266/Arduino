/*
 * LEATimeFlag.h
 */
#ifndef __MDNSTIMEFLAG_H
#define __MDNSTIMEFLAG_H


#include <limits>
#include <PolledTimeout.h>


/* Wrapper class around PolledTimeout
 * MDNS requires behavior that is slightly different from the default in PolledTimeout
 */
class clsMDNSTimeFlag {
protected:
    using oneShot = esp8266::polledTimeout::oneShot;
    oneShot m_clsPolledTimeout;

public:
    using timeType = oneShot::timeType;

    clsMDNSTimeFlag(timeType p_Timeout)
      : m_clsPolledTimeout(p_Timeout) {
    }
    clsMDNSTimeFlag()
      : m_clsPolledTimeout(std::numeric_limits<timeType>::max()) {
    }

    operator bool() const {
        return flagged();
    }

    bool flagged() const {
        return m_clsPolledTimeout.checkExpired(millis());
    }
  
    void restart() {
        m_clsPolledTimeout.reset();
    }

    void restart(const timeType p_Timeout) {
        m_clsPolledTimeout.reset(p_Timeout);
    }

    void reset() {
        m_clsPolledTimeout.reset(std::numeric_limits<timeType>::max());
    }

    timeType getTimeout() const {
        return m_clsPolledTimeout.getTimeout();
    }

    bool hypotheticalTimeout(const timeType p_Timeout) const {
      return m_clsPolledTimeout.checkExpired(p_Timeout);
    }
};


#endif  // __MDNSTIMEFLAG_H
