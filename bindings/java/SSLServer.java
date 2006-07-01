/*
 *  Copyright(C) 2006
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
 * @class SSLServer
 * @ingroup java_api 
 * @brief The server context.
 *
 * All server connections are started within a server context.
 */
public class SSLServer extends SSLCTX
{
    /**
     * @brief Start a new server context.
     * 
     * @see SSLCTX for details.
     */
    public SSLServer(int options, int num_sessions)
    {
        super(options, num_sessions);
    }

    /**
     * @brief Establish a new SSL connection to an SSL client.
     *
     * It is up to the application to establish the initial socket connection.
     *
     * Call dispose() when the connection is to be removed.
     * @param s [in] A reference to a <A HREF="http://java.sun.com/j2se/1.4.2/docs/api">Socket</A> object.
     * @return An SSL object reference.
     */
    public SSL connect(Socket s)
    {
        int client_fd = axtlsj.getFd(s);
        return new SSL(axtlsj.ssl_server_new(m_ctx, client_fd));
    }
}
