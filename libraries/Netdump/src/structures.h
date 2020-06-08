/*
 * structures.h
 *
 *  Created on: 5 mei 2020
 *      Author: Herman
 */

#ifndef LIBRARIES_NETDUMP_SRC_STRUCTURES_H_
#define LIBRARIES_NETDUMP_SRC_STRUCTURES_H_

#include "Arduino.h"
#include "NetdumpUtils.h"
#include "NetdumpIP.h"

namespace NetCapture
{

class EthernetFrame
{
public:

	struct EthernetHeader
	{
		uint8_t destinationMAC[6];
		uint8_t sourceMAC[6];
		uint8_t type[2];
		uint8_t payload[];
	};

	EthernetFrame(const uint8_t* frame)
	{
		hdr = reinterpret_cast<const EthernetHeader*>(frame);
		raw = frame;
	};
	~EthernetFrame(){};

	const EthernetHeader* hdr;
	const uint8_t* raw;

	String destinationMAC() {return "";}
	const uint8_t* payload() { return hdr->payload;}
	uint16_t type() {return NetdumpUtils::ntoh16(hdr->type);}
};

class ARPPacket
{
public:
	struct ARPHeader
	{
		uint8_t hardwareType[2];
		uint8_t protocolType[2];
		uint8_t hardwareAddressLength;
		uint8_t protocolAddressLength;
		uint8_t opcode[2];
		uint8_t senderHardwareAddress[6];
		uint8_t senderProtocolAddress[4];
		uint8_t targetHardwareAddress[6];
		uint8_t targetProtocolAddress[4];
	};

	ARPPacket(const uint8_t* packet)
	{
		hdr = reinterpret_cast<const ARPHeader*>(packet);
		raw = packet;
	}

	const ARPHeader* hdr;
	const uint8_t* raw;

	uint16_t opcode()
	{
		return NetdumpUtils::ntoh16(hdr->opcode);
	}
	NetdumpIP targetIP()
	{
		return NetdumpIP(hdr->targetProtocolAddress);
	}
	NetdumpIP senderIP()
	{
		return NetdumpIP(hdr->senderProtocolAddress);
	}
};

class IPv4Packet
{
public:
	struct IPv4Header
	{
//		uint8_t version:4;
//		uint8_t headerLength:4;
		uint8_t version_header;
//		uint8_t serviceType:6;
//		uint8_t ecn:2;
		uint8_t type_ecn;
		uint8_t totalLength[2];
		uint8_t identification[2];
//		uint8_t flags:3;
//		uint16_t fragmentOffset:13;
		uint8_t flags_fragment[2];
		uint8_t ttl;
		uint8_t protocol;
		uint8_t headerChecksum[2];
		uint8_t sourceIP[4];
		uint8_t destinationIP[4];
		uint8_t payload[];
	};
	IPv4Packet(const uint8_t* packet)
	{
		hdr = reinterpret_cast<const IPv4Header*>(packet);
		raw = packet;
	}

	const IPv4Header* hdr;
	const uint8_t* raw;

	uint16_t headerLength()
	{
		return (hdr->version_header & 0x0f) << 2;
	}
	uint8_t ipVersion()
	{
		return (hdr->version_header >> 4) & 0x0f;
	}
	uint8_t serviceType()
	{
		return (hdr->type_ecn >> 2) & 0x3f;
	}
	uint8_t flags()
	{
		return (NetdumpUtils::ntoh16(hdr->flags_fragment) >> 13) & 0x07;
	}
	uint16_t fragmentationOffset()
	{
		return (NetdumpUtils::ntoh16(hdr->flags_fragment) & 0x1fff);
	}
	NetdumpIP sourceIP()
	{
		return NetdumpIP(hdr->sourceIP);
	}
	NetdumpIP destinationIP()
	{
		return NetdumpIP(hdr->destinationIP);
	}
};

class IPv6Packet
{
public:

	struct IPv6Header
	{
		uint8_t version_traffic_flow[4];
		uint8_t payloadLength[2];
		uint8_t nextHeader;
		uint8_t hopLimit;
		uint8_t sourceAddress[16];
		uint8_t destinationAddress[16];
		uint8_t payload[];
	};

	IPv6Packet(const uint8_t* data)
	  : raw(data),
		hdr(reinterpret_cast<const IPv6Header*>(data))
	{};

	const uint8_t* raw;
	const IPv6Header* hdr;

