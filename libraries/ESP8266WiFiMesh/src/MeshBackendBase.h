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

#ifndef __MESHBACKENDBASE_H__
#define __MESHBACKENDBASE_H__

#include <ESP8266WiFi.h>
#include "TransmissionOutcome.h"
#include "NetworkInfoBase.h"

enum class MeshBackendType
{
  TCP_IP = 0,
  ESP_NOW = 1
};

class MeshBackendBase {

protected:

  using requestHandlerType = std::function<String(const String &, MeshBackendBase &)> ;
  using responseHandlerType = std::function<TransmissionStatusType(const String &, MeshBackendBase &)>;
  using networkFilterType = std::function<void(int, MeshBackendBase &)>;
  using transmissionOutcomesUpdateHookType = std::function<bool(MeshBackendBase &)>;

public:

  MeshBackendBase(requestHandlerType requestHandler, responseHandlerType responseHandler, networkFilterType networkFilter, MeshBackendType classType);

  virtual ~MeshBackendBase();

  /**
   * Initialises the node.
   */
  virtual void begin() = 0;

  /**
   * Activate the WiFi access point of this ESP8266.
   * 
   * For TCP/IP, each AP requires a separate server port. If two AP:s are using the same server port, they will not be able to have both server instances active at the same time.
   * This is managed automatically by the activateAP method.
   * 
   * Note that only one AP can be active at a time in total (there is only one WiFi radio on the ESP8266), and this will always be the one which was last activated.
   * Thus the AP is shared by all backends.
   */
  void activateAP();

  /** 
   * Deactivate the WiFi access point of this ESP8266, regardless of which MeshBackendBase instance is in control of the AP.
   * 
   * Note that only one AP can be active at a time in total (there is only one WiFi radio on the ESP8266), and this will always be the one which was last activated.
   * Thus the AP is shared by all backends.
   */
  static void deactivateAP();
  
  /** 
   * Deactivate the WiFi access point of this ESP8266, provided that this MeshBackendBase instance is in control of the AP (which normally is the case for the MeshBackendBase instance that did the last activateAP() call).
   * 
   * Note that only one AP can be active at a time in total (there is only one WiFi radio on the ESP8266), and this will always be the one which was last activated.
   * Thus the AP is shared by all backends.
   * 
   * @return True if the AP was deactivated. False otherwise.
   */
  bool deactivateControlledAP();

  /**
   * Restart the WiFi access point of this ESP8266.
   * 
   * Note that only one AP can be active at a time in total (there is only one WiFi radio on the ESP8266), and this will always be the one which was last activated.
   * Thus the AP is shared by all backends.
   */
  void restartAP();

  /**
   * Get the MeshBackendBase instance currently in control of the ESP8266 AP.
   * Note that the result will be nullptr when there is no active AP controller.
   * If another instance takes control over the AP after the pointer is created,
   * the created pointer will still point to the old AP instance.
   * 
   * @return A pointer to the MeshBackendBase instance currently in control of the ESP8266 AP,
   *          or nullptr if there is no active AP controller.
   */
  static MeshBackendBase *getAPController();
  
  /**
   * Check if this MeshBackendBase instance is in control of the ESP8266 AP.
   * 
   * @return True if this MeshBackendBase instance is in control of the ESP8266 AP. False otherwise.
   */
  bool isAPController();

  /**
   * Change the WiFi channel used by this MeshBackendBase instance. 
   * Will also change the WiFi channel for the active AP (via an AP restart)
   * if this MeshBackendBase instance is the current AP controller and it is possible to change channel.
   * 
   * WARNING: The ESP8266 has only one WiFi channel, and the station/client mode is always prioritized for channel selection.
   * This can cause problems if several MeshBackendBase instances exist on the same ESP8266 and use different WiFi channels. 
   * In such a case, whenever the station of one MeshBackendBase instance connects to an AP, it will silently force the 
   * WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly 
   * make it impossible for other stations to detect the APs whose WiFi channels have changed.
   * 
   * @param newWiFiChannel The WiFi channel to change to. Valid values are determined by wifi_get_country, usually integers from 1 to 11 or 1 to 13.
   *                          
   */
  void setWiFiChannel(uint8 newWiFiChannel);
  uint8 getWiFiChannel() const;

