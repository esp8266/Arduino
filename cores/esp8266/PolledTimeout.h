#ifndef __POLLEDTIMING_H__
#define __POLLEDTIMING_H__

namespace esp8266
{

template<bool PeriodicT>  
class polledTimeout
{
public:
  using timeType = unsigned int;
  
  polledTimeout(timeType timeout) 
    : _timeout(timeout), _start(millis())  
  {} 

  bool expired()
  {
    if(PeriodicT)
      return expiredRetrigger();
    return expiredOneShot();
  }
  
  operator bool()
  {
    return expired(); 
  }
  
  bool reset()
  {
    _start = millis();  
  }
  
protected:
  bool checkExpired(timeType t) const
  {
    return (t - _start) >=  _timeout;
  }
  
  bool expiredRetrigger()
  {
    timeType current = millis();
    if(checkExpired(current))
    {
      unsigned int n = (current - _start) / _timeout; //how many _timeouts periods have elapsed, will usually be 1 (current - _start >= _timeout)
      _start += n  * _timeout;
      return true;
    }
    return false;
  }
  
  bool expiredOneShot() const
  {
    return checkExpired(millis());
  }
  
  timeType _timeout;
  timeType _start;
};


using polledTimeoutOneShot = polledTimeout<false>;
using polledTimeoutPeriodic = polledTimeout<true>;


}//esp8266

#endif
