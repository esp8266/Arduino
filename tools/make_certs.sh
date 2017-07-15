#!/bin/sh
#
# Copyright (c) 2007-2016, Cameron Rich
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# * Neither the name of the axTLS project nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

AXDIR=`pwd`/`dirname $0`
CWD=`mktemp -d` && cd $dir
cd $CWD 

#
# Generate the certificates and keys for testing.
#

PROJECT_NAME="axTLS Project"

# Generate the openssl configuration files.
cat > ca_cert.conf << EOF  
[ req ]
distinguished_name      = req_distinguished_name
prompt                  = no
req_extensions          = v3_ca

[ req_distinguished_name ]
 O                      = $PROJECT_NAME Dodgy Certificate Authority

[ v3_ca ]
basicConstraints        = critical, CA:true
keyUsage                = critical, cRLSign, keyCertSign, digitalSignature
EOF

cat > certs.conf << EOF  
[ req ]
distinguished_name      = req_distinguished_name
prompt                  = no
req_extensions          = v3_usr_cert

[ req_distinguished_name ]
 O                      = $PROJECT_NAME
 CN                     = localhost

[ v3_usr_cert ]
basicConstraints        = critical, CA:false
keyUsage                = critical, nonRepudiation, digitalSignature, keyEncipherment
subjectAltName          = @alt_names
 
[alt_names]
DNS.1 = www.example.net
DNS.2 = www.example.org
EOF

cat > device_cert.conf << EOF  
[ req ]
distinguished_name     = req_distinguished_name
prompt                 = no

[ req_distinguished_name ]
 O                      = $PROJECT_NAME Device Certificate
EOF

cat > intermediate_ca.conf << EOF  
[ req ]
distinguished_name     = req_distinguished_name
prompt                 = no
req_extensions         = v3_intermediate_ca

[ req_distinguished_name ]
 O                      = $PROJECT_NAME Intermediate CA

[ v3_intermediate_ca ]
basicConstraints        = critical, CA:true, pathlen:0
keyUsage                = cRLSign, keyCertSign, digitalSignature
EOF

cat > intermediate_ca2.conf << EOF  
[ req ]
distinguished_name      = req_distinguished_name
prompt                  = no
req_extensions          = v3_intermediate_ca2

[ req_distinguished_name ]
 O                      = $PROJECT_NAME Intermediate 2 CA

[ v3_intermediate_ca2 ]
basicConstraints        = critical, CA:true, pathlen:10
keyUsage                = encipherOnly, keyCertSign, decipherOnly
EOF

# private key generation
openssl genrsa -out axTLS.ca_key.pem 2048
openssl genrsa -out axTLS.key_1024.pem 1024
openssl genrsa -out axTLS.key_2048.pem 2048
openssl genrsa -out axTLS.key_4096.pem 4096
openssl genrsa -out axTLS.key_device.pem 2048
openssl genrsa -out axTLS.key_intermediate_ca.pem 2048
openssl genrsa -out axTLS.key_intermediate_ca2.pem 2048
openssl genrsa -out axTLS.key_end_chain.pem 2048
openssl genrsa -aes128 -passout pass:abcd -out axTLS.key_aes128.pem 1024
openssl genrsa -aes256 -passout pass:abcd -out axTLS.key_aes256.pem 1024

# convert private keys into DER format
openssl rsa -in axTLS.key_1024.pem -out axTLS.key_1024 -outform DER
openssl rsa -in axTLS.key_2048.pem -out axTLS.key_2048 -outform DER
openssl rsa -in axTLS.key_4096.pem -out axTLS.key_4096 -outform DER

# cert requests
openssl req -out axTLS.ca_x509.csr -key axTLS.ca_key.pem -new \
            -config ./ca_cert.conf 
openssl req -out axTLS.x509_1024.csr -key axTLS.key_1024.pem -new \
            -config ./certs.conf 
