/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "HttpClient.h"

unsigned int HttpClient::get(String &url) {
  begin("curl");
  addParameter(url);
  return run();
}

unsigned int HttpClient::get(const char *url) {
  begin("curl");
  addParameter(url);
  return run();
}

void HttpClient::getAsynchronously(String &url) {
  begin("curl");
  addParameter(url);
  runAsynchronously();
}

void HttpClient::getAsynchronously(const char *url) {
  begin("curl");
  addParameter(url);
  runAsynchronously();
}

boolean HttpClient::ready() {
  return running();
}

unsigned int HttpClient::getResult() {
  return exitValue();
}


