/*****************************************************************************
 * @file     ReadMe.txt
 * @purpose  Explanation how to use the DeviceSupport folder and template files 
 * @version  V2.00
 * @date     24. March 2010
 *****************************************************************************/

Following directory structure and template files are given:

  - Vendor
      |
      +-- Device
            +- Device.h                         Device header file 
            +- system_Device.h                  Device system include file 
            +- system_Device.c                  Device system source file 
            |
            +-- startup
                  |
                  +-- arm
                  |    +- startup_Device.s      Device startup file for toolchain ARM
                  |
                  +-- gcc
                  |
                  +-- iar


Copy the complete folder including files and replace:
  - folder name 'Vendor' with the abbreviation for the device vendor  e.g.: NXP. 
  - folder name 'Device' with your specific device name e.g.: LPC17xx.
  - in the filenames 'Device'with your specific device name e.g.: LPC17xx. 


The template files contain comments starting with 'ToDo: '
There it is described what you need to do.


The template files contain following placeholder:

  <Device>
  <Device> should be replaced with your specific device name.
   e.g.: LPC17xx
  
  <DeviceInterrupt>
  <DeviceInterrupt> should be replaced with a specific device interrupt name.
  e.g.: TIM1 for Timer#1 interrupt.

  <DeviceAbbreviation>
  <DeviceAbbreviation> should be replaced with a dedicated device family abbreviation.
  e.g.: LPC for LPC17xx device family.

  Cortex-M#
  Cortex-M# can be replaced with the specific Cortex-M number
  e.g.: Cortex-M3

  



