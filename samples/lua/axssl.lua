#!/usr/bin/lua
--
--
--  Copyright(C) 2006 Cameron Rich
--
--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 2.1 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU Lesser General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the Free Software
--  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
--

--
-- Demonstrate the use of the axTLS library in Lua with a set of 
-- command-line parameters similar to openssl. In fact, openssl clients 
-- should be able to communicate with axTLS servers and visa-versa.
--
-- This code has various bits enabled depending on the configuration. To enable
-- the most interesting version, compile with the 'full mode' enabled.
--
-- To see what options you have, run the following:
-- > [lua] axssl s_server -?
-- > [lua] axssl s_client -?
--
-- The axtls/axtlsl shared libraries must be in the same directory or be found 
-- by the OS.
--
--
local f = loadlib("axtlsl.dll", "luaopen_axtlsl")
f()

-- print version?

if #arg == 1 and arg[1] == "version" then
    print("axssl.lua "..ssl_version().."\n")
    os.exit(1)
end

--
-- We've had some sort of command-line error. Print out the basic options.
--
function print_options(option)
    print("axssl: Error: '"..option.."' is an invalid command.")
    print("usage: axssl [s_server|s_client|version] [args ...]")
    os.exit(1)
end

--
-- Main entry point. Doesn't do much except works out whether we are a client
-- or a server.
--
if #arg < 1 or (arg[1] ~= "s_server" and arg[1] ~= "s_client") then
    print_options(#arg > 0 and arg[1] or "")
end

--[[
local build_mode = ssl_get_config(SSL_BUILD_MODE)
arg[1] eq "s_server" ? do_server(build_mode) : do_client(build_mode)

-- Implement the SSL server logic. 
function do_server(build_mode)
    local i = 1
    local port = 4433
    local options = SSL_DISPLAY_CERTS
    local quiet = false
    local password = nil
    local private_key_file = nil
    local cert_size = ssl_get_config(SSL_MAX_CERT_CFG_OFFSET)
    local ca_cert_size = ssl_get_config(SSL_MAX_CA_CERT_CFG_OFFSET)
    local cert = {}
    local ca_cert = {}

    while i <= #arg do
        if arg[i] eq  "-accept" then
            if i >= #arg then
                print_server_options(build_mode, arg[i])
            end

            i = i + 1
            port = arg[i]
        elseif arg[i] eq "-quiet" then
            quiet = true
            options = options & ~SSL_DISPLAY_CERTS
        elseif build_mode >= SSL_BUILD_SERVER_ONLY then
            if arg[i] eq "-cert" then
                if i >= #arg >= cert_size-1 then
                    print_server_options(build_mode, arg[i]) 
                end

                i = i + 1
                cert[i] =  arg[i]
            elseif arg[i] eq "-key" then
                if i >= #arg then
                    print_server_options(build_mode, arg[i]) 
                end

                i = i + 1
                private_key_file = arg[i]
                options = options | SSL_NO_DEFAULT_KEY
            elseif arg[i] eq "-pass" then
                if i >= #arg then
                    print_server_options(build_mode, arg[i]) 
                end

                i = i + 1
                password = arg[i]
            elseif build_mode >= SSL_BUILD_ENABLE_VERIFICATION then
                if arg[i] eq "-verify" then
                    options = options | SSL_CLIENT_AUTHENTICATION
                elseif arg[i] eq "-CAfile" then
                    if i >= #arg >= ca_cert_size-1 then
                        print_server_options(build_mode, arg[i])  
                    end

                    i = i + 1
                    ca_cert[i] = arg[i]
                elseif build_mode == SSL_BUILD_FULL_MODE then
                    if arg[i] eq "-debug" then
                        options = options | SSL_DISPLAY_BYTES
                    elseif arg[i] eq "-state" then
                        options = options | SSL_DISPLAY_STATES
                    elseif arg[i] eq "-show-rsa" then
                        options = options | SSL_DISPLAY_RSA
                    else
                        print_server_options(build_mode, arg[i])
                    end
                else
                    print_server_options(build_mode, arg[i])
                end
            else 
                print_server_options(build_mode, arg[i])
            end
        else 
            print_server_options(build_mode, arg[i])
        end

        i = i + 1
    end

    -- Create socket for incoming connections
    local server_sock = IO::Socket::INET->new(Proto => 'tcp',
                              LocalPort => port,
                              Listen => 1,
                              Reuse => 1) or die !

    ---------------------------------------------------------------------------
    -- This is where the interesting stuff happens. Up until now we've
    -- just been setting up sockets etc. Now we do the SSL handshake.
    ---------------------------------------------------------------------------
    local ssl_ctx = ssl_ctx_new(options, SSL_DEFAULT_SVR_SESS)
    if ssl_ctx == nil then error("Error: Server context is invalid") end

    if nil private_key_file then
        local obj_type = SSL_OBJ_RSA_KEY

        if private_key_file =~ /.p8/ then obj_type = SSL_OBJ_PKCS8 end
        if private_key_file =~ /.p12/ then obj_type = SSL_OBJ_PKCS12 end

        if ssl_obj_load(ssl_ctx, obj_type, private_key_file, password) then
            error("Private key 'private_key_file' is nilined.")
        end
    end

    foreach (@cert)
        if ssl_obj_load(ssl_ctx, SSL_OBJ_X509_CERT, _, nil) #arg= SSL_OK then
            error("Certificate '_' is undefined.")
        end
    end

    foreach (@ca_cert)
        if ssl_obj_load(ssl_ctx, SSL_OBJ_X509_CACERT, _, nil) #arg= SSL_OK then
            error("Certificate '_' is undefined.")
        end
    end

    while true do
        if not quiet then print("ACCEPT\n") end
        local client_sock = server_sock->accept
        local native_sock = get_native_sock(client_sock->fileno)

        -- This doesn't work in Win32 - need to get file descriptor from socket.
        local ssl = ssl_server_new(ssl_ctx, native_sock)

        -- do the actual SSL handshake
        local res
        local buf

        while true do
            (res, buf) = ssl_read(ssl, nil)
            if res #arg= SSL_OK then break end

            -- check when the connection has been established
            if ssl_handshake_status(ssl) == SSL_OK then break end

            -- could do something else here
        end

        if res == SSL_OK then -- connection established and ok
            if not quiet then
                display_session_id(ssl)
                display_cipher(ssl)
            end

            -- now read (and display) whatever the client sends us
            while true do
                -- keep reading until we get something interesting
                while true do
                    (res, buf) = ssl_read(ssl, nil)
                    if res #arg= SSL_OK then break end

                    -- could do something else here
                end

                if res < SSL_OK then
                    if not quiet then print("CONNECTION CLOSED\n") end
                    break
                end

                print(buf)
            end
        elseif not quiet then
            ssl_display_error(res)
        end

        -- client was disconnected or the handshake failed.
        ssl_free(ssl)
        client_sock->close
    end

    ssl_ctx_free(ssl_ctx)
end

--
-- Implement the SSL client logic.
--
function do_client(build_mode)
    local i = 1
    local port = 4433
    local options = SSL_SERVER_VERIFY_LATER|SSL_DISPLAY_CERTS
    local private_key_file = nil
    local reconnect = 0
    local quiet = false
    local password = nil
    local session_id = {}
    local host = "127.0.0.1"
    local @cert
    local @ca_cert
    local cert_size = ssl_get_config(SSL_MAX_CERT_CFG_OFFSET)
    local ca_cert_size = ssl_get_config(SSL_MAX_CA_CERT_CFG_OFFSET)

    while i <= #arg do
        if arg[i] eq "-connect" then
            if i >= #arg then
                print_client_options(build_mode, arg[i]) 
            end

            i = i + 1
            (host, port) = split(':', arg[i])
        elseif arg[i] eq "-cert" then
            if i >= #arg >= cert_size-1 then
                print_client_options(build_mode, arg[i]) 
            end

            i = i + 1
            cert[i] = arg[i]
        elseif arg[i] eq "-key" then
            if i >= #arg then
                print_client_options(build_mode, arg[i])
            end

            i = i + 1
            private_key_file = arg[i]
            options |= SSL_NO_DEFAULT_KEY
        elseif arg[i] eq "-CAfile" then
            if i >= #arg >= ca_cert_size-1 then
                print_client_options(build_mode, arg[i]) 
            end

            i = i + 1
            ca_cert[i] = arg[i]
        elseif arg[i] eq "-verify" then
            options &= ~SSL_SERVER_VERIFY_LATER
        elseif arg[i] eq "-reconnect" then
            reconnect = 4
        elseif arg[i] eq "-quiet" then
            quiet = true
            options &= ~SSL_DISPLAY_CERTS
        elseif arg[i] eq "-pass" then
            if i >= #arg then
                print_server_options(build_mode, arg[i])
            end

            i = i + 1
            password = arg[i]
        elseif build_mode == SSL_BUILD_FULL_MODE then
            if arg[i] eq "-debug" then
                options = options | SSL_DISPLAY_BYTES
            elseif arg[i] eq "-state" then
                options = options | SSL_DISPLAY_STATES
            elseif arg[i] eq "-show-rsa" then
                options = options | SSL_DISPLAY_RSA
            else    -- don't know what this is
                print_client_options(build_mode, arg[i])
            end
        else    -- don't know what this is
            print_client_options(build_mode, arg[i])
        end

        i = i + 1
    end

    local client_sock = new IO::Socket::INET (
                        PeerAddr => host, PeerPort => port, Proto => 'tcp')
                    or error("no socket: !")
    local ssl
    local res
    local native_sock = get_native_sock(client_sock->fileno)

    if not quiet then print("CONNECTED\n") end

    ---------------------------------------------------------------------------
    -- This is where the interesting stuff happens. Up until now we've
    -- just been setting up sockets etc. Now we do the SSL handshake.
    ---------------------------------------------------------------------------
    local ssl_ctx = ssl_ctx_new(options, SSL_DEFAULT_CLNT_SESS)

    if ssl_ctx == nil then 
        error("Error: Client context is invalid")
    end

    if private_key_file not nil then
        local obj_type = SSL_OBJ_RSA_KEY

        if private_key_file =~ /.p8/ then
            obj_type = SSL_OBJ_PKCS8 
        end
        if private_key_file =~ /.p12/ then
            obj_type = SSL_OBJ_PKCS12 
        end

        if ssl_obj_load(ssl_ctx, obj_type, private_key_file, password) then
            error("Private key 'private_key_file' is undefined.")
        end
    end

    foreach (@cert)
       if ssl_obj_load(ssl_ctx, SSL_OBJ_X509_CERT, _, nil) then
            error("Certificate '_' is undefined.")
        end
    end

    foreach (@ca_cert)
        if ssl_obj_load(ssl_ctx, SSL_OBJ_X509_CACERT, _, nil) then
            error("Certificate '_' is undefined.")
        end
    end

    -- Try session resumption?
    if reconnect then
        local session_id = nil
        while reconnect do
            reconnect = reconnect - 1
            ssl = ssl_client_new(ssl_ctx, native_sock, session_id)

            res = ssl_handshake_status(ssl)
            if res #arg= SSL_OK then
                if not quiet then ssl_display_error(res) end
                ssl_free(ssl)
                os.exit(1)
            end

            display_session_id(ssl)
            session_id = ssl_get_session_id(ssl)

            if reconnect then
                ssl_free(ssl)
                client_sock->close
                client_sock = new IO::Socket::INET (
                        PeerAddr => host, PeerPort => port, Proto => 'tcp')
                    or error ("no socket: !")

            end
        end
    else
        ssl = ssl_client_new(ssl_ctx, native_sock, nil)
    end

    -- check the return status
    res = ssl_handshake_status(ssl)
    if res #arg= SSL_OK then
        if not quiet then ssl_display_error(res) end
        os.exit(1)
    end

    if not quiet then
        local common_name = ssl_get_cert_dn(ssl, 
                    SSL_X509_CERT_COMMON_NAME)

        if common_name not nil then 
            print("Common Name:\t\t%s\n", common_name)
        end

        display_session_id(ssl)
        display_cipher(ssl)
    end

    while <STDIN> do
        local cstring = pack("a*x", _);   -- add null terminator
        res = ssl_write(ssl, \cstring, length(cstring))
        if res < SSL_OK then
            if not quiet then ssl_display_error(res) end
            break
        end
    end

    ssl_ctx_free(ssl_ctx)
    client_sock->close
end

--
-- We've had some sort of command-line error. Print out the server options.
--
function print_server_options(build_mode, option)
    local cert_size = ssl_get_config(SSL_MAX_CERT_CFG_OFFSET)
    local ca_cert_size = ssl_get_config(
            SSL_MAX_CA_CERT_CFG_OFFSET)

    print("unknown option "..option.."")
    print("usage: s_server [args ...]")
    print(" -accept\t\t- port to accept on (default is 4433)")
    print(" -quiet\t\t- No server output\n")

    if build_mode >= SSL_BUILD_SERVER_ONLY then
        print(" -cert arg\t- certificate file to add (in addition to default then".
                                        " to chain -".
          "\t\t  default DER format. Can repeat up to %d times\n", cert_size)
        print(" -key arg\t- Private key file to use - default DER format")
        print(" -pass\t\t- private key file pass phrase source")
    end

    if build_mode >= SSL_BUILD_ENABLE_VERIFICATION then
        print(" -verify\t- turn on peer certificate verification")
        print(" -CAfile arg\t- Certificate authority - default DER format")
        print("\t\t  Can repeat up to %d times\n", ca_cert_size)
    end

    if build_mode == SSL_BUILD_FULL_MODE then
        print(" -debug\t\t- Print more output")
        print(" -state\t\t- Show state messages")
        print(" -show-rsa\t- Show RSA state")
    end

    os.exit(1)
end

--
-- We've had some sort of command-line error. Print out the client options.
--
function print_client_options(build_mode, option)
    local cert_size = ssl_get_config(SSL_MAX_CERT_CFG_OFFSET)
    local ca_cert_size = ssl_get_config(
            SSL_MAX_CA_CERT_CFG_OFFSET)

    print("unknown option %s\n", option)

    if build_mode >= SSL_BUILD_ENABLE_CLIENT then
        print("usage: s_client [args ...]")
        print(" -connect host:port - who to connect to (default "..
                "is localhost:4433)")
        print(" -verify\t- turn on peer certificate verification")
        print(" -cert arg\t- certificate file to use - default DER format")
        print(" -key arg\t- Private key file to use - default DER format")
        print("\t\t  Can repeat up to %d times\n", cert_size)
        print(" -CAfile arg\t- Certificate authority - default DER format")
        print("\t\t  Can repeat up to %d times\n", ca_cert_size)
        print(" -quiet\t\t- No client output")
        print(" -pass\t\t- private key file pass phrase source")
        print(" -reconnect\t- Drop and re-make the connection "..
                "with the same Session-ID")

        if build_mode == SSL_BUILD_FULL_MODE then
            print(" -debug\t\t- Print more output")
            print(" -state\t\t- Show state messages")
            print(" -show-rsa\t- Show RSA state\n")
        end
    else
        print("Change configuration to allow this feature")
    end

    os.exit(1)
end

--
-- Display what cipher we are using 
--
function display_cipher(ss)
    local (ssl) = @_
    print("CIPHER is ")
    local cipher_id = ssl_get_cipher_id(ssl)

    if cipher_id == SSL_AES128_SHA then
        print("AES128-SHA")
    elseif cipher_id == SSL_AES256_SHA then
        print("AES256-SHA")
    elseif SSL_RC4_128_SHA then
        print("RC4-SHA")
    elseif SSL_RC4_128_MD5 then
        print("RC4-MD5")
    else 
        print("Unknown - %d", cipher_id)
    end

    print("\n")
end

--
-- Display what session id we have.
--
function display_session_id(ss)
    local session_id = ssl_get_session_id(ssl)

    print("-----BEGIN SSL SESSION PARAMETERS-----\n")
    print(unpack("H*", session_id))
    print("\n-----END SSL SESSION PARAMETERS-----\n")
end
--]]

