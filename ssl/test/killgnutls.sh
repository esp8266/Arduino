#!/bin/sh
ps -ef|grep gnutls-serv | /usr/bin/awk '{print $2}' |xargs kill -9
