#include <ESP8266mDNS.h>

/*
 * MDNS responder global instance
 *
 * Class type that is instantiated depends on the type mapping in ESP8266mDNS.h
 */
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MDNS)
MDNSResponder MDNS;
#endif

