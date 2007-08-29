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

import java.net.*;

/**
 * @class SSLClient
 * @ingroup java_api 
 * @brief The client context.
 *
 * All client connections are started within a client context.
 */
public class SSLClient extends SSLCTX
{
    /**
     * @brief Start a new client context.
     * 
     * @see SSLCTX for details.
     */
    public SSLClient(int options, int num_sessions)
    {
        super(options, num_sessions);
    }

    /**
     * @brief Establish a new SSL connection to an SSL server.
     *
     * It is up to the application to establish the initial socket connection.
     *
     * This is a blocking call - it will finish when the handshake is 
     * complete (or has failed).
     *
     * Call dispose() when the connection is to be removed.
     * @param s [in] A reference to a <A HREF="http://java.sun.com/j2se/1.4.2/docs/api">Socket</A> object.
     * @param session_id [in] A 32 byte session id for session resumption. This 
     * can be null if no session resumption is not required.
     * @return An SSL object reference. Use SSL.handshakeStatus() to check 
     * if a handshake succeeded.
     */
    public SSL connect(Socket s, byte[] session_id)
    {
        int client_fd = axtlsj.getFd(s);
        byte sess_id_size = (byte)(session_id != null ? 
                                session_id.length : 0);
        return new SSL(axtlsj.ssl_client_new(m_ctx, client_fd, session_id,
                        sess_id_size));
    }
}