	uint8_t type()
	{
		return hdr->nextHeader;
	}
	NetdumpIP sourceIP()
	{
		return NetdumpIP(hdr->sourceAddress,false);
	}
	NetdumpIP destinationIP()
	{
		return NetdumpIP(hdr->destinationAddress,false);
	}
};

class IPPacket
{
public:
	IPPacket(std::unique_ptr<IPv4Packet>& i4) : ip4(true)
	{
		ipv4.reset(new IPv4Packet(i4->raw));
	};
	IPPacket(std::unique_ptr<IPv6Packet>& i6) : ip4(false)
	{
		ipv6.reset(new IPv6Packet(i6->raw));
	};

	union ipv4v6
	{
		IPv4Packet::IPv4Header* ipv4h;
		IPv6Packet::IPv6Header* ipv6h;
	};

	ipv4v6 ht;

	bool ip4 = false;
	std::unique_ptr<IPv4Packet> ipv4 = nullptr;
	std::unique_ptr<IPv6Packet> ipv6 = nullptr;

	uint8_t packetType()
	{
		return ip4 ? ipv4->hdr->protocol : ipv6->type();
	}
	NetdumpIP sourceIP()
	{
		return ip4 ? ipv4->sourceIP() : ipv6->sourceIP();
	}
	NetdumpIP destinationIP()
	{
		return ip4 ? ipv4->destinationIP() : ipv6->destinationIP();
	}
	const uint8_t* payload()
	{
		return ip4 ? ipv4->hdr->payload : ipv6->hdr->payload;
	}
	const uint8_t* raw()
	{
		return ip4 ? ipv4->raw : ipv6->raw;
	}
};

class TCPPacket
{
public:
	struct TCPHeader
	{
		uint8_t sourcePort[2];
		uint8_t destinationPort[2];
		uint8_t sequenceNumber[4];
		uint8_t acknowledgementNumber[4];
		uint8_t dataOffset:4;
		uint8_t reserved:6;
		uint8_t controlFlags:6;
		uint8_t window[2];
		uint8_t checksum[2];
		uint8_t urgentPointer[2];
		uint8_t payload[];
	};

	TCPPacket(const uint8_t* packet)
	{
		hdr = reinterpret_cast<const TCPHeader*>(packet);
		raw = packet;
	}

	const TCPHeader* hdr;
	const uint8_t* raw;

	uint16_t sourcePort() const
	{
		return NetdumpUtils::ntoh16(hdr->sourcePort);
	}
	uint16_t destinationPort() const
	{
		return NetdumpUtils::ntoh16(hdr->destinationPort);
	}
	uint32_t sequenceNumber() const
	{
		return NetdumpUtils::ntoh32(hdr->sequenceNumber);
	}
};

class UDPPacket
{
public:
	struct UDPHeader
	{
		uint8_t sourcePort[2];
		uint8_t destinationPort[2];
		uint8_t length[2];
		uint8_t checksum[2];
		uint8_t payload[];
	};

	UDPPacket(const uint8_t* packet)
	{
		hdr = reinterpret_cast<const UDPHeader*>(packet);
		raw = packet;
	}

	const UDPHeader* hdr;
	const uint8_t* raw;

	uint16_t sourcePort()
	{
		return NetdumpUtils::ntoh16(hdr->sourcePort);
	}
	uint16_t destinationPort()
	{
		return NetdumpUtils::ntoh16(hdr->destinationPort);
	}
	uint16_t length()
	{
		return NetdumpUtils::ntoh16(hdr->length);
	}
	uint16_t checksum()
	{
		return NetdumpUtils::ntoh16(hdr->checksum);
	}
	uint16_t hdrLength()
	{
		return 8;
	}
};

class DNSPacket
{
public:

	struct DNSHeaderFlags
	{
		uint8_t opcode:4;
		uint8_t aa:1;
		uint8_t tc:1;
		uint8_t rd:1;
		uint8_t z:1;
		uint8_t ad:1;
		uint8_t cd:1;
		uint8_t rcode:4;
	};

	struct DNSHeader
	{
		uint8_t id[2];
		uint8_t flags[2];
		uint8_t qdcount[2];
		uint8_t ancount[2];
		uint8_t nscount[2];
		uint8_t arcount[2];
		uint8_t questions[];
		uint8_t details[];
	};

	struct DNSQuestion
	{
		String qname = "";
		uint16_t qtype = 0;
		uint16_t qclass = 0;
	};

	struct DNSAnswer
	{
		String name;
		uint16_t type;
		uint16_t aclass;
		uint32_t ttl;
		uint16_t rdlength;
		const uint8_t* rdata;

		NetCapture::NetdumpIP getIP()
		{
			if (type == 1)
			{
				return NetdumpIP(rdata[0],rdata[1],rdata[2],rdata[3]);
			}
			else if (type == 28)
			{
				return NetdumpIP(rdata, false);
			}
			else
			{
				return NetdumpIP();
			}
		}

	};

	DNSPacket(const uint8_t* packet)
	  : raw(packet),
		hdr(reinterpret_cast<const DNSHeader*>(packet))
	{};

