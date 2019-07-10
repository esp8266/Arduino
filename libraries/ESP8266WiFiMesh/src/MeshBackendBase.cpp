/*
  MeshBackendBase
 
  Copyright (c) 2015 Julian Fell and 2019 Anders Löfgren. All rights reserved.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "MeshBackendBase.h"

#include <assert.h>

MeshBackendBase *MeshBackendBase::apController = nullptr;

std::vector<NetworkInfo> MeshBackendBase::connectionQueue = {};
std::vector<TransmissionResult> MeshBackendBase::latestTransmissionOutcomes = {};

bool MeshBackendBase::_printWarnings = true;

MeshBackendBase::MeshBackendBase(requestHandlerType requestHandler, responseHandlerType responseHandler, networkFilterType networkFilter, mesh_backend_t classType)
{
  setRequestHandler(requestHandler);
  setResponseHandler(responseHandler);
  setNetworkFilter(networkFilter);
  setClassType(classType);
}

MeshBackendBase::~MeshBackendBase()
{
  deactivateAP();
}

void MeshBackendBase::setClassType(mesh_backend_t classType)
{
  _classType = classType;
}

mesh_backend_t MeshBackendBase::getClassType() {return _classType;}

void MeshBackendBase::activateAP()
{
  // Deactivate active AP to avoid two servers using the same port, which can lead to crashes.
  if(MeshBackendBase *currentAPController = MeshBackendBase::getAPController())
    currentAPController->deactivateAP();

  activateAPHook();
  
  WiFi.mode(WIFI_AP_STA); 

  apController = this;
}

void MeshBackendBase::activateAPHook()
{
  WiFi.softAP( getSSID().c_str(), getMeshPassword().c_str(), getWiFiChannel(), getAPHidden() ); // Note that a maximum of 8 TCP/IP stations can be connected at a time to each AP, max 4 by default.
}

void MeshBackendBase::deactivateAP()
{
  if(isAPController())
  {
    deactivateAPHook();

    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);

    // Since there is no active AP controller now, make the apController variable point to nothing. 
    apController = nullptr;
  }
}

void MeshBackendBase::deactivateAPHook()
{
}

void MeshBackendBase::restartAP()
{
  deactivateAP();
  yield();
  activateAP();
  yield();
}

MeshBackendBase *MeshBackendBase::getAPController()
{
  return apController; 
}

bool MeshBackendBase::isAPController()
{
  return (this == getAPController());
}

void MeshBackendBase::setWiFiChannel(uint8 newWiFiChannel)
{
  assert(1 <= newWiFiChannel && newWiFiChannel <= 13);
  
  _meshWiFiChannel = newWiFiChannel;

  // WiFi.channel() will change if this node connects to an AP with another channel, 
  // so there is no guarantee we are using _meshWiFiChannel.
  // Also, we cannot change the WiFi channel while we are still connected to the other AP.
  if(WiFi.channel() != getWiFiChannel() && WiFi.status() != WL_CONNECTED)
  {  
    // Apply changes to active AP.
    if(isAPController())
      restartAP();
  }
}

uint8 MeshBackendBase::getWiFiChannel()
{
  return _meshWiFiChannel;
}

void MeshBackendBase::setSSID(const String &newSSIDPrefix, const String &newSSIDRoot, const String &newSSIDSuffix)
{
  if(newSSIDPrefix != "")
    _SSIDPrefix = newSSIDPrefix;
  if(newSSIDRoot != "")
    _SSIDRoot = newSSIDRoot;
  if(newSSIDSuffix != "")
    _SSIDSuffix = newSSIDSuffix;

  String newSSID = _SSIDPrefix + _SSIDRoot + _SSIDSuffix;

  if(getSSID() != newSSID)
  {
    _SSID = newSSID;
  
    // Apply SSID changes to active AP.
    if(isAPController())
      restartAP();
  }
}

String MeshBackendBase::getSSID() {return _SSID;}

void MeshBackendBase::setSSIDPrefix(const String &newSSIDPrefix)
{
  setSSID(newSSIDPrefix);
}

String MeshBackendBase::getSSIDPrefix() {return _SSIDPrefix;}

void MeshBackendBase::setSSIDRoot(const String &newSSIDRoot)
{
  setSSID("", newSSIDRoot);
}

String MeshBackendBase::getSSIDRoot() {return _SSIDRoot;}

void MeshBackendBase::setSSIDSuffix(const String &newSSIDSuffix)
{
  setSSID("", "", newSSIDSuffix);
}

String MeshBackendBase::getSSIDSuffix() {return _SSIDSuffix;}

void MeshBackendBase::setMeshName(const String &newMeshName)
{
  setSSIDPrefix(newMeshName);
}

String MeshBackendBase::getMeshName() {return getSSIDPrefix();}

void MeshBackendBase::setNodeID(const String &newNodeID)
{
  setSSIDSuffix(newNodeID);
}

String MeshBackendBase::getNodeID() {return getSSIDSuffix();}

void MeshBackendBase::setMeshPassword(const String &newMeshPassword)
{
  assert(8 <= newMeshPassword.length() && newMeshPassword.length() <= 64); // Limited by the ESP8266 API.
  
  _meshPassword = newMeshPassword;

  // Apply changes to active AP.
  if(isAPController())
    restartAP();
}

String MeshBackendBase::getMeshPassword() {return _meshPassword;}

void MeshBackendBase::setMessage(const String &newMessage) {_message = newMessage;}
String MeshBackendBase::getMessage() {return _message;}

void MeshBackendBase::setRequestHandler(MeshBackendBase::requestHandlerType requestHandler) {_requestHandler = requestHandler;}
MeshBackendBase::requestHandlerType MeshBackendBase::getRequestHandler() {return _requestHandler;}

void MeshBackendBase::setResponseHandler(MeshBackendBase::responseHandlerType responseHandler) {_responseHandler = responseHandler;}
MeshBackendBase::responseHandlerType MeshBackendBase::getResponseHandler() {return _responseHandler;}

void MeshBackendBase::setNetworkFilter(MeshBackendBase::networkFilterType networkFilter) {_networkFilter = networkFilter;}
MeshBackendBase::networkFilterType MeshBackendBase::getNetworkFilter() {return _networkFilter;}

void MeshBackendBase::setScanHidden(bool scanHidden)
{
  _scanHidden = scanHidden;
}

bool MeshBackendBase::getScanHidden() {return _scanHidden;}

void MeshBackendBase::setAPHidden(bool apHidden)
{
  if(getAPHidden() != apHidden)
  {
    _apHidden = apHidden;
    
    // Apply changes to active AP.
    if(isAPController())
      restartAP();
  }
}

bool MeshBackendBase::getAPHidden() {return _apHidden;}

bool MeshBackendBase::latestTransmissionSuccessful()
{
  if(MeshBackendBase::latestTransmissionOutcomes.empty())
    return false;
  else
    for(TransmissionResult &transmissionResult : MeshBackendBase::latestTransmissionOutcomes) 
      if(transmissionResult.transmissionStatus != TS_TRANSMISSION_COMPLETE) 
        return false;

  return true;
}

void MeshBackendBase::scanForNetworks(bool scanAllWiFiChannels)
{  
  verboseModePrint(F("Scanning... "), false);
      
  /* Scan for APs */
  connectionQueue.clear();

  // If scanAllWiFiChannels is true, scanning will cause the WiFi radio to cycle through all WiFi channels.
  // This means existing WiFi connections are likely to break or work poorly if done frequently.
  int n = 0;
  if(scanAllWiFiChannels)
  {
    n = WiFi.scanNetworks(false, getScanHidden());
  }
  else
  {
    // Scan function argument overview: scanNetworks(bool async = false, bool show_hidden = false, uint8 channel = 0, uint8* ssid = NULL)
    n = WiFi.scanNetworks(false, getScanHidden(), getWiFiChannel());
  }
  
  getNetworkFilter()(n, *this); // Update the connectionQueue.
}

void MeshBackendBase::printAPInfo(const int apNetworkIndex, const String &apSSID, const int apWiFiChannel)
{
   verboseModePrint(String(F("AP acquired: ")) + apSSID + String(F(", Ch:")) + String(apWiFiChannel) + " ", false);

  if(apNetworkIndex != NETWORK_INFO_DEFAULT_INT)
  {
    verboseModePrint("(" + String(WiFi.RSSI(apNetworkIndex)) + String(F("dBm) ")) + 
                     (WiFi.encryptionType(apNetworkIndex) == ENC_TYPE_NONE ? String(F("open")) : ""), false);
  }

  verboseModePrint(F("... "), false);
}
