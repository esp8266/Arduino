#ifndef RFC1305_H
#define RFC1305_H
/*
    see https://www.eecis.udel.edu/~mills/database/rfc/rfc1305/rfc1305c.pdf
    https://tools.ietf.org/html/rfc1305
*/

#pragma pack(1)
struct sRFC1305
{
    // NOTE all fields are BIG-ENDIAN so must be swapped on little endian machines
    uint8_t MODE: 3;
    uint8_t VN: 3;
    uint8_t LI: 2;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    int16_t rootdelay_main;
    uint16_t rootdelay_fraction;
    int16_t rootdispersion_main;
    uint16_t rootdispersion_fraction;
    uint8_t identifier[4];
    // 64 bit timestamps contain 32 bit whole part + 32 bit fractional part
    uint32_t referencetimestamp_main;
    uint32_t referencetimestamp_fraction;
    uint32_t origintimestamp_main;
    uint32_t origintimestamp_fraction;
    uint32_t receivetimestamp_main;
    uint32_t receivetimestamp_fraction;
    uint32_t transmittimestamp_main;
    uint32_t transmittimestamp_fraction;
};
#pragma pack(0)

#define LI_NOWARNING 0
#define LI_61_SEC 1
#define LI_59_SEC 2
#define LI_ALARM 3

#define VERN 4

#define MODE_SYMMETRIC_ACTIVE 1
#define MODE_SYMMETRIC_PASSIVE 2
#define MODE_CLIENT 3
#define MODE_SERVER 4
#define MODE_BROADCAST 5

#define ENDIAN_SWAP_32(l) ((l>>24) |((l>>16)<<8)&0xff00 | ((l>>8)<<16)&0xff0000 | (l << 24))
#define ENDIAN_SWAP_16(l) ((l>>8) | (l << 8))
#endif