	const uint8_t* raw;
	const DNSHeader* hdr;

	uint16_t id()
	{
		return (NetdumpUtils::ntoh16(hdr->id));
	}
	uint16_t qdcount()
	{
		return (NetdumpUtils::ntoh16(hdr->qdcount));
	}
	uint16_t ancount()
	{
		return (NetdumpUtils::ntoh16(hdr->ancount));
	}
	uint16_t nscount()
	{
		return (NetdumpUtils::ntoh16(hdr->nscount));
	}
	uint16_t arcount()
	{
		return (NetdumpUtils::ntoh16(hdr->arcount));
	}
	uint16_t flags()
	{
		return (NetdumpUtils::ntoh16(hdr->flags));
	}

	using questionResult = std::tuple<DNSQuestion,const uint8_t*>;

	DNSQuestion getQuestion(uint8_t questionIdx)
	{
		if (questionIdx >= qdcount())
		{
			Serial.printf("Question index too high %d\r\n",questionIdx);
			return DNSQuestion();
		}

		const uint8_t* qPtr = hdr->details;
		DNSQuestion dq;

		for (uint8_t idx=0;idx<=questionIdx;idx++)
		{
			std::tie(dq,qPtr) = getQuestion1(qPtr);
		}
		return dq;
	}

	using qNameResult = std::tuple<String,const uint8_t*>;

	questionResult getQuestion1(const uint8_t* questionPtr)
	{
		DNSQuestion dq;

		String n;
		const uint8_t* p;
		std::tie(n,p) = qName(questionPtr); // c++17 : auto [n,p] = qName(questionPtr);

		dq.qname = n;
		dq.qtype = NetdumpUtils::ntoh16(p);
		dq.qclass = NetdumpUtils::ntoh16(&p[2]);
		return std::make_tuple(dq,p+4);
	}

	qNameResult qName(const uint8_t* qname)
	{
	    String qn;
	    const uint8_t* returnvalue = nullptr;
	    while (*qname != 0x00)
	    {
	    	if (((*qname) & 0xc0) == 0)
	    	{
	    		if (qn.length() != 0)
	    		{
	    			qn += String('.');
	    		}
	    		uint8_t l = *qname;
	    		qname++;
		    	for(int idx=0;idx<l;idx++)
		    	{
		    		qn += (char)*(qname);
		    		qname++;
		    	}
	    	}
	    	else
	    	{
	    		if (!returnvalue)
	    		{
	    			returnvalue = qname + 2;
	    		}
	    		uint8_t offset = qname[1] + (qname[0] & 0x3f);
				qname = &raw[offset];
	    	}
	    }
	    //qname++; // get past the 0x00
	    return std::make_tuple(qn,returnvalue ? returnvalue : qname+1);
	}

	using answerResult = std::tuple<DNSAnswer,const uint8_t*>;

	DNSAnswer getAnswer(uint8_t answerIdx)
	{
		if (answerIdx >= ancount())
		{
			Serial.printf("answer index too high %d\r\n",answerIdx);
			return DNSAnswer();
		}

		const uint8_t* qPtr = hdr->details;
		DNSQuestion dq;

		for (uint8_t idx=0;idx<qdcount();idx++)
		{
			std::tie(dq,qPtr) = getQuestion1(qPtr);
		}

		DNSAnswer da;
		for (uint8_t idx=0;idx<=answerIdx;idx++)
		{
			std::tie(da,qPtr) = getAnswer1(qPtr);
		}

		return da;
	}

	answerResult getAnswer1(const uint8_t* answerPtr)
	{
		DNSAnswer da;

		String n;
		const uint8_t* p;

		std::tie(n,p) = qName(answerPtr); // c++17 : auto [n,p] = qName(questionPtr);
		da.name = n;
		da.type = NetdumpUtils::ntoh16(p);
		da.ttl = NetdumpUtils::ntoh32(p+4);
		da.rdlength = NetdumpUtils::ntoh16(p+8);
		uint16_t next = NetdumpUtils::ntoh16(p+8)+10;
		da.rdata = p+10;
		return std::make_tuple(da,p+next);
	}


};

class ICMPPacket
{
public:
	struct ICMPHeader
	{
		uint8_t type;
		uint8_t code;
		uint8_t checksum[2];
		uint8_t unused[4];
		uint8_t payload[];
	};

	ICMPPacket(const uint8_t* packet)
	{
		hdr = reinterpret_cast<const ICMPHeader*>(packet);
		raw = packet;
	}

	const ICMPHeader* hdr;
	const uint8_t* raw;

};

} // namespace NetCapture

#endif /* LIBRARIES_NETDUMP_SRC_STRUCTURES_H_ */
