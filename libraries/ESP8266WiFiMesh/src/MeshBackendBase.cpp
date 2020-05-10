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
#include "TypeConversionFunctions.h"
#include "MutexTracker.h"

#include <assert.h>

namespace
{
  namespace TypeCast = MeshTypeConversionFunctions;
  
  MeshBackendBase *apController = nullptr;
}

std::shared_ptr<bool> MeshBackendBase::_scanMutex = std::make_shared<bool>(false);

bool MeshBackendBase::_printWarnings = true;

MeshBackendBase::MeshBackendBase(const requestHandlerType requestHandler, const responseHandlerType responseHandler, const networkFilterType networkFilter, const MeshBackendType classType)
{
  setRequestHandler(requestHandler);
  setResponseHandler(responseHandler);
  setNetworkFilter(networkFilter);
  setClassType(classType);
}

MeshBackendBase::~MeshBackendBase()
{
  deactivateControlledAP();
}

void MeshBackendBase::setClassType(const MeshBackendType classType)
{
  _classType = classType;
}

MeshBackendType MeshBackendBase::getClassType() const {return _classType;}

void MeshBackendBase::activateAP()
{
  // Deactivate active AP to avoid two servers using the same port, which can lead to crashes.
  deactivateAP();

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
  if(MeshBackendBase *currentAPController = MeshBackendBase::getAPController())
    currentAPController->deactivateControlledAP();
}

bool MeshBackendBase::deactivateControlledAP()
{
  if(isAPController())
  {
    deactivateAPHook();

    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);

    // Since there is no active AP controller now, make the apController variable point to nothing. 
    apController = nullptr;

    return true;
  }

  return false;
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

bool MeshBackendBase::isAPController() const
{
  return (this == getAPController());
}

