##
## $Id: README.gdb,v 1.5 2003/09/19 19:59:20 troth Exp $
##

This README file discusses the issues involved with using the simulator
remotely from gdb.

First off, you will need to compile gdb >= 5.2.1 with the avr target support.

Once you've built and installed gdb for avr, you need to know how to use it to
debug a remote program. It's an easy, two step process.  First, you need to
start up simulavr in gdbserver mode as such:

    $ simulavr --gdbserver [other options]

Second, just start up avr-gdb with an avr elf program, connect to the remote
target, and load the program into the sim with these commands:

    $ avr-gdb myprog.elf
    (gdb) target remote localhost:<port>
    (gdb) load

where <port> is the port number that simulavr is listening on. You need to
have the simulavr running before giving gdb the target command. In most cases,
you'll be running simulavr on the same host as gdb, thus the "localhost"
part. You must use the same port number for simulavr and in the gdb target
command. If you feel the need to run simulavr on another host, just change
localhost to the name of the host that simulavr will be running on.

After gdb has connected to the remote target, you should be able to use gdb
just as if you were debugging a program natively.

For more detailed information, see the documentation in the doc subdirectory.

