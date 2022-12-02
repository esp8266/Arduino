/*
  WiFiClient.h - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

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

  Modified by Ivan Grokhotkov, December 2014 - esp8266 support
*/

#ifndef wificlient_h
#define wificlient_h
#include <memory>
#include "Arduino.h"
#include "Print.h"
#include "Client.h"
#include "IPAddress.h"
#include "include/slist.h"

#ifndef TCP_MSS
#define TCP_MSS 1460 // lwip1.4
#endif

#define WIFICLIENT_MAX_PACKET_SIZE TCP_MSS
#define WIFICLIENT_MAX_FLUSH_WAIT_MS 300

#define TCP_DEFAULT_KEEPALIVE_IDLE_SEC          7200 // 2 hours
#define TCP_DEFAULT_KEEPALIVE_INTERVAL_SEC      75   // 75 sec
#define TCP_DEFAULT_KEEPALIVE_COUNT             9    // fault after 9 failures

class ClientContext;
class WiFiServer;

class WiFiClient : public Client, public SList<WiFiClient> {
protected:
  WiFiClient(ClientContext* client);

public:
  WiFiClient();
  virtual ~WiFiClient();
  WiFiClient(const WiFiClient&);
  WiFiClient& operator=(const WiFiClient&);

  // b/c this is both a real class and a virtual parent of the secure client, make sure
  // there's a safe way to copy from the pointer without 'slicing' it; i.e. only the base
  // portion of a derived object will be copied, and the polymorphic behavior will be corrupted. 
  //
  // this class still implements the copy and assignment though, so this is not yet enforced
  // (but, *should* be inside the Core itself, see httpclient & server)
  //
  // ref.
  // - https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-copy-virtual
  // - https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-copy
  virtual std::unique_ptr<WiFiClient> clone() const;

  virtual uint8_t status();
  virtual int connect(IPAddress ip, uint16_t port) override;
  virtual int connect(const char *host, uint16_t port) override;
  virtual int connect(const String& host, uint16_t port);
  virtual size_t write(uint8_t) override;
  virtual size_t write(const uint8_t *buf, size_t size) override;
  virtual size_t write_P(PGM_P buf, size_t size);
  [[ deprecated("use stream.sendHow(client...)") ]]
  size_t write(Stream& stream);

  virtual int available() override;
  virtual int read() override;
  virtual int read(uint8_t* buf, size_t size) override;
  int read(char* buf, size_t size);

  virtual int peek() override;
  virtual size_t peekBytes(uint8_t *buffer, size_t length);
  size_t peekBytes(char *buffer, size_t length) {
    return peekBytes((uint8_t *) buffer, length);
  }
  virtual void flush() override { (void)flush(0); } // wait for all outgoing characters to be sent, output buffer should be empty after this call
  virtual void stop() override { (void)stop(0); }
  bool flush(unsigned int maxWaitMs);
  bool stop(unsigned int maxWaitMs);
  virtual uint8_t connected() override;
  virtual operator bool() override;

  virtual IPAddress remoteIP();
  virtual uint16_t  remotePort();
  virtual IPAddress localIP();
  virtual uint16_t  localPort();

  static void setLocalPortStart(uint16_t port) { _localPort = port; }

  int availableForWrite() override;

  friend class WiFiServer;