openssl req -out axTLS.x509_2048.csr -key axTLS.key_2048.pem -new \
            -config ./certs.conf 
openssl req -out axTLS.x509_4096.csr -key axTLS.key_4096.pem -new \
            -config ./certs.conf 
openssl req -out axTLS.x509_device.csr -key axTLS.key_device.pem -new \
            -config ./device_cert.conf
openssl req -out axTLS.x509_intermediate_ca.csr \
            -key axTLS.key_intermediate_ca.pem -new \
            -config ./intermediate_ca.conf
openssl req -out axTLS.x509_intermediate_ca2.csr \
            -key axTLS.key_intermediate_ca2.pem -new \
            -config ./intermediate_ca2.conf
openssl req -out axTLS.x509_end_chain.csr -key axTLS.key_end_chain.pem -new \
            -config ./certs.conf
openssl req -out axTLS.x509_aes128.csr -key axTLS.key_aes128.pem \
            -new -config ./certs.conf -passin pass:abcd
openssl req -out axTLS.x509_aes256.csr -key axTLS.key_aes256.pem \
            -new -config ./certs.conf -passin pass:abcd

# generate the actual certs.
openssl x509 -req -in axTLS.ca_x509.csr -out axTLS.ca_x509.pem \
            -sha1 -days 5000 -signkey axTLS.ca_key.pem \
            -CAkey axTLS.ca_key.pem -extfile ./ca_cert.conf -extensions v3_ca
openssl x509 -req -in axTLS.ca_x509.csr -out axTLS.ca_x509_sha256.pem \
            -sha256 -days 5000 -signkey axTLS.ca_key.pem \
            -CAkey axTLS.ca_key.pem -extfile ./ca_cert.conf -extensions v3_ca
openssl x509 -req -in axTLS.x509_1024.csr -out axTLS.x509_1024.pem \
            -sha1 -CAcreateserial -days 5000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_1024.csr -out axTLS.x509_1024_sha256.pem \
            -sha256 -CAcreateserial -days 5000 \
            -CA axTLS.ca_x509_sha256.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_1024.csr -out axTLS.x509_1024_sha384.pem \
            -sha384 -CAcreateserial -days 5000 \
            -CA axTLS.ca_x509_sha256.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_1024.csr -out axTLS.x509_1024_sha512.pem \
            -sha512 -CAcreateserial -days 5000 \
            -CA axTLS.ca_x509_sha256.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_2048.csr -out axTLS.x509_2048.pem \
            -sha1 -CAcreateserial -days 5000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_4096.csr -out axTLS.x509_4096.pem \
            -sha1 -CAcreateserial -days 5000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_device.csr -out axTLS.x509_device.pem \
            -sha1 -CAcreateserial -days 5000 \
            -CA axTLS.x509_1024.pem -CAkey axTLS.key_1024.pem
openssl x509 -req -in axTLS.x509_intermediate_ca.csr -out axTLS.x509_intermediate_ca.pem \
            -sha256 -CAcreateserial -days 5000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem \
            -extfile ./intermediate_ca.conf -extensions v3_intermediate_ca
openssl x509 -req -in axTLS.x509_intermediate_ca2.csr -out axTLS.x509_intermediate_ca2.pem \
            -sha256 -CAcreateserial -days 5000 \
            -CA axTLS.x509_intermediate_ca.pem \
            -CAkey axTLS.key_intermediate_ca.pem \
            -extfile ./intermediate_ca2.conf -extensions v3_intermediate_ca2
openssl x509 -req -in axTLS.x509_end_chain.csr -out axTLS.x509_end_chain.pem \
            -sha256 -CAcreateserial -days 5000 \
            -CA axTLS.x509_intermediate_ca.pem \
            -CAkey axTLS.key_intermediate_ca.pem \
            -extfile ./certs.conf -extensions v3_usr_cert 
