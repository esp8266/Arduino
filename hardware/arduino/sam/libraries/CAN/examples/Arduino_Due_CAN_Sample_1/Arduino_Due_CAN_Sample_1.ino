
// Brief CAN example for Arduino Due
// Test the transmission from CAN0 Mailbox 0 to CAN1 Mailbox 0
// Modified from Atmel ASF SAM3X-EK CAN sample by Wilfredo Molina 2012

// Required libraries
#include <can.h>
#include <sysclk.h>
#include <sam3x8e.h>

#define TEST1_CAN_COMM_MB_IDX    0
#define TEST1_CAN_TRANSFER_ID    0x07
#define TEST1_CAN0_TX_PRIO       15
#define CAN_MSG_DUMMY_DATA       0x55AAAA55

/** CAN frame max data length */
#define MAX_CAN_FRAME_DATA_LEN   8

//Message variable to be send
uint32_t CAN_MSG_1 = 0;

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
      delay(1000);  
    }
  }
  
  uint32_t ul_sysclk;
  
  /* Initialize the SAM system. */
  sysclk_init();
  board_init();
  
  // Enable CAN0 & CAN1 clock 
  pmc_enable_periph_clk(ID_CAN0);
  pmc_enable_periph_clk(ID_CAN1);
   
  // Initialize CAN0 and CAN1, baudrate is 1Mb/s
  ul_sysclk = sysclk_get_cpu_hz();
  can_init(CAN0, ul_sysclk, CAN_BPS_1000K);
  can_init(CAN1, ul_sysclk, CAN_BPS_1000K);
  
  // Reset all CAN0 and CAN1 mailboxes
  can_reset_all_mailbox(CAN0);
  can_reset_all_mailbox(CAN1);
   
  // Initialize CAN1 mailbox 0 as receiver, frame ID is 0x07
  reset_mailbox_conf(&can1_mailbox);
  can1_mailbox.ul_mb_idx = TEST1_CAN_COMM_MB_IDX;
  can1_mailbox.uc_obj_type = CAN_MB_RX_MODE;
  can1_mailbox.ul_id_msk = CAN_MAM_MIDvA_Msk | CAN_MAM_MIDvB_Msk;
  can1_mailbox.ul_id = CAN_MID_MIDvA(TEST1_CAN_TRANSFER_ID);
  can_mailbox_init(CAN1, &can1_mailbox);
  
  // Initialize CAN0 mailbox 0 as transmitter, transmit priority is 15
  reset_mailbox_conf(&can0_mailbox);
  can0_mailbox.ul_mb_idx = TEST1_CAN_COMM_MB_IDX;
  can0_mailbox.uc_obj_type = CAN_MB_TX_MODE;
  can0_mailbox.uc_tx_prio = TEST1_CAN0_TX_PRIO;
  can0_mailbox.uc_id_ver = 0;
  can0_mailbox.ul_id_msk = 0;
  can_mailbox_init(CAN0, &can0_mailbox);

  // Prepare transmit ID, data and data length in CAN0 mailbox 0 
  can0_mailbox.ul_id = CAN_MID_MIDvA(TEST1_CAN_TRANSFER_ID);
  can0_mailbox.ul_datal = CAN_MSG_1;
  can0_mailbox.ul_datah = CAN_MSG_DUMMY_DATA;
  can0_mailbox.uc_length = MAX_CAN_FRAME_DATA_LEN;
  can_mailbox_write(CAN0, &can0_mailbox);
  
  // Send out the information in the mailbox
  can_global_send_transfer_cmd(CAN0, CAN_TCR_MB0);
  
  // Wait for CAN1 mailbox 0 to receive the data
  while (!(can_mailbox_get_status(CAN1, 0) & CAN_MSR_MRDY)) {
 }
  // Read the received data from CAN1 mailbox 0
  can_mailbox_read(CAN1, &can1_mailbox);
  Serial.print("CAN message received= ");
  Serial.println(can1_mailbox.ul_datal);

  while (1) {}
}
