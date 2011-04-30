#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "WProgram.h"
#include "spi_drv.h"
#include "wifi_drv.h"

#define _DEBUG_

extern "C" {
#include "wifi_spi.h"
#include "wl_types.h"
#include "debug.h"
}

char 	WiFiDrv::_networkSsid[][WL_SSID_MAX_LENGTH] = {{"1"},{"2"},{"3"},{"4"},{"5"}};
char 	WiFiDrv::_ssid[] = {0};
uint8_t	WiFiDrv::_bssid[] = {0};
uint8_t WiFiDrv::_mac[] = {0};
uint8_t WiFiDrv::_localIp[] = {0};
uint8_t WiFiDrv::_subnetMask[] = {0};
uint8_t WiFiDrv::_gatewayIp[] = {0};


// Private Methods

void WiFiDrv::getNetworkData(uint8_t *ip, uint8_t *mask, uint8_t *gwip)
{
    tParam params[PARAM_NUMS_3] = { {0, (char*)ip}, {0, (char*)mask}, {0, (char*)gwip}};

    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_IPADDR_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, sizeof(_dummy), LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    SpiDrv::waitResponseParams(GET_IPADDR_CMD, PARAM_NUMS_3, params);

    SpiDrv::spiSlaveDeselect();
}

// Public Methods


void WiFiDrv::wifiDriverInit()
{
    SpiDrv::begin();
}

// If ssid == NULL execute a wifi scan, otherwise try to connect to the network specified
uint8_t WiFiDrv::wifiSetNetwork(char* ssid, uint8_t ssid_len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_NET_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam((uint8_t*)ssid, ssid_len, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(SET_NET_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();

    return(_data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
}

uint8_t WiFiDrv::wifiSetPassphrase(char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_PASSPHRASE_CMD, PARAM_NUMS_2);
    SpiDrv::sendParam((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)passphrase, len, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(SET_PASSPHRASE_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}


uint8_t WiFiDrv::wifiSetKey(char* ssid, uint8_t ssid_len, uint8_t key_idx, const void *key, const uint8_t len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SET_KEY_CMD, PARAM_NUMS_3);
    SpiDrv::sendParam((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
    SpiDrv::sendParam(&key_idx, KEY_IDX_LEN, NO_LAST_PARAM);
    SpiDrv::sendParam((uint8_t*)key, len, LAST_PARAM);
    
    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(SET_KEY_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
    return _data;
}
                        
uint8_t WiFiDrv::disconnect()
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(DISCONNECT_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    uint8_t result = SpiDrv::waitResponseCmd(DISCONNECT_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return result;
}

uint8_t WiFiDrv::getConnectionStatus()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_CONN_STATUS_CMD, PARAM_NUMS_0);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    SpiDrv::waitResponseCmd(GET_CONN_STATUS_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return _data;
}

uint8_t* WiFiDrv::getMacAddress()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_MACADDR_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _dataLen = 0;
    uint8_t result = SpiDrv::waitResponseCmd(GET_MACADDR_CMD, PARAM_NUMS_1, _mac, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return _mac;
}

void WiFiDrv::getIpAddress(uint8_t *ip)
{
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	memcpy(ip, _localIp, WL_IPV4_LENGTH);
}

 void WiFiDrv::getSubnetMask(uint8_t *ip)
 {
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	memcpy(ip, _subnetMask, WL_IPV4_LENGTH);
 }

 void WiFiDrv::getGatewayIP(uint8_t *ip)
 {
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	memcpy(ip, _gatewayIp, WL_IPV4_LENGTH);
 }

char* WiFiDrv::getCurrentSSID()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_CURR_SSID_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _dataLen = 0;
    uint8_t result = SpiDrv::waitResponseCmd(GET_CURR_SSID_CMD, PARAM_NUMS_1, (uint8_t*)_ssid, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return _ssid;
}

uint8_t* WiFiDrv::getCurrentBSSID()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_CURR_BSSID_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _dataLen = 0;
    uint8_t result = SpiDrv::waitResponseCmd(GET_CURR_BSSID_CMD, PARAM_NUMS_1, _bssid, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return _bssid;
}

int32_t WiFiDrv::getCurrentRSSI()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_CURR_RSSI_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _dataLen = 0;
    int32_t rssi = 0;
    uint8_t result = SpiDrv::waitResponseCmd(GET_CURR_RSSI_CMD, PARAM_NUMS_1, (uint8_t*)&rssi, &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return rssi;
}

uint8_t WiFiDrv::getCurrentEncryptionType()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(GET_CURR_ENCT_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t dataLen = 0;
    uint8_t encType = 0;
    uint8_t result = SpiDrv::waitResponseCmd(GET_CURR_ENCT_CMD, PARAM_NUMS_1, (uint8_t*)encType, &dataLen);

    SpiDrv::spiSlaveDeselect();

    return encType;
}

uint8_t WiFiDrv::scanNetworks()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    SpiDrv::sendCmd(SCAN_NETWORKS, PARAM_NUMS_0);

//    uint8_t _dummy = DUMMY_DATA;
//    SpiDrv::sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint16_t _dataLen = 0;

    tParam params[WL_NETWORKS_LIST_MAXNUM];

    uint8_t ssidListNum = 0;
    uint8_t result = SpiDrv::waitResponse(SCAN_NETWORKS, &ssidListNum, (uint8_t**)_networkSsid, WL_NETWORKS_LIST_MAXNUM);

    SpiDrv::spiSlaveDeselect();

    return ssidListNum;
}

char* WiFiDrv::getSSIDNetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return NULL;

	return _networkSsid[networkItem];
}

uint8_t WiFiDrv::getEncTypeNetowrks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return NULL;
	uint8_t networkEncType = 0;

	//TODO make an RPC call to get the encryption type associated with networkItem
	return networkEncType;
}

int32_t WiFiDrv::getRSSINetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return NULL;
	int32_t	networkRssi = 0;

	//TODO make an RPC call to get the rssi associated with networkItem
	return networkRssi;
}

WiFiDrv wiFiDrv;
