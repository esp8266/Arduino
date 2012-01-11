/*****************************************************************************
 * @file     ReadMe.txt
 * @purpose  Explanation how to use the Device folder and template files 
 * @version  V2.10
 * @date     25. July 2011
 *****************************************************************************/

Following directory structure and template files are given:

  - <Vendor>
      |
      +-- <Device>
            |
            +-- Include
            |     +- <Device>.h                           header file 
            |     +- system_Device.h                      system include file 
            +-- Source
                  |
                  + Templates
                      +- system_<Device>.c                system source file 
                      |
                      +-- ARM
                      |    +- startup_<Device>.s          startup file for ARMCC
                      |
                      +-- GCC
                      |
                      +-- IAR


Copy the complete folder including files and replace:
  - folder name 'Vendor' with the abbreviation for the device vendor  e.g.: NXP. 
  - folder name 'Device' with your specific device name e.g.: LPC17xx.
  - in the filenames 'Device' with your specific device name e.g.: LPC17xx. 


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
  <DeviceAbbreviation> should be replaced with a dedicated device family
  abbreviation (e.g.: LPC for LPC17xx device family)

  Cortex-M#
  Cortex-M# can be replaced with the specific Cortex-M number
  e.g.: Cortex-M3



Note:
  Template files (i.e. startup_Device.s, system_Device.c) are application
  specific and therefore expected to be copied into the application project
  folder prior to use!
  