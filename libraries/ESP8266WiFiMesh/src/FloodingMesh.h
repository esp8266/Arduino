/*
 * Copyright (C) 2019 Anders Löfgren
 *
 * License (MIT license):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __FLOODINGMESH_H__
#define __FLOODINGMESH_H__

#include "EspnowMeshBackend.h"
#include <set>
#include <unordered_map>
#include <queue>

/**
 * An alternative to standard delay(). Will continuously call performMeshMaintenance() during the waiting time, so that the FloodingMesh node remains responsive.
 * Note that if there is a lot of FloodingMesh transmission activity to the node during the floodingMeshDelay, the desired duration may be overshot by several ms. 
 * Thus, if precise timing is required, use standard delay() instead.
 *  
 * Should not be used inside callbacks since performMeshMaintenance() can alter the ESP-NOW state.
 *  
 * @param durationMs The shortest allowed delay duration, in milliseconds.
 */
void floodingMeshDelay(uint32_t durationMs);

class FloodingMesh {

protected:

  typedef std::function<bool(String &, FloodingMesh &)> messageHandlerType;
  typedef std::unordered_map<uint64_t, uint8_t>::iterator messageQueueElementType;
  
public:

  /**
   * FloodingMesh constructor method. Creates a FloodingMesh node, ready to be initialised.
   *
   * @param messageHandler The callback handler responsible for dealing with messages received from the mesh.
   * @param meshPassword The WiFi password for the mesh network.
   * @param espnowEncryptedConnectionKey An uint8_t array containing the secret key used by the EspnowMeshBackend instance for creating encrypted ESP-NOW connections.
   * @param espnowHashKey An uint8_t array containing the secret key used by the EspnowMeshBackend instance to generate HMACs for encrypted ESP-NOW connections.
   * @param ssidPrefix The prefix (first part) of the node SSID.
   * @param ssidSuffix The suffix (last part) of the node SSID.
   * @param verboseMode Determines if we should print the events occurring in the library to Serial. Off by default. This setting is shared by all EspnowMeshBackend instances.
   * @param meshWiFiChannel The WiFi channel used by the mesh network. Valid values are integers from 1 to 13. Defaults to 1.
   *                        WARNING: The ESP8266 has only one WiFi channel, and the the station/client mode is always prioritized for channel selection.
   *                        This can cause problems if several mesh instances exist on the same ESP8266 and use different WiFi channels. 
   *                        In such a case, whenever the station of one mesh instance connects to an AP, it will silently force the 
   *                        WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly 
   *                        make it impossible for other stations to detect the APs whose WiFi channels have changed.
   * 
   */
  FloodingMesh(messageHandlerType messageHandler, const String &meshPassword, const uint8_t espnowEncryptedConnectionKey[EspnowProtocolInterpreter::espnowEncryptedConnectionKeyLength], 
               const uint8_t espnowHashKey[EspnowProtocolInterpreter::espnowHashKeyLength], const String &ssidPrefix, 
               const String &ssidSuffix, bool verboseMode = false, uint8 meshWiFiChannel = 1);

  /**
   * FloodingMesh constructor method. Creates a FloodingMesh node, ready to be initialised.
   *
   * @param messageHandler The callback handler responsible for dealing with messages received from the mesh.
   * @param meshPassword The WiFi password for the mesh network.
   * @param espnowEncryptedConnectionKeySeed A string containing the seed that will generate the secret key used by the EspnowMeshBackend instance for creating encrypted ESP-NOW connections.
   * @param espnowHashKeySeed A string containing the seed that will generate the secret key used by the EspnowMeshBackend to generate HMACs for encrypted ESP-NOW connections.
   * @param ssidPrefix The prefix (first part) of the node SSID.
   * @param ssidSuffix The suffix (last part) of the node SSID.
   * @param verboseMode Determines if we should print the events occurring in the library to Serial. Off by default. This setting is shared by all EspnowMeshBackend instances.
   * @param meshWiFiChannel The WiFi channel used by the mesh network. Valid values are integers from 1 to 13. Defaults to 1.
   *                        WARNING: The ESP8266 has only one WiFi channel, and the the station/client mode is always prioritized for channel selection.
   *                        This can cause problems if several mesh instances exist on the same ESP8266 and use different WiFi channels. 
   *                        In such a case, whenever the station of one mesh instance connects to an AP, it will silently force the 
   *                        WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly 
   *                        make it impossible for other stations to detect the APs whose WiFi channels have changed.
   * 
   */
  FloodingMesh(messageHandlerType messageHandler, const String &meshPassword, const String &espnowEncryptedConnectionKeySeed, const String &espnowHashKeySeed, 
               const String &ssidPrefix, const String &ssidSuffix, bool verboseMode = false, uint8 meshWiFiChannel = 1);

