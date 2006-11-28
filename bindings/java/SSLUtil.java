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
 * @class SSLUtil
 * @ingroup java_api 
 * @brief Some global helper functions.
 *
 */
public class SSLUtil
{
    /**
     * @brief Load up the ddl/shared library 
     */
    static
    {
        System.loadLibrary("axtlsj");
    }

    /**
     * @brief Return the build mode of the axTLS project.
     * @return The build mode is one of:
     * - SSL_BUILD_SERVER_ONLY
     * - SSL_BUILD_ENABLE_VERIFICATION
     * - SSL_BUILD_ENABLE_CLIENT
     * - SSL_BUILD_FULL_MODE
     */
    public static int buildMode()
    {
        return axtlsj.ssl_get_config(axtlsj.SSL_BUILD_MODE);
    }

    /**
     * @brief Return the number of chained certificates that the client/server 
     * supports.
     * @return The number of supported client/server certificates.
     */
    public static int maxCerts()
    {
        return axtlsj.ssl_get_config(axtlsj.SSL_MAX_CERT_CFG_OFFSET);
    }

    /**
     * @brief Return the number of CA certificates that the client/server
     * supports.
     * @return The number of supported CA certificates.
     */
    public static int maxCACerts()
    {
        return axtlsj.ssl_get_config(axtlsj.SSL_MAX_CA_CERT_CFG_OFFSET);
    }

    /**
     * @brief Indicate if PEM is supported.
     * @return true if PEM supported.
     */
    public static boolean hasPEM()
    {
        return axtlsj.ssl_get_config(axtlsj.SSL_HAS_PEM) > 0 ? true : false;
    }

    /**
     * @brief Display the text string of the error.
     * @param error_code [in] The integer error code.
     * @see ssl.h for the error code list.
     */
    public static void displayError(int error_code)
    {
        axtlsj.ssl_display_error(error_code);
    }
}

