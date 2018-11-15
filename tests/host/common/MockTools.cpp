
#include <arpa/inet.h>
#include <stdarg.h>

extern "C"
{

uint32_t lwip_htonl (uint32_t hostlong)  { return htonl(hostlong);  }
uint16_t lwip_htons (uint16_t hostshort) { return htons(hostshort); }
uint32_t lwip_ntohl (uint32_t netlong)   { return ntohl(netlong);   }
uint16_t lwip_ntohs (uint16_t netshort)  { return ntohs(netshort);  }

char* ets_strcpy (char* d, const char* s) { return strcpy(d, s); }
size_t ets_strlen (const char* s) { return strlen(s); }

int ets_printf (const char* fmt, ...)
{
        va_list ap;
        va_start(ap, fmt);
	int len = vprintf(fmt, ap);
	va_end(ap);
	return len;
}

extern "C" void configTime(long timezone, int daylightOffset_sec,
                           const char* server1, const char* server2, const char* server3)
{
	(void)server1;
	(void)server2;
	(void)server3;
	
	fprintf(stderr, MOCK "configTime: TODO (tz=%dH offset=%dS) (time will be host's)\n", (int)timezone, daylightOffset_sec);
}

};
