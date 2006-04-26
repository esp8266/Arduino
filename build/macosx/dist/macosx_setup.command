#!/bin/sh

# modified to change from /var/spool/uucp to /var/lock for rxtx 2.1_7r2
# for processing release 0112, 13 april 2006 [fry]

# originally fixperm.sh from rxtx-2.1_6/contrib
# with a couple additions from pathsetup.command from fink [fry]

# A script to fix permissions for lock files on Mac OS X
# Contributed by Dmitry Markman <dimitry.markman@verizon.net>
# Fri Aug 23 15:46:46 MDT 2002

echo ""
echo ""
echo "This command will take care of a couple system things"
echo "so that you can properly use the serial port to communicate"
echo "between hardware devices and processing."
echo ""
echo "If there are actually changes that need to be made, then"
echo "enter your password when prompted. The command uses sudo,"
echo "and you'll need to have administrator access."
echo ""

echo -n "Do you want to continue? [Y/n] "
read answer
answer=`echo $answer | sed 's/^[yY].*$/y/'`
if [ -z "$answer" -o "x$answer" = "xy" ]; then
    curruser=`sudo id -p | grep 'login' | sed 's/login.//'`

    if [ ! -d /var/lock ]
    then
        sudo mkdir /var/lock
    fi

    sudo chgrp uucp /var/lock
    sudo chmod 775 /var/lock

    if [ ! `sudo niutil -readprop / /groups/uucp users | grep $curruser > /dev/null` ]
    then
        sudo niutil -mergeprop / /groups/uucp users $curruser
    fi

    echo "Finished making changes, you should be all set"
else
    echo "Ok, nevermind then."
fi

echo ""
echo ""
echo "     (All done... you can close this window now)"
echo ""
echo ""
echo ""