# basic constraint path len failure
openssl x509 -req -in axTLS.x509_end_chain.csr \
            -out axTLS.x509_end_chain_bad.pem \
            -sha256 -CAcreateserial -days 5000 \
            -CA axTLS.x509_intermediate_ca2.pem \
            -CAkey axTLS.key_intermediate_ca2.pem \
            -extfile ./certs.conf -extensions v3_usr_cert 
cat axTLS.x509_intermediate_ca.pem >> axTLS.x509_intermediate_ca2.pem 
openssl x509 -req -in axTLS.x509_aes128.csr \
            -out axTLS.x509_aes128.pem \
            -sha1 -CAcreateserial -days 5000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_aes256.csr \
            -out axTLS.x509_aes256.pem \
            -sha1 -CAcreateserial -days 5000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem

# note: must be root to do this
DATE_NOW=`date`
if date -s "Jan 1 2025"; then
openssl x509 -req -in axTLS.x509_1024.csr -out axTLS.x509_bad_before.pem \
            -sha1 -CAcreateserial -days 365 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
date -s "$DATE_NOW"
touch axTLS.x509_bad_before.pem
fi
openssl x509 -req -in axTLS.x509_1024.csr -out axTLS.x509_bad_after.pem \
            -sha1 -CAcreateserial -days -365 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem

# some cleanup
rm axTLS*.csr
rm *.srl
rm *.conf

# need this for the client tests
openssl x509 -in axTLS.ca_x509.pem -outform DER -out axTLS.ca_x509.cer 
openssl x509 -in axTLS.x509_1024.pem -outform DER -out axTLS.x509_1024.cer
openssl x509 -in axTLS.x509_2048.pem -outform DER -out axTLS.x509_2048.cer
openssl x509 -in axTLS.x509_4096.pem -outform DER -out axTLS.x509_4096.cer

# generate pkcs8 files (use RC4-128 for encryption)
openssl pkcs8 -in axTLS.key_1024.pem -passout pass:abcd -topk8 -v1 PBE-SHA1-RC4-128 -out axTLS.encrypted_pem.p8
openssl pkcs8 -in axTLS.key_1024.pem -passout pass:abcd -topk8 -outform DER -v1 PBE-SHA1-RC4-128 -out axTLS.encrypted.p8
openssl pkcs8 -in axTLS.key_1024.pem -nocrypt -topk8 -out axTLS.unencrypted_pem.p8
openssl pkcs8 -in axTLS.key_1024.pem -nocrypt -topk8 -outform DER -out axTLS.unencrypted.p8

# generate pkcs12 files (use RC4-128 for encryption)
openssl pkcs12 -export -in axTLS.x509_1024.pem -inkey axTLS.key_1024.pem -certfile axTLS.ca_x509.pem -keypbe PBE-SHA1-RC4-128 -certpbe PBE-SHA1-RC4-128 -name "p12_with_CA" -out axTLS.withCA.p12 -password pass:abcd
openssl pkcs12 -export -in axTLS.x509_1024.pem -inkey axTLS.key_1024.pem -keypbe PBE-SHA1-RC4-128 -certpbe PBE-SHA1-RC4-128 -name "p12_without_CA" -out axTLS.withoutCA.p12 -password pass:abcd
openssl pkcs12 -export -in axTLS.x509_1024.pem -inkey axTLS.key_1024.pem -keypbe PBE-SHA1-RC4-128 -certpbe PBE-SHA1-RC4-128 -out axTLS.noname.p12 -password pass:abcd

# PEM certificate chain
cat axTLS.ca_x509.pem >> axTLS.x509_device.pem

# set default key/cert for use in the server
xxd -i axTLS.x509_1024.cer | sed -e \
        "s/axTLS_x509_1024_cer/default_certificate/" > $AXDIR/../ssl/cert.h
xxd -i axTLS.key_1024 | sed -e \
        "s/axTLS_key_1024/default_private_key/" > $AXDIR/../ssl/private_key.h
