/** Is this an IP? */
boolean isIp(const String& str) {
  return IPAddress().fromString(str);
}

/** IP to String? */
String toStringIp(const IPAddress& ip) {
  return ip.toString();
}

