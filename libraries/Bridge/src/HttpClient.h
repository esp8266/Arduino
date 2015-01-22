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

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <Process.h>

class HttpClient : public Process {
  public:
    HttpClient();

    unsigned int get(String &url);
    unsigned int get(const char * url);
    void getAsynchronously(String &url);
    void getAsynchronously(const char * url);
    unsigned int post(String &url, String &data);
    unsigned int post(const char * url, const char * data);
    void postAsynchronously(String &url, String &data);
    void postAsynchronously(const char * url, const char * data);
    void setHeader(String &header);
    void setHeader(const char * header);
    boolean ready();
    unsigned int getResult();
    void noCheckSSL();
    void checkSSL();

  private:
    boolean insecure;

  private:
    void addHeader();
    String header;
};

#endif /* HTTPCLIENT_H_ */
