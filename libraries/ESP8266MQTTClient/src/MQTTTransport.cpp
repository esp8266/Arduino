#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <StreamString.h>
#include <base64.h>
#include "ESP8266MQTTClient.h"
#include "MQTTTransport.h"

MQTTTransportTraits::~MQTTTransportTraits()
{
}

std::unique_ptr<WiFiClient> MQTTTransportTraits::create()
{
	return std::unique_ptr<WiFiClient>(new WiFiClient());
}

bool MQTTTransportTraits::verify(WiFiClient& client, const char* host)
{
	return true;
}


MQTTTLSTraits::MQTTTLSTraits(const String& fingerprint) :
	_fingerprint(fingerprint)
{
}

std::unique_ptr<WiFiClient> MQTTTLSTraits::create()
{
	return std::unique_ptr<WiFiClient>(new WiFiClientSecure());
}

bool MQTTTLSTraits::verify(WiFiClient& client, const char* host)
{
	auto wcs = reinterpret_cast<WiFiClientSecure&>(client);
	return wcs.verify(_fingerprint.c_str(), host);
}