  /**
   * Change the SSID used by this MeshBackendBase instance. 
   * The SSID can be at most 31 characters long.
   * Will also change the SSID for the active AP (via an AP restart)
   * if this MeshBackendBase instance is the current AP controller.
   *
   * @param newSSIDPrefix The first part of the new SSID.
   * @param newSSIDRoot The middle part of the new SSID.
   * @param newSSIDSuffix The last part of the new SSID.
   */  
  void setSSID(const String &newSSIDPrefix = emptyString, const String &newSSIDRoot = emptyString, 
               const String &newSSIDSuffix = emptyString);
  String getSSID() const;

  /**
   * Change the first part of the SSID used by this MeshBackendBase instance. 
   * Will also change the first part of the SSID for the active AP (via an AP restart)
   * if this MeshBackendBase instance is the current AP controller.
   *
   * @param newSSIDPrefix The new first part of the SSID.
   */  
  void setSSIDPrefix(const String &newSSIDPrefix);
  String getSSIDPrefix() const;

  /**
   * Change the middle part of the SSID used by this MeshBackendBase instance. 
   * Will also change the middle part of the SSID for the active AP (via an AP restart)
   * if this MeshBackendBase instance is the current AP controller.
   *
   * @param newSSIDPrefix The new middle part of the SSID.
   */
  void setSSIDRoot(const String &newSSIDRoot);
  String getSSIDRoot() const;

  /**
   * Change the last part of the SSID used by this MeshBackendBase instance. 
   * Will also change the last part of the SSID for the active AP (via an AP restart)
   * if this MeshBackendBase instance is the current AP controller.
   *
   * @param newSSIDSuffix The new last part of the SSID.
   */  
  void setSSIDSuffix(const String &newSSIDSuffix);
  String getSSIDSuffix() const;

  /**
   * Change the mesh name used by this MeshBackendBase instance.
   * Will also change the mesh name for the active AP (via an AP restart)
   * if this MeshBackendBase instance is the current AP controller.
   * 
   * Used as alias for setSSIDPrefix by default. Feel free to override this method in a subclass if your mesh name is not equal to SSIDPrefix.
   *
   * @param newMeshName The mesh name to change to.                        
   */
  virtual void setMeshName(const String &newMeshName);
  virtual String getMeshName();
      
  /**
   * Change the node id used by this MeshBackendBase instance. 
   * Will also change the node id for the active AP (via an AP restart)
   * if this MeshBackendBase instance is the current AP controller.
   * 
   * Used as alias for setSSIDSuffix by default. Feel free to override this method in a subclass if your node id is not equal to SSIDSuffix.
   *
   * @param newNodeID The node id to change to.                        
   */
  virtual void setNodeID(const String &newNodeID);
  virtual String getNodeID();

  /**
   * Set the password used when connecting to other AP:s and when other nodes connect to the AP of this node.
   * Will also change the setting for the active AP (via an AP restart)
   * if this MeshBackendBase instance is the current AP controller.
   * 
   * @param newMeshPassword The password to use.
   */
  void setMeshPassword(const String &newMeshPassword);
  String getMeshPassword() const;
  
  /**
   * Set the message that will be sent to other nodes when calling attemptTransmission.
   * 
   * @param newMessage The message to send.
   */
  void setMessage(const String &newMessage);
  String getMessage() const;

  virtual void attemptTransmission(const String &message, bool scan = true, bool scanAllWiFiChannels = false) = 0;

  void setRequestHandler(requestHandlerType requestHandler);
  requestHandlerType getRequestHandler();
  
  void setResponseHandler(responseHandlerType responseHandler);
  responseHandlerType getResponseHandler();
    
  void setNetworkFilter(networkFilterType networkFilter);
  networkFilterType getNetworkFilter();