  /**
   * This constructor should be used in combination with serializeMeshState() when the node has gone to sleep while other nodes stayed awake.
   * Otherwise the message ID will be reset after sleep, which means that the nodes that stayed awake may ignore new broadcasts for a while.
   * 
   * @param serializedMeshState A String with a serialized mesh node state that the node should use.
   */
  FloodingMesh(const String &serializedMeshState, messageHandlerType messageHandler, const String &meshPassword, 
               const uint8_t espnowEncryptedConnectionKey[EspnowProtocolInterpreter::espnowEncryptedConnectionKeyLength], 
               const uint8_t espnowHashKey[EspnowProtocolInterpreter::espnowHashKeyLength], const String &ssidPrefix, 
               const String &ssidSuffix, bool verboseMode = false, uint8 meshWiFiChannel = 1);

  /**
   * This constructor should be used in combination with serializeMeshState() when the node has gone to sleep while other nodes stayed awake.
   * Otherwise the message ID will be reset after sleep, which means that the nodes that stayed awake may ignore new broadcasts for a while.
   * 
   * @param serializedMeshState A String with a serialized mesh node state that the node should use.
   */
  FloodingMesh(const String &serializedMeshState, messageHandlerType messageHandler, const String &meshPassword, const String &espnowEncryptedConnectionKeySeed, 
               const String &espnowHashKeySeed, const String &ssidPrefix, const String &ssidSuffix, bool verboseMode = false, uint8 meshWiFiChannel = 1);

  virtual ~FloodingMesh();

  /**
   * The method responsible for initialising this FloodingMesh instance.
   */
  void begin();

  /** 
   * Makes it possible to find the node through scans, and also makes it possible to recover from an encrypted ESP-NOW connection where only the other node is encrypted.
   * Required for encryptedBroadcast() usage, but also slows down the start-up of the node.
   * 
   * Note that only one AP can be active at a time in total (there is only one WiFi radio on the ESP8266), and this will always be the one which was last activated.
   * Thus the AP is shared by all backends.
   * All FloodingMesh instances can still broadcast messages though, even if their AP is not visible.
   */
  void activateAP();
  
  /**
   * Performs maintenance for all available Flooding Mesh instances
   */
  static void performMeshMaintenance();
  
  /** 
   * Performs maintenance for this particular Flooding Mesh instance
   */
  void performMeshInstanceMaintenance();

  /**
   * Serialize the current mesh node state. Useful to save a state before the node goes to sleep.
   * Note that this saves the current state only, so if a broadcast is made after this, the stored state is invalid.
   * 
   * @return A string with the serialized current mesh node state.
   */
  String serializeMeshState();

  /**
   * Make an unencrypted broadcast to the entire mesh network.
   * 
   * It is recommended that there is at most one new message transmitted in the mesh every 10, 20, 30 ms for messages up to length maxUnencryptedMessageLength()*n, 
   * where n is (roughly, depending on mesh name length) 1/4, 3/5 and 1 respectively. If transmissions are more frequent than this, message loss will increase.
   * 
   * @param message The message to broadcast. Maximum message length is given by maxUnencryptedMessageLength(). The longer the message, the longer the transmission time. 
   */
  void broadcast(const String &message);

  /**
   * Set the maximum number of redundant copies that will be received of every broadcast. (from different senders)
   * A greater number increases the likelihood that at least one of the copies is received successfully, but will also use more RAM.
   * 
   * @param redundancy The maximum number of extra copies that will be accepted. Defaults to 2. Valid values are 0 to 254.
   */
  void setBroadcastReceptionRedundancy(uint8_t redundancy);
  uint8_t getBroadcastReceptionRedundancy();
  
  /**
   * Make an encrypted broadcast to the entire mesh network.
   * 
   * ########## WARNING! This an experimental feature. API may change at any time. Only use if you like it when things break. ##########
   * Will be very slow compared to unencrypted broadcasts. Probably works OK in a small mesh with a maximum of 2-3 new messages transmitted in the mesh every second.
   * Because of the throughput difference, mixing encypted and unencrypted broadcasts is not recommended if there are frequent mesh broadcasts (multiple per second), 
   * since a lot of unencrypted broadcasts can build up while a single encrypted broadcast is sent.
   * 
   * It is recommended that verboseMode is turned off if using this, to avoid slowdowns due to excessive Serial printing.
   * 
   * @param message The message to broadcast. Maximum message length is given by maxEncryptedMessageLength(). The longer the message, the longer the transmission time. 
   */
  void encryptedBroadcast(const String &message);

  /**
   * Clear the logs used for remembering which messages this node has received from the mesh network.
   */
  void clearMessageLogs();
  
  /**
   * Remove all messages received from the mesh network which are stored waiting to be forwarded by this node.
   */
  void clearForwardingBacklog();

  /**
   * Set the callback handler responsible for dealing with messages received from the mesh.
   * 
   * @param messageHandler The message handler callback function to use.
   */
  void setMessageHandler(messageHandlerType messageHandler);
  messageHandlerType getMessageHandler();

  /**
   * Get the origin AP MAC address of the most recently received mesh message.
   * Returns a String.
   * 
   * @return A String filled with a hexadecimal representation of the MAC, without delimiters.
   */
  String getOriginMac();

