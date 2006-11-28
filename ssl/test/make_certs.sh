#!/bin/sh

#
#  Copyright(C) 2006 Cameron Rich
#
#  This license is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This license is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this license; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

#
# Generate the certificates and keys for testing.
#

PROJECT_NAME="axTLS Project"

# Generate the openssl configuration files.
cat > ca_cert.conf << EOF  
[ req ]
distinguished_name     = req_distinguished_name
prompt                 = no

[ req_distinguished_name ]
 O                      = $PROJECT_NAME Dodgy Certificate Authority
EOF

cat > certs.conf << EOF  
[ req ]
distinguished_name     = req_distinguished_name
prompt                 = no

[ req_distinguished_name ]
 O                      = $PROJECT_NAME
 CN                     = 127.0.0.1
EOF

cat > device_cert.conf << EOF  
[ req ]
distinguished_name     = req_distinguished_name
prompt                 = no

[ req_distinguished_name ]
 O                      = $PROJECT_NAME Device Certificate
EOF

# private key generation
openssl genrsa -out axTLS.ca_key.pem 1024
openssl genrsa -out axTLS.key_512.pem 512
openssl genrsa -out axTLS.key_1024.pem 1024
openssl genrsa -out axTLS.key_2048.pem 2048
openssl genrsa -out axTLS.key_4096.pem 4096
openssl genrsa -out axTLS.device_key.pem 1024
openssl genrsa -aes128 -passout pass:abcd -out axTLS.key_aes128.pem 512
openssl genrsa -aes256 -passout pass:abcd -out axTLS.key_aes256.pem 512

# convert private keys into DER format
openssl rsa -in axTLS.key_512.pem -out axTLS.key_512 -outform DER
openssl rsa -in axTLS.key_1024.pem -out axTLS.key_1024 -outform DER
openssl rsa -in axTLS.key_2048.pem -out axTLS.key_2048 -outform DER
openssl rsa -in axTLS.key_4096.pem -out axTLS.key_4096 -outform DER
openssl rsa -in axTLS.device_key.pem -out axTLS.device_key -outform DER

# cert requests
openssl req -out axTLS.ca_x509.req -key axTLS.ca_key.pem -new \
            -config ./ca_cert.conf 
openssl req -out axTLS.x509_512.req -key axTLS.key_512.pem -new \
            -config ./certs.conf 
openssl req -out axTLS.x509_1024.req -key axTLS.key_1024.pem -new \
            -config ./certs.conf 
openssl req -out axTLS.x509_2048.req -key axTLS.key_2048.pem -new \
            -config ./certs.conf 
openssl req -out axTLS.x509_4096.req -key axTLS.key_4096.pem -new \
            -config ./certs.conf 
openssl req -out axTLS.x509_device.req -key axTLS.device_key.pem -new \
            -config ./device_cert.conf
openssl req -out axTLS.x509_aes128.req -key axTLS.key_aes128.pem \
            -new -config ./certs.conf -passin pass:abcd
openssl req -out axTLS.x509_aes256.req -key axTLS.key_aes256.pem \
            -new -config ./certs.conf -passin pass:abcd

# generate the actual certs.
openssl x509 -req -in axTLS.ca_x509.req -out axTLS.ca_x509.pem \
            -sha1 -days 10000 -signkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_512.req -out axTLS.x509_512.pem \
            -sha1 -CAcreateserial -days 10000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_1024.req -out axTLS.x509_1024.pem \
            -sha1 -CAcreateserial -days 10000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_2048.req -out axTLS.x509_2048.pem \
            -md5 -CAcreateserial -days 10000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_4096.req -out axTLS.x509_4096.pem \
            -md5 -CAcreateserial -days 10000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_device.req -out axTLS.x509_device.pem \
            -sha1 -CAcreateserial -days 10000 \
            -CA axTLS.x509_512.pem -CAkey axTLS.key_512.pem
openssl x509 -req -in axTLS.x509_aes128.req \
            -out axTLS.x509_aes128.pem \
            -sha1 -CAcreateserial -days 10000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
openssl x509 -req -in axTLS.x509_aes256.req \
            -out axTLS.x509_aes256.pem \
            -sha1 -CAcreateserial -days 10000 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem

# note: must be root to do this
DATE_NOW=`date`
if date -s "Jan 1 2025"; then
openssl x509 -req -in axTLS.x509_512.req -out axTLS.x509_bad_before.pem \
            -sha1 -CAcreateserial -days 365 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem
date -s "$DATE_NOW"
touch axTLS.x509_bad_before.pem
fi
openssl x509 -req -in axTLS.x509_512.req -out axTLS.x509_bad_after.pem \
            -sha1 -CAcreateserial -days -365 \
            -CA axTLS.ca_x509.pem -CAkey axTLS.ca_key.pem

# some cleanup
rm axTLS*.req
rm axTLS.srl
rm *.conf

# need this for the client tests
openssl x509 -in axTLS.ca_x509.pem -outform DER -out axTLS.ca_x509.cer 
openssl x509 -in axTLS.x509_512.pem -outform DER -out axTLS.x509_512.cer
openssl x509 -in axTLS.x509_1024.pem -outform DER -out axTLS.x509_1024.cer
openssl x509 -in axTLS.x509_2048.pem -outform DER -out axTLS.x509_2048.cer
openssl x509 -in axTLS.x509_4096.pem -outform DER -out axTLS.x509_4096.cer
openssl x509 -in axTLS.x509_device.pem -outform DER -out axTLS.x509_device.cer

# generate pkcs8 files (use RC4-128 for encryption)
openssl pkcs8 -in axTLS.key_512.pem -passout pass:abcd -topk8 -v1 PBE-SHA1-RC4-128 -out axTLS.encrypted_pem.p8
openssl pkcs8 -in axTLS.key_512.pem -passout pass:abcd -topk8 -outform DER -v1 PBE-SHA1-RC4-128 -out axTLS.encrypted.p8
openssl pkcs8 -in axTLS.key_512.pem -nocrypt -topk8 -out axTLS.unencrypted_pem.p8
openssl pkcs8 -in axTLS.key_512.pem -nocrypt -topk8 -outform DER -out axTLS.unencrypted.p8

# generate pkcs12 files (use RC4-128 for encryption)
openssl pkcs12 -export -in axTLS.x509_1024.pem -inkey axTLS.key_1024.pem -certfile axTLS.ca_x509.pem -keypbe PBE-SHA1-RC4-128 -certpbe PBE-SHA1-RC4-128 -name "p12_with_CA" -out axTLS.withCA.p12 -password pass:abcd
openssl pkcs12 -export -in axTLS.x509_1024.pem -inkey axTLS.key_1024.pem -keypbe PBE-SHA1-RC4-128 -certpbe PBE-SHA1-RC4-128 -name "p12_without_CA" -out axTLS.withoutCA.p12 -password pass:abcd
openssl pkcs12 -export -in axTLS.x509_1024.pem -inkey axTLS.key_1024.pem -keypbe PBE-SHA1-RC4-128 -certpbe PBE-SHA1-RC4-128 -out axTLS.noname.p12 -password pass:abcd

# PEM certificate chain
cat axTLS.ca_x509.pem >> axTLS.x509_device.pem

# set default key/cert for use in the server
xxd -i axTLS.x509_1024.cer | sed -e \
        "s/axTLS_x509_1024_cer/default_certificate/" > ../../ssl/cert.h
xxd -i axTLS.key_1024 | sed -e \
        "s/axTLS_key_1024/default_private_key/" > ../../ssl/private_key.h