  /**
   * Set a function that should be called after each update of the latestTransmissionOutcomes vector during attemptTransmission. (which happens after each individual transmission has finished)
   * The hook should return a bool. If this return value is true, attemptTransmission will continue with the next entry in the connectionQueue. If it is false, attemptTransmission will stop.
   * The default transmissionOutcomesUpdateHook always returns true.
   * 
   * Example use cases is modifying getMessage() between transmissions, or aborting attemptTransmission before all nodes in the connectionQueue have been contacted.
   */
  void setTransmissionOutcomesUpdateHook(transmissionOutcomesUpdateHookType transmissionOutcomesUpdateHook);
  transmissionOutcomesUpdateHookType getTransmissionOutcomesUpdateHook();

  /**
   * Set whether scan results from this MeshBackendBase instance will include WiFi networks with hidden SSIDs.
   * This is false by default.
   * The SSID field of a found hidden network will be blank in the scan results.
   * WiFi.isHidden(networkIndex) can be used to verify that a found network is hidden.
   *
   * @param scanHidden If true, WiFi networks with hidden SSIDs will be included in scan results.
   */
  void setScanHidden(bool scanHidden);
  bool getScanHidden() const;

  /**
   * Set whether the AP controlled by this MeshBackendBase instance will have a WiFi network with hidden SSID.
   * This is false by default.
   * Will also change the setting for the active AP (via an AP restart)
   * if this MeshBackendBase instance is the current AP controller.
   *
   * @param apHidden If true, the WiFi network created will have a hidden SSID.
   */
  void setAPHidden(bool apHidden);
  bool getAPHidden() const;

  /**
   * Set whether the normal events occurring in the library will be printed to Serial or not. Off by default.
   * This setting is separate for each mesh instance.
   * 
   * @param enabled If true, library Serial prints are activated.
   */
  virtual void setVerboseModeState(bool enabled);
  virtual bool verboseMode();
  
  /**
   * Only print stringToPrint if verboseMode() returns true.
   * 
   * @param stringToPrint String to print.
   * @param newline If true, will end the print with a newline. True by default.
   */
  virtual void verboseModePrint(const String &stringToPrint, bool newline = true);
  
  /**
   * Set whether the warnings occurring in the library will be printed to Serial or not. On by default.
   * This setting will affect all mesh instances.
   * 
   * @param printEnabled If true, warning Serial prints from the library are activated.
   */
  static void setPrintWarnings(bool printEnabled);
  static bool printWarnings();

  /**
   * Only print stringToPrint if printWarnings() returns true.
   * 
   * @param stringToPrint String to print.
   * @param newline If true, will end the print with a newline. True by default.
   */
  static void warningPrint(const String &stringToPrint, bool newline = true);

  MeshBackendType getClassType();

protected:

  /**
   * @param latestTransmissionOutcomes The transmission outcomes vector to check.
   * 
   * @return True if latest transmission was successful (i.e. latestTransmissionOutcomes is not empty and all entries have transmissionStatus TransmissionStatusType::TRANSMISSION_COMPLETE). False otherwise.
   */
  static bool latestTransmissionSuccessfulBase(const std::vector<TransmissionOutcome> &latestTransmissionOutcomes);

  virtual void scanForNetworks(bool scanAllWiFiChannels);
  virtual void printAPInfo(const NetworkInfoBase &apNetworkInfo);

  /**
   * Called just before we activate the AP.
   * Put _server.stop() in deactivateAPHook() in case you use _server.begin() here.
   */
  virtual void activateAPHook();

  /**
   * Called just before we deactivate the AP.
   * Put _server.stop() here in case you use _server.begin() in activateAPHook().
   */
  virtual void deactivateAPHook();

  void setClassType(MeshBackendType classType);

  static std::shared_ptr<bool> _scanMutex;

private:

  MeshBackendType _classType;

  static MeshBackendBase *apController;

  String _SSID;
  String _SSIDPrefix;
  String _SSIDRoot;
  String _SSIDSuffix;
  String _meshPassword;
  uint8 _meshWiFiChannel;
  bool _verboseMode;
  String _message;
  bool _scanHidden = false;
  bool _apHidden = false;

  requestHandlerType _requestHandler;
  responseHandlerType _responseHandler;
  networkFilterType _networkFilter;
  transmissionOutcomesUpdateHookType _transmissionOutcomesUpdateHook = [](MeshBackendBase &){return true;};

  static bool _printWarnings;
};

#endif