  /**
   * Get the origin AP MAC address of the most recently received mesh message.
   * Returns a uint8_t array.
   * 
   * @param macArray The array that should store the MAC address. Must be at least 6 bytes.
   * @return macArray filled with the origin MAC.
   */
  uint8_t *getOriginMac(uint8_t *macArray);

 /**
 * The number of received messageID:s that will be stored by the node. Used to remember which messages have been received. 
 * Setting this too low will cause the same message to be received many times.
 * Setting this too high will cause the node to run out of RAM.
 * In practice, setting this value to more than 1337 is probably a bad idea since the node will run out of RAM quickly and crash as a result.
 * 
 * Defaults to 100.
 * 
 * @param messageLogSize The size of the message log for this FloodingMesh instance. Valid values are 1 to 65535 (uint16_t_max).
 *                       If a value close to the maximum is chosen, there is a high risk the node will ignore transmissions on messageID rollover if they are sent only by one node 
 *                       (especially if some transmissions are missed), since the messageID also uses uint16_t.
 */
  void setMessageLogSize(uint16_t messageLogSize);
  uint16_t messageLogSize();

  /**
   * Hint: Use String.length() to get the ASCII length of a String.
   * 
   * @return The maximum length in bytes an unencrypted ASCII message is allowed to be when broadcasted by this node. 
   *         Note that non-ASCII characters usually require at least two bytes each.
   *         Also note that for unencrypted messages the maximum size will depend on getEspnowMeshBackend().getMeshName().length()
   */
  uint32_t maxUnencryptedMessageLength();
  
  /**
   * Hint: Use String.length() to get the ASCII length of a String.
   * 
   * @return The maximum length in bytes an encrypted ASCII message is allowed to be when broadcasted by this node. 
   *         Note that non-ASCII characters usually require at least two bytes each.
   */
  uint32_t maxEncryptedMessageLength();

  /**
   * Set the delimiter character used for metadata by every FloodingMesh instance. 
   * Using characters found in the mesh name or in HEX numbers is unwise, as is using ','.
   * 
   * @param metadataDelimiter The metadata delimiter character to use.
   *                          Defaults to 23 = End-of-Transmission-Block (ETB) control character in ASCII
   */
  static void setMetadataDelimiter(char metadataDelimiter);
  static char metadataDelimiter();

  /*
   * Gives you access to the EspnowMeshBackend used by the mesh node.
   * The backend handles all mesh communication, and modifying it allows you to change every aspect of the mesh behaviour.
   * Random interactions with the backend have a high chance of breaking the mesh network,
   * and so are discouraged for those who prefer it when things just work.
   */
  EspnowMeshBackend &getEspnowMeshBackend();

  void restoreDefaultRequestHandler();
  void restoreDefaultResponseHandler();
  void restoreDefaultNetworkFilter();
  void restoreDefaultBroadcastFilter();
  void restoreDefaultTransmissionOutcomesUpdateHook();
  void restoreDefaultResponseTransmittedHook();

protected:

  static std::set<FloodingMesh *> availableFloodingMeshes;
  
  String generateMessageID();

  void broadcastKernel(const String &message);

  void encryptedBroadcastKernel(const String &message);

  bool insertPreliminaryMessageID(uint64_t messageID);
  bool insertCompletedMessageID(uint64_t messageID);
  void updateMessageQueue(messageQueueElementType messageIterator);
  
  void loadMeshState(const String &serializedMeshState);

  /**
   * Set the MAC address considered to be the origin AP MAC address of the most recently received mesh message.
   * 
   * @param macArray An uint8_t array which contains the MAC address to store. The method will store the first 6 bytes of the array.
   */
  void setOriginMac(uint8_t *macArray);
  
private:

  static const uint8_t MESSAGE_ID_LENGTH = 17; // 16 characters and one delimiter
  static const uint8_t MESSAGE_COMPLETE = 255;

  EspnowMeshBackend _espnowBackend;

  messageHandlerType _messageHandler;

  uint16_t _messageCount = 0;
  uint16_t _messageLogSize = 100;

  uint8_t _broadcastReceptionRedundancy = 2;

  static char _metadataDelimiter; // Defaults to 23 = End-of-Transmission-Block (ETB) control character in ASCII

  uint8_t _originMac[6] = {0};

  std::unordered_map<uint64_t, uint8_t> _messageIDs = {};
  std::queue<messageQueueElementType> _messageIdOrder = {};
  std::list<std::pair<String, bool>> _forwardingBacklog = {};

  String _macIgnoreList = "";
  
  String _defaultRequestHandler(const String &request, MeshBackendBase &meshInstance);
  transmission_status_t _defaultResponseHandler(const String &response, MeshBackendBase &meshInstance);
  void _defaultNetworkFilter(int numberOfNetworks, MeshBackendBase &meshInstance);
  bool _defaultBroadcastFilter(String &firstTransmission, EspnowMeshBackend &meshInstance);
  bool _defaultTransmissionOutcomesUpdateHook(MeshBackendBase &meshInstance);
  bool _defaultResponseTransmittedHook(const String &response, const uint8_t *recipientMac, uint32_t responseIndex, EspnowMeshBackend &meshInstance);
};

#endif
