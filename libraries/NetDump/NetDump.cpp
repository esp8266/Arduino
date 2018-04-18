
#include <NetDump.h>

static const char _unk [] PROGMEM = "(unk)";
static const char _right [] PROGMEM = "->";


void netDumpHex (Print& out, const char* data, size_t size, bool show_hex, bool show_ascii, size_t per_line)
{
    size_t start = 0;
    
    while (start < size)
    {
        size_t end = start + per_line;
        if (end > size)
            end = size;
        if (show_hex)
            for (size_t i = start; i < end; i++)
                out.printf("%02x ", (unsigned char)data[i]);
        if (show_ascii)
        {
            if (show_hex)
                for (size_t i = end; i < start + per_line; i++)
                    out.print("   ");
            for (size_t i = start; i < end; i++)
                out.printf("%c", data[i] >= 32 && data[i] < 128? data[i]: '.');
        }
        out.println();

        start += per_line;
    }
}

static void snap (Print& out)
{
    out.println(F("(snap)"));
}

static void netDumpMac (Print& out, const char* data)
{
    for (int i = 0; i < 6; i++)
    {
        out.printf("%02x", (unsigned char)data[i]);
        if (i < 5)
            out.print(':');
    }
}

static void netDumpIPv4 (Print& out, const char* data)
{
    for (int i = 0; i < 4; i++)
    {
        out.printf("%u", (unsigned char)data[i]);
        if (i < 3)
            out.print('.');
    }
}

static void netDumpARP (Print& out, const char* data, size_t size)
{
    out.print(F(" ARP "));
    if (size < 28)
        return;
    if (data[7] == 1)
    {
        out.print(F("who has "));
        netDumpIPv4(out, data + 24);
        out.print(F(" tell "));
        netDumpIPv4(out, data + 14);
    }
    else // if (data[7] == 2)
    {
        netDumpIPv4(out, data + 14);
        out.print(F(" is at "));
        netDumpMac(out, data + 8);
    }
    out.println();
}

static void netDumpICMP (Print& out, const char* data, size_t size)
{
    out.print(F(" ICMP "));
    if (size < 1)
        return snap(out);
        
    out.println(
    	*data == 0? F("ping reply"):
    	*data == 8? F("ping request"):
    	// ...
    	FPSTR(_unk));
}

static void netDumpIGMP (Print& out, const char* data, size_t size)
{
    out.println(F(" IGMP"));
    if (size < 1)
        return snap(out);
    (void)data;
}

uint16_t netDumpSport (const char* data)
{
    return data[1] | (((uint16_t)data[0]) << 8);
}

uint16_t netDumpDport (const char* data)
{
    return netDumpSport(data + 2);
}

static void netDumpPort (Print& out, const char* data)
{
    out.printf("%d->%d", netDumpSport(data), netDumpDport(data));
}

static void netDumpTCP (Print& out, const char* data, size_t size)
{
    out.print(F(" TCP "));
    netDumpPort(out, data);
    // print len, seq, ack, window, flags
    (void)size;
    out.println();
}

static void netDumpUDP (Print& out, const char* data, size_t size)
{
    out.print(F(" UDP "));
    if (size < 8)
        return snap(out);

    netDumpPort(out, data);
    // print len
    out.println();
}

static uint8_t iptype (const char* ipdata)
{
  return ipdata[9];
}

bool netDump_is_ICMP (const char* data) { return iptype(data) == 1; }
bool netDump_is_IGMP (const char* data) { return iptype(data) == 2; }
bool netDump_is_TCP  (const char* data) { return iptype(data) == 6; }
bool netDump_is_UDP  (const char* data) { return iptype(data) == 17; }

static void netDumpIPv4 (Print& out, const char* data, size_t size)
{
    if (size < 20)
        return snap(out);
        
    out.print(F(" IPv4 "));
    
    netDumpIPv4(out, data + 12);
    out.print(FPSTR(_right));
    netDumpIPv4(out, data + 16);

    const char* subdata = data + 20; /*XXX missing option*/
    size_t subsize = size - 20;
    
    if      (netDump_is_ICMP(data)) netDumpICMP(out, subdata, subsize);
    else if (netDump_is_IGMP(data)) netDumpIGMP(out, subdata, subsize);
    else if (netDump_is_TCP(data))  netDumpTCP (out, subdata, subsize);
    else if (netDump_is_UDP(data))  netDumpUDP (out, subdata, subsize);
    else out.println(FPSTR(_unk));
}

static uint16_t ethtype (const char* ethdata)
{
  return ethdata[13] | (((uint16_t)ethdata[12]) << 8);
}

bool netDump_is_ARP  (const char* data) { return ethtype(data) == 0x0806; }
bool netDump_is_IPv4 (const char* data) { return ethtype(data) == 0x0800; }
bool netDump_is_IPv6 (const char* data) { return ethtype(data) == 0x86dd; }

void netDump (Print& out, const char* data, size_t size)
{
    if (size < 14)
        return snap(out);

    //if show-mac
    {
        netDumpMac(out, data + 6);
        out.print(FPSTR(_right));
        netDumpMac(out, data);
    }

    const char* subdata = data + 14;
    size_t subsize = size - 14;

    if      (netDump_is_ARP(data))  netDumpARP (out, subdata, subsize);
    else if (netDump_is_IPv4(data)) netDumpIPv4(out, subdata, subsize);
    else out.println(FPSTR(_unk));
}
