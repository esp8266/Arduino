#include "w5100.h"
#include "socket.h"

static uint16_t local_port;

/**
 * @brief	This Socket function initialize the channel in perticular mode, and set the port and wait for W5100 done it.
 * @return 	1 for success else 0.
 */
uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag)
{
  uint8_t ret;
  if ((protocol == SnMR::TCP) || (protocol == SnMR::UDP) || (protocol == SnMR::IPRAW) || (protocol == SnMR::MACRAW) || (protocol == SnMR::PPPOE))
  {
    close(s);
    W5100.writeSnMR(s, protocol | flag);
    if (port != 0) {
      W5100.writeSnPORT(s, port);
    } 
    else {
      local_port++; // if don't set the source port, set local_port number.
      W5100.writeSnPORT(s, local_port);
    }

    W5100.execCmdSn(s, Sock_OPEN);
    
    return 1;
  }

  return 0;
}


/**
 * @brief	This function close the socket and parameter is "s" which represent the socket number
 */
void close(SOCKET s)
{
  W5100.execCmdSn(s, Sock_CLOSE);
  W5100.writeSnIR(s, 0xFF);
}


/**
 * @brief	This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
 * @return	1 for success else 0.
 */
uint8_t listen(SOCKET s)
{
  if (W5100.readSnSR(s) != SnSR::INIT)
    return 0;
  W5100.execCmdSn(s, Sock_LISTEN);
  return 1;
}


/**
 * @brief	This function established  the connection for the channel in Active (client) mode. 
 * 		This function waits for the untill the connection is established.
 * 		
 * @return	1 for success else 0.
 */
uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port)
{
  if 
    (
  ((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
    ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
    (port == 0x00) 
    ) 
    return 0;

  // set destination IP
  W5100.writeSnDIPR(s, addr);
  W5100.writeSnDPORT(s, port);
  W5100.execCmdSn(s, Sock_CONNECT);

  return 1;
}



/**
 * @brief	This function used for disconnect the socket and parameter is "s" which represent the socket number
 * @return	1 for success else 0.
 */
void disconnect(SOCKET s)
{
  W5100.execCmdSn(s, Sock_DISCON);
}


/**
 * @brief	This function used to send the data in TCP mode
 * @return	1 for success else 0.
 */
uint16_t send(SOCKET s, const uint8_t * buf, uint16_t len)
{
  uint8_t status=0;
  uint16_t ret=0;
  uint16_t freesize=0;

  if (len > W5100.SSIZE) 
    ret = W5100.SSIZE; // check size not to exceed MAX size.
  else 
    ret = len;

  // if freebuf is available, start.
  do 
  {
    freesize = W5100.getTXFreeSize(s);
    status = W5100.readSnSR(s);
    if ((status != SnSR::ESTABLISHED) && (status != SnSR::CLOSE_WAIT))
    {
      ret = 0; 
      break;
    }
  } 
  while (freesize < ret);

  // copy data
  W5100.send_data_processing(s, (uint8_t *)buf, ret);
  W5100.execCmdSn(s, Sock_SEND);

  /* +2008.01 bj */
  while ( (W5100.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
  {
    /* m2008.01 [bj] : reduce code */
    if ( W5100.readSnSR(s) == SnSR::CLOSED )
    {
      close(s);
      return 0;
    }
  }
  /* +2008.01 bj */
  W5100.writeSnIR(s, SnIR::SEND_OK);
  return ret;
}


/**
 * @brief	This function is an application I/F function which is used to receive the data in TCP mode.
 * 		It continues to wait for data as much as the application wants to receive.
 * 		
 * @return	received data size for success else -1.
 */
uint16_t recv(SOCKET s, uint8_t *buf, uint16_t len)
{
  uint16_t ret=0;

  if ( len > 0 )
  {
    W5100.recv_data_processing(s, buf, len);
    W5100.execCmdSn(s, Sock_RECV);
    ret = len;
  }
  return ret;
}


/**
 * @brief	Returns the first byte in the receive queue (no checking)
 * 		
 * @return
 */
uint16_t peek(SOCKET s, uint8_t *buf)
{
  W5100.recv_data_processing(s, buf, 1, 1);

  return 1;
}


/**
 * @brief	This function is an application I/F function which is used to send the data for other then TCP mode. 
 * 		Unlike TCP transmission, The peer's destination address and the port is needed.
 * 		
 * @return	This function return send data size for success else -1.
 */
uint16_t sendto(SOCKET s, const uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t port)
{
  uint16_t ret=0;

  if (len > W5100.SSIZE) ret = W5100.SSIZE; // check size not to exceed MAX size.
  else ret = len;

  if
    (
  ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
    ((port == 0x00)) ||(ret == 0)
    ) 
  {
    /* +2008.01 [bj] : added return value */
    ret = 0;
  }
  else
  {
    W5100.writeSnDIPR(s, addr);
    W5100.writeSnDPORT(s, port);

    // copy data
    W5100.send_data_processing(s, (uint8_t *)buf, ret);
    W5100.execCmdSn(s, Sock_SEND);

    /* +2008.01 bj */
    while ( (W5100.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
    {
      if (W5100.readSnIR(s) & SnIR::TIMEOUT)
      {
        /* +2008.01 [bj]: clear interrupt */
        W5100.writeSnIR(s, (SnIR::SEND_OK | SnIR::TIMEOUT)); /* clear SEND_OK & TIMEOUT */
        return 0;
      }
    }

    /* +2008.01 bj */
    W5100.writeSnIR(s, SnIR::SEND_OK);
  }
  return ret;
}


/**
 * @brief	This function is an application I/F function which is used to receive the data in other then
 * 	TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well. 
 * 	
 * @return	This function return received data size for success else -1.
 */
uint16_t recvfrom(SOCKET s, uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t *port)
{
  uint8_t head[8];
  uint16_t data_len=0;
  uint16_t ptr=0;

  if ( len > 0 )
  {
    ptr = W5100.readSnRX_RD(s);
    switch (W5100.readSnMR(s) & 0x07)
    {
    case SnMR::UDP :
      W5100.read_data(s, (uint8_t *)ptr, head, 0x08);
      ptr += 8;
      // read peer's IP address, port number.
      addr[0] = head[0];
      addr[1] = head[1];
      addr[2] = head[2];
      addr[3] = head[3];
      *port = head[4];
      *port = (*port << 8) + head[5];
      data_len = head[6];
      data_len = (data_len << 8) + head[7];

      W5100.read_data(s, (uint8_t *)ptr, buf, data_len); // data copy.
      ptr += data_len;

      W5100.writeSnRX_RD(s, ptr);
      break;

    case SnMR::IPRAW :
      W5100.read_data(s, (uint8_t *)ptr, head, 0x06);
      ptr += 6;

      addr[0] = head[0];
      addr[1] = head[1];
      addr[2] = head[2];
      addr[3] = head[3];
      data_len = head[4];
      data_len = (data_len << 8) + head[5];

      W5100.read_data(s, (uint8_t *)ptr, buf, data_len); // data copy.
      ptr += data_len;

      W5100.writeSnRX_RD(s, ptr);
      break;

    case SnMR::MACRAW:
      W5100.read_data(s,(uint8_t*)ptr,head,2);
      ptr+=2;
      data_len = head[0];
      data_len = (data_len<<8) + head[1] - 2;

      W5100.read_data(s,(uint8_t*) ptr,buf,data_len);
      ptr += data_len;
      W5100.writeSnRX_RD(s, ptr);
      break;

    default :
      break;
    }
    W5100.execCmdSn(s, Sock_RECV);
  }
  return data_len;
}


uint16_t igmpsend(SOCKET s, const uint8_t * buf, uint16_t len)
{
  uint8_t status=0;
  uint16_t ret=0;

  if (len > W5100.SSIZE) 
    ret = W5100.SSIZE; // check size not to exceed MAX size.
  else 
    ret = len;

  if (ret == 0)
    return 0;

  W5100.send_data_processing(s, (uint8_t *)buf, ret);
  W5100.execCmdSn(s, Sock_SEND);

  while ( (W5100.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
  {
    status = W5100.readSnSR(s);
    if (W5100.readSnIR(s) & SnIR::TIMEOUT)
    {
      /* in case of igmp, if send fails, then socket closed */
      /* if you want change, remove this code. */
      close(s);
      return 0;
    }
  }

  W5100.writeSnIR(s, SnIR::SEND_OK);
  return ret;
}

