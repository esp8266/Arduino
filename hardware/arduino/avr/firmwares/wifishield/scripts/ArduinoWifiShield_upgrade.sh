#!/bin/sh

WIFI_FW_PATH="/hardware/arduino/avr/firmwares/wifishield/binary"
AVR_TOOLS_PATH="/hardware/tools/avr/bin"

TARGET_MICRO="at32uc3a1256"


progname=$0

usage () {
cat <<EOF
Usage: $progname [-a Arduino_path] [-f which_firmware] [-h] 
       -a set the path where the Arduino IDE is installed
       -f the firmware you want to upload, valid parameters are:
	  shield  - to upgrade the WiFi shield firmware
	  all     - to upgrade both firmwares
       -h help
EOF
  exit 0
}

upgradeHDmodule () {
  sleep 1 # Give time to the shield to end the boot
  echo "****Upgrade HD WiFi module firmware****\n"
  dfu-programmer $TARGET_MICRO erase 
  dfu-programmer $TARGET_MICRO flash --suppress-bootloader-mem $WIFI_FW_PATH/wifi_dnld.hex
  dfu-programmer $TARGET_MICRO start
  
  if [ $? != 0 ] ; then 
  echo "\nError during device initialization, please close the J3 jumper and press the reset button.\nTry -h for help\n"
  exit 1 # if the device is not recognized exit 
  fi

  echo -n "\nPress the RESET button on the shield then type [ENTER] to upgrade the firmware of the shield..\n"
  read readEnter
}

upgradeShield () {
  sleep 1 # Give time to the shield to end the boot
  echo "****Upgrade WiFi Shield firmware****\n"
  dfu-programmer $TARGET_MICRO erase
  dfu-programmer $TARGET_MICRO flash --suppress-bootloader-mem $WIFI_FW_PATH/wifiHD.hex
  dfu-programmer $TARGET_MICRO start 

  if [ $? != 0 ] ;  then 
  echo "\nError during device initialization, please close the J3 jumper and press the reset button.\nTry -h for help\n"
  exit 1 # if the device is not recognized exit 
  fi

  echo "\nDone. Remove the J3 jumper and press the RESET button on the shield."
  echo "Thank you!\n"
}


cat <<EOF

       Arduino WiFi Shield upgrade
=========================================
Instructions:

To access to the USB devices correctly, the dfu-programmer needs to have the root permissions.

You can upgrade the firmware of the antenna togheter with the shield firmware or only the shield firmware
if there aren't changes on the antenna firmware.

Use the '-h' parameter for help
=========================================

EOF

if [ $USER = 'root' ] ; then  #check if the current user is root
  while getopts ":a:f:h" opt; do
    case $opt in
	a)
	  ARDUINO_PATH=$OPTARG 
	  WIFI_FW_PATH=$ARDUINO_PATH$WIFI_FW_PATH
	  AVR_TOOLS_PATH=$ARDUINO_PATH$AVR_TOOLS_PATH
	  cd $AVR_TOOLS_PATH
	  ./avr-objcopy --output-target=ihex $WIFI_FW_PATH/wifi_dnld.elf  $WIFI_FW_PATH/wifi_dnld.hex	
	  ./avr-objcopy --output-target=ihex $WIFI_FW_PATH/wifiHD.elf  $WIFI_FW_PATH/wifiHD.hex
	  ;;
	f)
	  if [ "$ARDUINO_PATH" != "" ] ; then
	    if [ "$OPTARG" = "all" ] ; then
	      upgradeHDmodule
	      upgradeShield
	      exit 0
	    else
	      if [ "$OPTARG" = "shield" ] ; then
		upgradeShield
		exit 0
	      else
		echo "invalid parameter for the -f [firmware] option, please retry."
		echo "Type -h for help\n"
		exit 1
	      fi
	     fi
	  else
	    echo "Arduino Path not setted. Retry...\n"
	  fi
	  ;;
	h)
	  usage ;; 
	\?)
	  echo "Invalid option: $OPTARG" >&2
	  usage
	  exit 1
	  ;;
	:)
	  echo "Option -$OPTARG requires an argument." >&2
	  exit 1
	  ;;
	esac
  done
else
  echo "Please retry running the script as root.\n"
fi

shift $(($OPTIND - 1))