void MeshBackendBase::setWiFiChannel(const uint8 newWiFiChannel)
{
  wifi_country_t wifiCountry;
  wifi_get_country(&wifiCountry); // Note: Should return 0 on success and -1 on failure, but always seems to return 1. Possibly broken API. Channels 1 to 13 are the default limits.
  assert(wifiCountry.schan <= newWiFiChannel && newWiFiChannel <= wifiCountry.schan + wifiCountry.nchan - 1);
  
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

uint8 MeshBackendBase::getWiFiChannel() const
{
  return _meshWiFiChannel;
}

void MeshBackendBase::setSSID(const String &newSSIDPrefix, const String &newSSIDRoot, const String &newSSIDSuffix)
{
  if(!newSSIDPrefix.isEmpty())
    _SSIDPrefix = newSSIDPrefix;
  if(!newSSIDRoot.isEmpty())
    _SSIDRoot = newSSIDRoot;
  if(!newSSIDSuffix.isEmpty())
    _SSIDSuffix = newSSIDSuffix;

  String newSSID = _SSIDPrefix + _SSIDRoot + _SSIDSuffix;

  assert(newSSID.length() <= 31);

  if(getSSID() != newSSID)
  {
    _SSID = newSSID;
  
    // Apply SSID changes to active AP.
    if(isAPController())
      restartAP();
  }
}

String MeshBackendBase::getSSID() const {return _SSID;}

void MeshBackendBase::setSSIDPrefix(const String &newSSIDPrefix)
{
  setSSID(newSSIDPrefix);
}

String MeshBackendBase::getSSIDPrefix() const {return _SSIDPrefix;}

void MeshBackendBase::setSSIDRoot(const String &newSSIDRoot)
{
  setSSID(emptyString, newSSIDRoot);
}

String MeshBackendBase::getSSIDRoot() const {return _SSIDRoot;}

void MeshBackendBase::setSSIDSuffix(const String &newSSIDSuffix)
{
  setSSID(emptyString, emptyString, newSSIDSuffix);
}

String MeshBackendBase::getSSIDSuffix() const {return _SSIDSuffix;}

void MeshBackendBase::setMeshName(const String &newMeshName)
{
  setSSIDPrefix(newMeshName);
}

String MeshBackendBase::getMeshName() const {return getSSIDPrefix();}

void MeshBackendBase::setNodeID(const String &newNodeID)
{
  setSSIDSuffix(newNodeID);
}

String MeshBackendBase::getNodeID() const {return getSSIDSuffix();}

void MeshBackendBase::setMeshPassword(const String &newMeshPassword)
{
  assert(8 <= newMeshPassword.length() && newMeshPassword.length() <= 64); // Limited by the ESP8266 API.
  assert(newMeshPassword.indexOf('"') == -1); // " is not allowed in passwords to allow for easier JSON parsing and predictable password length (no need for extra escape characters).
  
  _meshPassword = newMeshPassword;

  // Apply changes to active AP.
  if(isAPController())
    restartAP();
}

String MeshBackendBase::getMeshPassword() const {return _meshPassword;}

void MeshBackendBase::setMessage(const String &newMessage) {_message = newMessage;}
String MeshBackendBase::getMessage() const {return _message;}

void MeshBackendBase::setRequestHandler(const MeshBackendBase::requestHandlerType requestHandler) {_requestHandler = requestHandler;}
MeshBackendBase::requestHandlerType MeshBackendBase::getRequestHandler() const {return _requestHandler;}

void MeshBackendBase::setResponseHandler(const MeshBackendBase::responseHandlerType responseHandler) {_responseHandler = responseHandler;}
MeshBackendBase::responseHandlerType MeshBackendBase::getResponseHandler() const {return _responseHandler;}

void MeshBackendBase::setNetworkFilter(const MeshBackendBase::networkFilterType networkFilter) {_networkFilter = networkFilter;}
MeshBackendBase::networkFilterType MeshBackendBase::getNetworkFilter() const {return _networkFilter;}

void MeshBackendBase::setTransmissionOutcomesUpdateHook(const MeshBackendBase::transmissionOutcomesUpdateHookType transmissionOutcomesUpdateHook) {_transmissionOutcomesUpdateHook = transmissionOutcomesUpdateHook;}
MeshBackendBase::transmissionOutcomesUpdateHookType MeshBackendBase::getTransmissionOutcomesUpdateHook() const {return _transmissionOutcomesUpdateHook;}

void MeshBackendBase::setScanHidden(const bool scanHidden)
{
  _scanHidden = scanHidden;
}

bool MeshBackendBase::getScanHidden() const {return _scanHidden;}

void MeshBackendBase::setAPHidden(const bool apHidden)
{
  if(getAPHidden() != apHidden)
  {
    _apHidden = apHidden;
    
    // Apply changes to active AP.
    if(isAPController())
      restartAP();
  }
}

bool MeshBackendBase::getAPHidden() const {return _apHidden;}

bool MeshBackendBase::latestTransmissionSuccessfulBase(const std::vector<TransmissionOutcome> &latestTransmissionOutcomes)
{
  if(latestTransmissionOutcomes.empty())
    return false;

  for(const TransmissionOutcome &transmissionOutcome : latestTransmissionOutcomes) 
    if(transmissionOutcome.transmissionStatus() != TransmissionStatusType::TRANSMISSION_COMPLETE) 
      return false;

  return true;
}

void MeshBackendBase::scanForNetworks(const bool scanAllWiFiChannels)
{
  MutexTracker mutexTracker(_scanMutex);
  if(!mutexTracker.mutexCaptured())
  {
    assert(false && String(F("ERROR! Scan already in progress. Don't call scanForNetworks from callbacks as this may corrupt program state! Aborting."))); 
    return;
  }
  
  verboseModePrint(F("Scanning... "), false);
      
  /* Scan for APs */

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

void MeshBackendBase::printAPInfo(const NetworkInfoBase &apNetworkInfo)
{
  String mainNetworkIdentifier = apNetworkInfo.SSID();
  if(mainNetworkIdentifier == NetworkInfoBase::defaultSSID) // If SSID not provided, use BSSID instead
  {
    mainNetworkIdentifier = TypeCast::macToString(apNetworkInfo.BSSID());
  }
  
  verboseModePrint(String(F("AP acquired: ")) + mainNetworkIdentifier + String(F(", Ch:")) + String(apNetworkInfo.wifiChannel()) + ' ', false);

  if(apNetworkInfo.RSSI() != NetworkInfoBase::defaultRSSI)
  {
    verboseModePrint(String('(') + String(apNetworkInfo.RSSI()) + String(F("dBm) ")) + 
                     (apNetworkInfo.encryptionType() == ENC_TYPE_NONE ? String(F("open")) : emptyString), false);
  }

  verboseModePrint(F("... "), false);
}
