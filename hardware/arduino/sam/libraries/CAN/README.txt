Controller Area network (CAN) API for Arduino Due 

This is a beta release of the CAN API for Arduino Due. It contains the necessary classes and functions to configure, enable, disable and use of the CAN peripherals controllers embedded in the SAM3X8E core inside Arduino Due, and the two external SN65HVD234 transceivers.

This CAN API for Arduino Due is released together with a CAN sample1 sketch for the Arduino IDE 1.5.2 and it shows how to configure the CAN controllers and how to manage CAN message transfers. The two CAN controllers (CAN0 and CAN1) and two mailboxes (mailbox 0 and mailbox 1) are used: CAN0 mailbox 0 is configured as transmitter, and CAN1 mailbox 0 is configured as receiver. The communication baudrate is 1Mbit/s. The CAN0 controller tries to send on the bus messages through the mailbox 0 and waits for messages from mailbox 1 on CAN1 controller. 

It is required to use the two CAN pins in Arduino Due, connected to the two external SN65HVD234transceivers in loop mode via a pair cable. The CAN message transaction can be monitored by a serial UART connection (115.2 Kbps, 8 data bits, no parity, 1 stop bit, no flux control) or serial monitor of the Arduino IDE 1.5.2 with autoscroll and newline modes activated.

Source files:
- CAN.cpp
- CAN.h
- sn65hvd234.c
- sn65hvd234.h
- variant.cpp
- variant.h

CAN files (.cpp .h) contain the CANRaw class with 38 functions.

sn65hvd234 files (.c .h) contain 7 driver functions.

The variant files (.cpp .h) are updates to the IDE 1.5.2 ones. Added initialization of the CAN pins in variant.cpp 
and CAN pins definition in variant.h.

Hardware requirements:
- Arduino Due board
- Dual CAN transceiver shield
- Twisted shielded pair cable

Software requirements:
- Arduino IDE 1.5.2
- CAN API library

To perform the CAN sample 1 test, the pair cable needs to be connected between the two CAN ports as follows:
CANRX0 <-> CANRX0
CANRX1 <-> CANRX1

Once the CAN sample 1 and the CAN library are loaded in Arduino IDE 1.5.2, after serial monitor open or after a reset of the Arduino board, the following message should be displayed in the monitor terminal: Type CAN message to send. Then, an 8 digit message can be typed and after a return stroke of the keyboard, the following message should be displayed: Sent value=XXXXXXXX, where XXXXXXXX is the typed message. If the message was sent/received successfully, the following message should be displayed: CAN message received=XXXXXXX and End of test. Otherwise, there is a CAN communication error.
