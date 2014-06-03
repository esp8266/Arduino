/*
  Copyright (c) 2013-2014 Arduino LLC. All right reserved.

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

HttpClient::HttpClient() :
  insecure(false) {
  // Empty
}

unsigned int HttpClient::get(String &url) {
  begin("curl");
  if (insecure) {
    addParameter("-k");
  }
  addHeader();
  addParameter(url);
  return run();
}

unsigned int HttpClient::get(const char *url) {
  begin("curl");
  if (insecure) {
    addParameter("-k");
  }
  addHeader();
  addParameter(url);
  return run();
}

void HttpClient::getAsynchronously(String &url) {
  begin("curl");
  if (insecure) {
    addParameter("-k");
  }
  addHeader();
  addParameter(url);
  runAsynchronously();
}

void HttpClient::getAsynchronously(const char *url) {
  begin("curl");
  if (insecure) {
    addParameter("-k");
  }
  addHeader();
  addParameter(url);
  runAsynchronously();
}

unsigned int HttpClient::post(String &url, String &data) {
    return post(url.c_str(), data.c_str());
}

unsigned int HttpClient::post(const char *url, const char *data) {
  begin("curl");
  if (insecure) {
    addParameter("-k");
  }
  addParameter("--request");
  addParameter("POST");
  addParameter("--data");
  addParameter(data);
  addHeader();
  addParameter(url);
  return run();
}

void HttpClient::postAsynchronously(String &url, String &data) {
  postAsynchronously(url.c_str(), data.c_str());
}

void HttpClient::postAsynchronously(const char *url, const char *data) {
  begin("curl");
  if (insecure) {
    addParameter("-k");
  }
  addParameter("--request");
  addParameter("POST");
  addParameter("--data");
  addParameter(data);
  addHeader();
  addParameter(url);
  runAsynchronously();
}

boolean HttpClient::ready() {
  return !running();
}

unsigned int HttpClient::getResult() {
  return exitValue();
}

void HttpClient::noCheckSSL() {
  insecure = true;
}

void HttpClient::checkSSL() {
  insecure = false;
}

void HttpClient::setHeader(String &header) {
  this->header = header;
}

void HttpClient::setHeader(const char * header) {
  this->header = String(header);
}

void HttpClient::addHeader() {
  if (header.length() > 0) {
    addParameter("--header");
    addParameter(header);
  }
}

