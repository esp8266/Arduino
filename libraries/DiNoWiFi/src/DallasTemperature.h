#ifndef DallasTemperature_h
#define DallasTemperature_h

#define DALLASTEMPLIBVERSION "3.7.3"

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// set to true to include code for new and delete operators
#ifndef REQUIRESNEW
#define REQUIRESNEW false
#endif

// set to true to include code implementing alarm search functions
#ifndef REQUIRESALARMS
#define REQUIRESALARMS true
#endif

#include <inttypes.h>
#include <OneWire.h>

// Model IDs
#define DS18S20MODEL 0x10  // also DS1820
#define DS18B20MODEL 0x28
#define DS1822MODEL  0x22
#define DS1825MODEL  0x3B

// OneWire commands
#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy EEPROM
#define READSCRATCH     0xBE  // Read EEPROM
#define WRITESCRATCH    0x4E  // Write to EEPROM
#define RECALLSCRATCH   0xB8  // Reload from last known
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

// Scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define HIGH_ALARM_TEMP 2
#define LOW_ALARM_TEMP  3
#define CONFIGURATION   4
#define INTERNAL_BYTE   5
#define COUNT_REMAIN    6
#define COUNT_PER_C     7
#define SCRATCHPAD_CRC  8

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

// Error Codes
#define DEVICE_DISCONNECTED_C -127
#define DEVICE_DISCONNECTED_F -196.6
#define DEVICE_DISCONNECTED_RAW -7040

typedef uint8_t DeviceAddress[8];

class DallasTemperature
{
public:

    DallasTemperature();
    DallasTemperature(OneWire*);

    void setOneWire(OneWire*);

    // initialise bus
    void begin(void);

    // returns the number of devices found on the bus
    uint8_t getDeviceCount(void);

    // returns true if address is valid
    bool validAddress(const uint8_t*);

    // returns true if address is of the family of sensors the lib supports.
    bool validFamily(const uint8_t* deviceAddress);

    // finds an address at a given index on the bus
    bool getAddress(uint8_t*, uint8_t);

    // attempt to determine if the device at the given address is connected to the bus
    bool isConnected(const uint8_t*);

    // attempt to determine if the device at the given address is connected to the bus
    // also allows for updating the read scratchpad
    bool isConnected(const uint8_t*, uint8_t*);

    // read device's scratchpad
    bool readScratchPad(const uint8_t*, uint8_t*);

    // write device's scratchpad
    void writeScratchPad(const uint8_t*, const uint8_t*);

    // read device's power requirements
    bool readPowerSupply(const uint8_t*);

    // get global resolution
    uint8_t getResolution();

    // set global resolution to 9, 10, 11, or 12 bits
    void setResolution(uint8_t);

    // returns the device resolution: 9, 10, 11, or 12 bits
    uint8_t getResolution(const uint8_t*);

    // set resolution of a device to 9, 10, 11, or 12 bits
    bool setResolution(const uint8_t*, uint8_t);

    // sets/gets the waitForConversion flag
    void setWaitForConversion(bool);
    bool getWaitForConversion(void);

    // sets/gets the checkForConversion flag
    void setCheckForConversion(bool);
    bool getCheckForConversion(void);

    // sends command for all devices on the bus to perform a temperature conversion
    void requestTemperatures(void);

    // sends command for one device to perform a temperature conversion by address
    bool requestTemperaturesByAddress(const uint8_t*);

    // sends command for one device to perform a temperature conversion by index
    bool requestTemperaturesByIndex(uint8_t);

    // returns temperature raw value (12 bit integer of 1/128 degrees C)
    int16_t getTemp(const uint8_t*);

    // returns temperature in degrees C
    float getTempC(const uint8_t*);

    // returns temperature in degrees F
    float getTempF(const uint8_t*);

    // Get temperature for device index (slow)
    float getTempCByIndex(uint8_t);

    // Get temperature for device index (slow)
    float getTempFByIndex(uint8_t);

    // returns true if the bus requires parasite power
    bool isParasitePowerMode(void);

    bool isConversionAvailable(const uint8_t*);

#if REQUIRESALARMS

    typedef void AlarmHandler(const uint8_t*);

    // sets the high alarm temperature for a device
    // accepts a char.  valid range is -55C - 125C
    void setHighAlarmTemp(const uint8_t*, char);

    // sets the low alarm temperature for a device
    // accepts a char.  valid range is -55C - 125C
    void setLowAlarmTemp(const uint8_t*, char);

    // returns a signed char with the current high alarm temperature for a device
    // in the range -55C - 125C
    char getHighAlarmTemp(const uint8_t*);

    // returns a signed char with the current low alarm temperature for a device
    // in the range -55C - 125C
    char getLowAlarmTemp(const uint8_t*);

    // resets internal variables used for the alarm search
    void resetAlarmSearch(void);

    // search the wire for devices with active alarms
    bool alarmSearch(uint8_t*);

    // returns true if ia specific device has an alarm
    bool hasAlarm(const uint8_t*);

    // returns true if any device is reporting an alarm on the bus
    bool hasAlarm(void);

    // runs the alarm handler for all devices returned by alarmSearch()
    void processAlarms(void);

    // sets the alarm handler
    void setAlarmHandler(const AlarmHandler *);

    // The default alarm handler
    static void defaultAlarmHandler(const uint8_t*);

#endif

    // if no alarm handler is used the two bytes can be used as user data
    // example of such usage is an ID.
    // note if device is not connected it will fail writing the data.
    // note if address cannot be found no error will be reported.
    // in short use carefully
    void setUserData(const uint8_t*, int16_t );
    void setUserDataByIndex(uint8_t, int16_t );
    int16_t getUserData(const uint8_t* );
    int16_t getUserDataByIndex(uint8_t );

    // convert from Celsius to Fahrenheit
    static float toFahrenheit(float);

    // convert from Fahrenheit to Celsius
    static float toCelsius(float);

    // convert from raw to Celsius
    static float rawToCelsius(int16_t);

    // convert from raw to Fahrenheit
    static float rawToFahrenheit(int16_t);

#if REQUIRESNEW

    // initialize memory area
    void* operator new (unsigned int);

    // delete memory reference
    void operator delete(void*);

#endif

private:
    typedef uint8_t ScratchPad[9];

    // parasite power on or off
    bool parasite;

    // used to determine the delay amount needed to allow for the
    // temperature conversion to take place
    uint8_t bitResolution;

    // used to requestTemperature with or without delay
    bool waitForConversion;

    // used to requestTemperature to dynamically check if a conversion is complete
    bool checkForConversion;

    // count of devices on the bus
    uint8_t devices;

    // Take a pointer to one wire instance
    OneWire* _wire;

    // reads scratchpad and returns the raw temperature
    int16_t calculateTemperature(const uint8_t*, uint8_t*);

    int16_t millisToWaitForConversion(uint8_t);

    void	blockTillConversionComplete(uint8_t, const uint8_t*);

#if REQUIRESALARMS

    // required for alarmSearch
    uint8_t alarmSearchAddress[8];
    char alarmSearchJunction;
    uint8_t alarmSearchExhausted;

    // the alarm handler function pointer
    AlarmHandler *_AlarmHandler;

#endif

};
#endif