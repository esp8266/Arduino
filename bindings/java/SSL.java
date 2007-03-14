/*
 *  Copyright(C) 2006 Cameron Rich
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * A wrapper around the unmanaged interface to give a semi-decent Java API
 */

package axTLSj;

import java.io.*;
import java.util.*;

/**
 * @defgroup java_api Java API.
 *
 * Ensure that the appropriate dispose() methods are called when finished with
 * various objects - otherwise memory leaks will result.
 */

/**
 * @class SSL
 * @ingroup java_api 
 * @brief A representation of an SSL connection.
 *
 */
public class SSL
{
    public int m_ssl;    /**< A pointer to the real SSL type */

    /**
     * @brief Store the reference to an SSL context.
     * @param ip [in] A reference to an SSL object.
     */
    public SSL(int ip)
    {
        m_ssl = ip;
    }

    /**
     * @brief Free any used resources on this connection. 
     * 
     * A "Close Notify" message is sent on this connection (if possible). It 
     * is up to the application to close the socket.
     */
    public void dispose()
    {
        axtlsj.ssl_free(m_ssl);
    }

    /**
     * @brief Return the result of a handshake.
     * @return SSL_OK if the handshake is complete and ok.
     * @see ssl.h for the error code list.
     */
    public int handshakeStatus()
    {
        return axtlsj.ssl_handshake_status(m_ssl);
    }

    /**
     * @brief Return the SSL cipher id.
     * @return The cipher id which is one of:
     * - SSL_AES128_SHA (0x2f)
     * - SSL_AES256_SHA (0x35)
     * - SSL_RC4_128_SHA (0x05)
     * - SSL_RC4_128_MD5 (0x04)
     */
    public byte getCipherId()
    {
        return axtlsj.ssl_get_cipher_id(m_ssl);
    }

    /**
     * @brief Get the session id for a handshake. 
     * 
     * This will be a 32 byte sequence and is available after the first
     * handshaking messages are sent.
     * @return The session id as a 32 byte sequence.
     * @note A SSLv23 handshake may have only 16 valid bytes.
     */
    public byte[] getSessionId()
    {
        return axtlsj.ssl_get_session_id(m_ssl);
    }

    /**
     * @brief Retrieve an X.509 distinguished name component.
     * 
     * When a handshake is complete and a certificate has been exchanged, 
     * then the details of the remote certificate can be retrieved.
     *
     * This will usually be used by a client to check that the server's common 
     * name matches the URL.
     *
     * A full handshake needs to occur for this call to work.
     *
     * @param component [in] one of:
     * - SSL_X509_CERT_COMMON_NAME
     * - SSL_X509_CERT_ORGANIZATION
     * - SSL_X509_CERT_ORGANIZATIONAL_NAME
     * - SSL_X509_CA_CERT_COMMON_NAME
     * - SSL_X509_CA_CERT_ORGANIZATION
     * - SSL_X509_CA_CERT_ORGANIZATIONAL_NAME
     * @return The appropriate string (or null if not defined)
     */
    public String getCertificateDN(int component)
    {
        return axtlsj.ssl_get_cert_dn(m_ssl, component);
    }
}
