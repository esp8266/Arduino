#!/bin/bash

# This script generates a self-signed certificate for use by the ESP8266
# Replace your-name-here with somethine appropriate before running and use
# the generated .H files in your code as follows:
#
#      static const uint8_t rsakey[]  ICACHE_RODATA_ATTR = {
#        #include "key.h"
#      };
#
#      static const uint8_t x509[] ICACHE_RODATA_ATTR = {
#        #include "x509.h"
#      };
#
#      ....
#      WiFiServerSecure server(443);
#      server.setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));
#      ....

# 1024 or 512.  512 saves memory...
BITS=512
C=$PWD
pushd /tmp

openssl genrsa -out tls.ca_key.pem $BITS
openssl genrsa -out tls.key_$BITS.pem $BITS
openssl rsa -in tls.key_$BITS.pem -out tls.key_$BITS -outform DER
cat > certs.conf <<EOF
[ req ]
distinguished_name = req_distinguished_name
prompt = no

[ req_distinguished_name ]
O = your-name-here
CN = 127.0.0.1
EOF
openssl req -out tls.ca_x509.req -key tls.ca_key.pem -new -config certs.conf 
openssl req -out tls.x509_$BITS.req -key tls.key_$BITS.pem -new -config certs.conf 
openssl x509 -req -in tls.ca_x509.req  -out tls.ca_x509.pem -sha256 -days 5000 -signkey tls.ca_key.pem 
openssl x509 -req -in tls.x509_$BITS.req  -out tls.x509_$BITS.pem -sha256 -CAcreateserial -days 5000 -CA tls.ca_x509.pem -CAkey tls.ca_key.pem 
openssl x509 -in tls.ca_x509.pem -outform DER -out tls.ca_x509.cer
openssl x509 -in tls.x509_$BITS.pem -outform DER -out tls.x509_$BITS.cer

xxd -i tls.key_$BITS       | sed 's/.*{//' | sed 's/\};//' | sed 's/unsigned.*//' > "$C/key.h"
xxd -i tls.x509_$BITS.cer  | sed 's/.*{//' | sed 's/\};//' | sed 's/unsigned.*//' > "$C/x509.h"

rm -f tls.ca_key.pem tls.key_$BITS.pem tls.key_$BITS certs.conf tls.ca_x509.req tls.x509_$BITS.req tls.ca_x509.pem tls.x509_$BITS.pem tls.srl tls.x509_$BITS.cer tls.ca_x509.cer

popd
