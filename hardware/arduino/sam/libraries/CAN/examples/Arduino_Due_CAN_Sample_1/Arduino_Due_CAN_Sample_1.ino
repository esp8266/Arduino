// Arduino Due - CAN Sample 1
// Brief CAN example for Arduino Due
// Test the transmission from CAN0 Mailbox 0 to CAN1 Mailbox 0
// By Thibaut Viard/Wilfredo Molina 2012

// Required libraries
#include "variant.h"
#include <CAN.h>

#define TEST1_CAN_COMM_MB_IDX    0
#define TEST1_CAN_TRANSFER_ID    0x07
#define TEST1_CAN0_TX_PRIO       15
#define CAN_MSG_DUMMY_DATA       0x55AAAA55

// CAN frame max data length
#define MAX_CAN_FRAME_DATA_LEN   8

// CAN class
CANRaw CAN;

// Message variable to be send
uint32_t CAN_MSG_1 = 0;

// CAN0 Transceiver
SSN65HVD234_Data can0_transceiver;

// CAN1 Transceiver
SSN65HVD234_Data can1_transceiver;

// Define the struct for CAN message mailboxes needed
can_mb_conf_t can0_mailbox;
can_mb_conf_t can1_mailbox;

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  Serial.println("Type CAN message to send");
  while (Serial.available() == 0);
}
void loop(){

  while (Serial.available() > 0) {
    CAN_MSG_1 = Serial.parseInt();
    if (Serial.read() == '\n') {
      Serial.print("Sent value= ");
      Serial.println(CAN_MSG_1);
    }
  }

  // Initialize CAN0 Transceiver
  SN65HVD234_Init(&can0_transceiver);
  SN65HVD234_SetRs(&can0_transceiver, 61);
  SN65HVD234_SetEN(&can0_transceiver, 62);
  // Enable CAN0 Transceiver
  SN65HVD234_DisableLowPower(&can0_transceiver);
  SN65HVD234_Enable(&can0_transceiver);

  // Initialize CAN1 Transceiver
  SN65HVD234_Init(&can1_transceiver);
  SN65HVD234_SetRs(&can1_transceiver, 63);
  SN65HVD234_SetEN(&can1_transceiver, 64);
  // Enable CAN1 Transceiver
  SN65HVD234_DisableLowPower(&can1_transceiver);
  SN65HVD234_Enable(&can1_transceiver);

  // Enable CAN0 & CAN1 clock
  pmc_enable_periph_clk(ID_CAN0);
  pmc_enable_periph_clk(ID_CAN1);

  // Initialize CAN0 and CAN1, baudrate is 1Mb/s
  CAN.init(CAN0, SystemCoreClock, CAN_BPS_1000K);
  CAN.init(CAN1, SystemCoreClock, CAN_BPS_1000K);

  // Initialize CAN1 mailbox 0 as receiver, frame ID is 0x07
  // can_reset_mailbox_data(&can1_mailbox);
  can1_mailbox.ul_mb_idx = TEST1_CAN_COMM_MB_IDX;
  can1_mailbox.uc_obj_type = CAN_MB_RX_MODE;
  can1_mailbox.ul_id_msk = CAN_MAM_MIDvA_Msk | CAN_MAM_MIDvB_Msk;
  can1_mailbox.ul_id = CAN_MID_MIDvA(TEST1_CAN_TRANSFER_ID);
  CAN.mailbox_init(CAN1, &can1_mailbox);

  // Initialize CAN0 mailbox 0 as transmitter, transmit priority is 15
  // can_reset_mailbox_data(&can0_mailbox);
  can0_mailbox.ul_mb_idx = TEST1_CAN_COMM_MB_IDX;
  can0_mailbox.uc_obj_type = CAN_MB_TX_MODE;
  can0_mailbox.uc_tx_prio = TEST1_CAN0_TX_PRIO;
  can0_mailbox.uc_id_ver = 0;
  can0_mailbox.ul_id_msk = 0;
  CAN.mailbox_init(CAN0, &can0_mailbox);

  // Prepare transmit ID, data and data length in CAN0 mailbox 0
  can0_mailbox.ul_id = CAN_MID_MIDvA(TEST1_CAN_TRANSFER_ID);
  can0_mailbox.ul_datal = CAN_MSG_1;
  can0_mailbox.ul_datah = CAN_MSG_DUMMY_DATA;
  can0_mailbox.uc_length = MAX_CAN_FRAME_DATA_LEN;
  CAN.mailbox_write(CAN0, &can0_mailbox);

  // Send out the information in the mailbox
  CAN.global_send_transfer_cmd(CAN0, CAN_TCR_MB0);

  // Wait for CAN1 mailbox 0 to receive the data
  while (!(CAN.mailbox_get_status(CAN1, 0) & CAN_MSR_MRDY)) {
  }

  // Read the received data from CAN1 mailbox 0
  CAN.mailbox_read(CAN1, &can1_mailbox);
  Serial.print("CAN message received= ");
  Serial.println(can1_mailbox.ul_datal);

  // Disable CAN0 Controller
  CAN.disable(CAN0);
  // Disable CAN0 Transceiver
  SN65HVD234_EnableLowPower(&can0_transceiver);
  SN65HVD234_Disable(&can0_transceiver);

  // Disable CAN1 Controller
  CAN.disable(CAN1);
  // Disable CAN1 Transceiver
  SN65HVD234_EnableLowPower(&can1_transceiver);
  SN65HVD234_Disable(&can1_transceiver);

  Serial.print("End of test");

  while (1) {
  }
}