  using Print::write;
  
// Api for saving heap when Client class is used by a Server (WiFiServer class): Client = Server.available(). 

// Suppose the local end is the server and the remote end is the client, we will deal with heap memory at the local end.
  
// When the local application (server) decides to close an active connection with a remote end it issues an Client.stop.
// the stop() function calls the close() function of ClientContext class which in turn calls tcp_close.
// The connexion is closed by tcp_close and the protocol control block (pcb) can be put in the following states depending 
// on the requests sent by the remote: CLOSING, FIN_WAIT_1 and FIN_WAIT_2. In theses states pcbs are not freed, then consume 
// some memory heap.  
// If an acknowledgment from the remote end is received, the pcb enter in TIME_WAIT state for some minutes but pcbs in TIME_WAIT
// state are not freed. Then consume some heap memory. 
// TIME_WAIT pcbs are automatically freed after some minutes or can be freed for instance issuing an tcp_kill_timewait() 
// in the local application which will free the oldest pcb in TIME_WAIT state.
    
// If the connection is first closed from the remote end (the client), the local end (server) receive a connection termination request. It then
// acknowledge it and enter in CLOSE_WAIT state waiting for a connection termination request from the local application. 
// It then send a termination request and enter in LAST_ACK state until it receive an acknowledgment from the remote end.
// After receiving the acknowledgment it enter in ClOSED state and the local pcb is freed leaving some room in the heap memory.
  
// To summarize, when a connexion termination request is send by one end (remote or local), the local pcb is not freed immediatly.
// This pcb can be in the following states: FIN_WAIT_1, FIN_WAIT_2, CLOSING, TIME_WAIT, CLOSE_WAIT, LAST_ACK. 
// As a consequence, some old pcbs from old closed connections can still consume heap memory.
  
// The local application can call tcp_kill_timewait hoping it will free some TIME_WAIT state pcbs. But if the server
// receive frequent connections requests and close them after sending whatever it has to send, there may be zero pcbs 
// in TIME_WAIT state among all previously closed pcbs.
  
// In case of insufficient memory to accept a new connection, lwip has developped a strategy: it successively tries 
// to kill the oldest pcb in TIME_WAIT state, or in LAST_ACK state or in CLOSING state or the oldest active connection 
// with lower priority than the new one.

// A a matter of fact this "urgent" strategy is deployed only when very few heap memory remain available (less than some kb).
// In case of success, Client.available returns a valid Client but the local application will crash when sending or receiving
// data from the client (Client.read ou readuntil or available) because this need more heap memory and just some kb were 
// freed in lwip to allocate the new pcb structure ans start the new connection.  

// The propose API is intended to avoid this drawback by calling the abort function of ClientContext which in turn 
// calls tcp_abort which calls tcp_abandon. The connection is aborted and notified to the client with a RESET 
// and the pcb and ressources associated are immediately released increasing the available heap memory.

// This API can be used in two ways:

// WiFiClient Client;
// 1- Replace every Client.stop() with Client.abort()
// or
// 2- In conjonction with Client.stop in the following style:
//    # define MIN_HEAP_FREE 20000 // or whatever min available heap memory convienent for your application 
//    if ( ESP.getFreeHeap() >= MIN_HEAP_FREE ) Client.stop();
//    else Client.abort();
  void abort(); 
      
  static void stopAll();
  static void stopAllExcept(WiFiClient * c);

  void     keepAlive (uint16_t idle_sec = TCP_DEFAULT_KEEPALIVE_IDLE_SEC, uint16_t intv_sec = TCP_DEFAULT_KEEPALIVE_INTERVAL_SEC, uint8_t count = TCP_DEFAULT_KEEPALIVE_COUNT);
  bool     isKeepAliveEnabled () const;
  uint16_t getKeepAliveIdle () const;
  uint16_t getKeepAliveInterval () const;
  uint8_t  getKeepAliveCount () const;
  void     disableKeepAlive () { keepAlive(0, 0, 0); }

  // default NoDelay=False (Nagle=True=!NoDelay)
  // Nagle is for shortly delaying outgoing data, to send less/bigger packets
  // Nagle should be disabled for telnet-like/interactive streams
  // Nagle is meaningless/ignored when Sync=true
  static void setDefaultNoDelay (bool noDelay);
  static bool getDefaultNoDelay ();
  bool getNoDelay() const;
  void setNoDelay(bool nodelay);

  // default Sync=false
  // When sync is true, all writes are automatically flushed.
  // This is slower but also does not allocate
  // temporary memory for sending data
  static void setDefaultSync (bool sync);
  static bool getDefaultSync ();
  bool getSync() const;
  void setSync(bool sync);

  // peek buffer API is present
  virtual bool hasPeekBufferAPI () const override;

  // return number of byte accessible by peekBuffer()
  virtual size_t peekAvailable () override;

  // return a pointer to available data buffer (size = peekAvailable())
  // semantic forbids any kind of read() before calling peekConsume()
  virtual const char* peekBuffer () override;

  // consume bytes after use (see peekBuffer)
  virtual void peekConsume (size_t consume) override;

  virtual bool outputCanTimeout () override { return connected(); }
  virtual bool inputCanTimeout () override { return connected(); }

protected:

  static int8_t _s_connected(void* arg, void* tpcb, int8_t err);
  static void _s_err(void* arg, int8_t err);

  int8_t _connected(void* tpcb, int8_t err);
  void _err(int8_t err);

  ClientContext* _client;
  WiFiClient* _owned;
  static uint16_t _localPort;
};

#endif
