/*
  ESP8266WebServerSecure.h - Dead simple HTTPS web-server.
  Supports only one simultaneous client, knows how to handle GET and POST.

  Copyright (c) 2017 Earle F. Philhower, III. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>

namespace axTLS {
  using ESP8266WebServerSecure = ESP8266WebServerTemplate<WiFiServerSecure>;
};

namespace BearSSL {
  using ESP8266WebServerSecure = ESP8266WebServerTemplate<WiFiServerSecure>;
};
