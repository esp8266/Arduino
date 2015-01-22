/*
 * $HeadURL$
 * $Revision$
 * $Date$
 * 
 * ====================================================================
 *
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 */

package processing.app.debug;

import org.apache.commons.httpclient.ConnectTimeoutException;
import org.apache.commons.httpclient.HttpClientError;
import org.apache.commons.httpclient.params.HttpConnectionParams;
import org.apache.commons.httpclient.protocol.SecureProtocolSocketFactory;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import javax.net.SocketFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.TrustManager;
import java.io.IOException;
import java.net.*;

/**
 * <p>
 * EasySSLProtocolSocketFactory can be used to creats SSL {@link Socket}s
 * that accept self-signed certificates.
 * </p>
 * <p>
 * This socket factory SHOULD NOT be used for productive systems
 * due to security reasons, unless it is a concious decision and
 * you are perfectly aware of security implications of accepting
 * self-signed certificates
 * </p>
 * <p/>
 * <p>
 * Example of using custom protocol socket factory for a specific host:
 * <pre>
 *     Protocol easyhttps = new Protocol("https", new EasySSLProtocolSocketFactory(), 443);
 *
 *     URI uri = new URI("https://localhost/", true);
 *     // use relative url only
 *     GetMethod httpget = new GetMethod(uri.getPathQuery());
 *     HostConfiguration hc = new HostConfiguration();
 *     hc.setHost(uri.getHost(), uri.getPort(), easyhttps);
 *     HttpClient client = new HttpClient();
 *     client.executeMethod(hc, httpget);
 *     </pre>
 * </p>
 * <p>
 * Example of using custom protocol socket factory per default instead of the standard one:
 * <pre>
 *     Protocol easyhttps = new Protocol("https", new EasySSLProtocolSocketFactory(), 443);
 *     Protocol.registerProtocol("https", easyhttps);
 *
 *     HttpClient client = new HttpClient();
 *     GetMethod httpget = new GetMethod("https://localhost/");
 *     client.executeMethod(httpget);
 *     </pre>
 * </p>
 *
 * @author <a href="mailto:oleg -at- ural.ru">Oleg Kalnichevski</a>
 *         <p/>
 *         <p>
 *         DISCLAIMER: HttpClient developers DO NOT actively support this component.
 *         The component is provided as a reference material, which may be inappropriate
 *         for use without additional customization.
 *         </p>
 */

public class EasySSLProtocolSocketFactory implements SecureProtocolSocketFactory {

  /**
   * Log object for this class.
   */
  private static final Log LOG = LogFactory.getLog(EasySSLProtocolSocketFactory.class);
  public static final String[] SSL_PROTOCOLS = {"SSLv3", "TLSv1"};
  public static final String[] SSL_CYPHER_SUITES = {"TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA", "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA", "TLS_RSA_WITH_AES_128_CBC_SHA", "TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA", "TLS_ECDH_RSA_WITH_AES_128_CBC_SHA", "TLS_DHE_RSA_WITH_AES_128_CBC_SHA", "TLS_DHE_DSS_WITH_AES_128_CBC_SHA", "TLS_ECDHE_ECDSA_WITH_RC4_128_SHA", "TLS_ECDHE_RSA_WITH_RC4_128_SHA", "SSL_RSA_WITH_RC4_128_SHA", "TLS_ECDH_ECDSA_WITH_RC4_128_SHA", "TLS_ECDH_RSA_WITH_RC4_128_SHA", "TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA", "TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA", "SSL_RSA_WITH_3DES_EDE_CBC_SHA", "TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA", "TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA", "SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA", "SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA", "SSL_RSA_WITH_RC4_128_MD5", "TLS_EMPTY_RENEGOTIATION_INFO_SCSV"};

  private SSLContext sslcontext = null;

  /**
   * Constructor for EasySSLProtocolSocketFactory.
   */
  public EasySSLProtocolSocketFactory() {
    super();
  }

  private static SSLContext createEasySSLContext() {
    try {
      SSLContext context = SSLContext.getInstance("SSL");
      context.init(
              null,
              new TrustManager[]{new EasyX509TrustManager(null)},
              null);
      return context;
    } catch (Exception e) {
      LOG.error(e.getMessage(), e);
      throw new HttpClientError(e.toString());
    }
  }

  private SSLContext getSSLContext() {
    if (this.sslcontext == null) {
      this.sslcontext = createEasySSLContext();
    }
    return this.sslcontext;
  }

  /**
   * @see SecureProtocolSocketFactory#createSocket(java.lang.String, int, java.net.InetAddress, int)
   */
  public Socket createSocket(
          String host,
          int port,
          InetAddress clientHost,
          int clientPort)
          throws IOException, UnknownHostException {

    Socket socket = getSSLContext().getSocketFactory().createSocket(
            host,
            port,
            clientHost,
            clientPort
    );
    return socket;
  }

  /**
   * Attempts to get a new socket connection to the given host within the given time limit.
   * <p>
   * To circumvent the limitations of older JREs that do not support connect timeout a
   * controller thread is executed. The controller thread attempts to create a new socket
   * within the given limit of time. If socket constructor does not return until the
   * timeout expires, the controller terminates and throws an {@link ConnectTimeoutException}
   * </p>
   *
   * @param host       the host name/IP
   * @param port       the port on the host
   * @param clientHost the local host name/IP to bind the socket to
   * @param clientPort the port on the local machine
   * @param params     {@link HttpConnectionParams Http connection parameters}
   * @return Socket a new socket
   * @throws IOException          if an I/O error occurs while creating the socket
   * @throws UnknownHostException if the IP address of the host cannot be
   *                              determined
   */
  public Socket createSocket(
          final String host,
          final int port,
          final InetAddress localAddress,
          final int localPort,
          final HttpConnectionParams params
  ) throws IOException, UnknownHostException, ConnectTimeoutException {
    if (params == null) {
      throw new IllegalArgumentException("Parameters may not be null");
    }
    int timeout = params.getConnectionTimeout();
    SocketFactory socketfactory = getSSLContext().getSocketFactory();
    Socket socket;
    if (timeout == 0) {
      socket = socketfactory.createSocket(host, port, localAddress, localPort);
    } else {
      socket = socketfactory.createSocket();
      SocketAddress localaddr = new InetSocketAddress(localAddress, localPort);
      SocketAddress remoteaddr = new InetSocketAddress(host, port);
      socket.bind(localaddr);
      socket.connect(remoteaddr, timeout);
    }

    SSLSocket sslSocket = (SSLSocket) socket;
    sslSocket.setEnabledProtocols(SSL_PROTOCOLS);
    sslSocket.setEnabledCipherSuites(SSL_CYPHER_SUITES);

    return socket;
  }

  /**
   * @see SecureProtocolSocketFactory#createSocket(java.lang.String, int)
   */
  public Socket createSocket(String host, int port)
          throws IOException, UnknownHostException {
    return getSSLContext().getSocketFactory().createSocket(
            host,
            port
    );
  }

  /**
   * @see SecureProtocolSocketFactory#createSocket(java.net.Socket, java.lang.String, int, boolean)
   */
  public Socket createSocket(
          Socket socket,
          String host,
          int port,
          boolean autoClose)
          throws IOException, UnknownHostException {
    return getSSLContext().getSocketFactory().createSocket(
            socket,
            host,
            port,
            autoClose
    );
  }

  public boolean equals(Object obj) {
    return ((obj != null) && obj.getClass().equals(EasySSLProtocolSocketFactory.class));
  }

  public int hashCode() {
    return EasySSLProtocolSocketFactory.class.hashCode();
  }

}
