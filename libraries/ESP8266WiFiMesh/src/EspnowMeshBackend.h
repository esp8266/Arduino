/*
  EspnowMeshBackend
 
  Copyright (C) 2019 Anders Löfgren
 
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

// ESP-NOW is faster for small data payloads (up to a few kB, split over multiple messages). Transfer of up to 234 bytes takes 4 ms.
// In general ESP-NOW transfer time can be approximated with the following function: transferTime = ceil(bytesToTransfer / 234.0)*3 ms.
// If you only transfer 234 bytes at a time, this adds up to around 56kB/s. Finally a chance to relive the glory of the olden days
// when people were restricted to V90 dial-up modems for internet access!
// TCP-IP takes longer to connect (around 1000 ms), and an AP has to disconnect all connected stations in order to transfer data to another AP, 
// but this backend has a much higher data transfer speed than ESP-NOW once connected (100x faster or so).

/**
 * This ESP-NOW framework uses a few different message types to enable easier interpretation of transmissions. 
 * The message type is stored in the first transmission byte, see EspnowProtocolInterpreter.h for more detailed information on the protocol.
 * Available message types are 'Q' for question (request), 'A' for answer (response), 
 * 'B' for broadcast, 'S' for synchronization request, 'P' for peer request and 'C' for peer request confirmation.
 * 
 * 'B', 'Q' and 'A' are the message types that are assigned to data transmitted by the user. 
 * 'S', 'P' and 'C' are used only for internal framework transmissions.
 * 
 * Messages with type 'B' are only used for broadcasts. They cannot be encrypted.
 * 
 * Messages with type 'Q' are used for requests sent by the user. They can be encrypted.
 * 
 * Messages with type 'A' are used for responses given by the user when 'B' or 'Q' messages have been received. They can be encrypted.
 *
 * Messages with type 'P' and 'C' are used exclusively for automatically pairing two ESP-NOW nodes to each other.
 * This enables flexible easy-to-use encrypted ESP-NOW communication. 'P' and 'C' messages can be encrypted. 
 * The encryption pairing process works as follows (from top to bottom):
 * 
 * Encrypted connection pairing process, schematic overview: 
 * 
 *     Connection   |         Peer sends ('C'):           |    Peer requester sends ('P'):   |    Connection
 *     encrypted:   |                                     |                                  |    encrypted:
 *                  |                                     |    Peer request + Nonce + HMAC   |
 *                  |       StaMac + Nonce + HMAC         |                                  |
 *                  |                                     |              Ack                 |
 *          X       |   SessionKeys + Nonce + Password    |                                  |        X
 *          X       |                                     |              Ack                 |        X
 *          X       |                                     |           SessionKey             |        X
 *          X       |               Ack                   |                                  |        X
 *                  |                                     |                                  |
 *  
 *  
 * The ESP-NOW CCMP encryption should have replay attack protection built in, 
 * but since there is no official documentation from Espressif about this a 128 bit random nonce is included in encrypted connection requests.
 * 
 * Messages with type 'S' are used exclusively when we try to send an encrypted 'R' or 'P' transmission and the last such transmission we tried failed to receive an ack. 
 * Since we then do not know if the receiving node has incremented its corresponding session key or not, we first send an 'S' request to make sure the key is incremented.
 * Once we get an ack for our 'S' request we send the new encrypted 'R' or 'P' transmission. 'S' messages are always encrypted.
 * 
 * Messages of type 'A' and 'C' are response types, and thus use the same session key as the corresponding 'R' and 'P' message they are responding to.
 * This means they can never cause a desynchronization to occur, and therefore they do not trigger 'S' messages.
 * 
 * In addition to using encrypted ESP-NOW connections the framework can also send automatically encrypted messages (AEAD) over both encrypted and unencrypted connections. 
 * Using AEAD will only encrypt the message content, not the transmission metadata. 
 * The AEAD encryption does not require any pairing, and is thus faster for single messages than establishing a new encrypted connection before transfer.
 * AEAD encryption also works with ESP-NOW broadcasts and supports an unlimited number of nodes, which is not true for encrypted connections.
 * Encrypted ESP-NOW connections do however come with built in replay attack protection, which is not provided by the framework when using AEAD encryption, 
 * and allow EspnowProtocolInterpreter::aeadMetadataSize extra message bytes per transmission.
 * Transmissions via encrypted connections are also slightly faster than via AEAD once a connection has been established.
 */

#ifndef __ESPNOWMESHBACKEND_H__
#define __ESPNOWMESHBACKEND_H__

#include "EspnowDatabase.h"
#include "EspnowConnectionManager.h"
#include "EspnowTransmitter.h"
#include "EspnowEncryptionBroker.h"
#include "MeshBackendBase.h"
#include "EspnowProtocolInterpreter.h"
#include "EncryptedConnectionLog.h"
#include "PeerRequestLog.h"
#include "RequestData.h"
#include "ResponseData.h"
#include "MessageData.h"
#include <map>
#include <list>
#include "EspnowNetworkInfo.h"

/**
 * An alternative to standard delay(). Will continuously call performEspnowMaintenance() during the waiting time, so that the ESP-NOW node remains responsive.
 * Note that if there is a lot of ESP-NOW transmission activity to the node during the espnowDelay, the desired duration may be overshot by several ms. 
 * Thus, if precise timing is required, use standard delay() instead.
 *  
 * Should not be used inside responseHandler, requestHandler, networkFilter or broadcastFilter callbacks since performEspnowMaintenance() can alter the ESP-NOW state.
 *  
 * @param durationMs The shortest allowed delay duration, in milliseconds.
 */
void espnowDelay(const uint32_t durationMs);

class EspnowMeshBackend : public MeshBackendBase {

public: 

  using broadcastFilterType = std::function<bool(String &, EspnowMeshBackend &)>;
  
  /**
   * ESP-NOW constructor method. Creates an ESP-NOW node, ready to be initialised.
   *
   * @param requestHandler The callback handler for dealing with received requests. Takes a string as an argument which
   *          is the request string received from another node and returns the string to send back.
   * @param responseHandler The callback handler for dealing with received responses. Takes a string as an argument which
   *          is the response string received from another node. Returns a transmission status code as a TransmissionStatusType.
   * @param networkFilter The callback handler for deciding which WiFi networks to connect to.
   * @param broadcastFilter The callback handler for deciding which ESP-NOW broadcasts to accept.
   * @param meshPassword The WiFi password for the mesh network.
   * @param espnowEncryptedConnectionKey An uint8_t array containing the secret key used by this EspnowMeshBackend instance for creating encrypted ESP-NOW connections.
   * @param espnowHashKey An uint8_t array containing the secret key used by this EspnowMeshBackend to generate HMACs for encrypted ESP-NOW connections.
   * @param ssidPrefix The prefix (first part) of the node SSID.
   * @param ssidSuffix The suffix (last part) of the node SSID.
   * @param verboseMode Determines if we should print the events occurring in the library to Serial. Off by default. This setting is shared by all EspnowMeshBackend instances.
   * @param meshWiFiChannel The WiFi channel used by the mesh network. Valid values are integers from 1 to 13. Defaults to 1.
   *                    WARNING: The ESP8266 has only one WiFi channel, and the station/client mode is always prioritized for channel selection.
   *                    This can cause problems if several mesh instances exist on the same ESP8266 and use different WiFi channels. 
   *                    In such a case, whenever the station of one mesh instance connects to an AP, it will silently force the 
   *                    WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly 
   *                    make it impossible for other stations to detect the APs whose WiFi channels have changed.
   * 
   */
  EspnowMeshBackend(const requestHandlerType requestHandler, const responseHandlerType responseHandler, const networkFilterType networkFilter, const broadcastFilterType broadcastFilter,
                    const String &meshPassword, const uint8_t espnowEncryptedConnectionKey[EspnowProtocolInterpreter::encryptedConnectionKeyLength], 
                    const uint8_t espnowHashKey[EspnowProtocolInterpreter::hashKeyLength], const String &ssidPrefix, 
                    const String &ssidSuffix, const bool verboseMode = false, const uint8 meshWiFiChannel = 1);

  /**
   * ESP-NOW constructor method. Creates an ESP-NOW node, ready to be initialised.
   *
   * @param requestHandler The callback handler for dealing with received requests. Takes a string as an argument which
   *          is the request string received from another node and returns the string to send back.
   * @param responseHandler The callback handler for dealing with received responses. Takes a string as an argument which
   *          is the response string received from another node. Returns a transmission status code as a TransmissionStatusType.
   * @param networkFilter The callback handler for deciding which WiFi networks to connect to.
   * @param broadcastFilter The callback handler for deciding which ESP-NOW broadcasts to accept.
   * @param meshPassword The WiFi password for the mesh network.
   * @param espnowEncryptedConnectionKeySeed A string containing the seed that will generate the secret key used by this EspnowMeshBackend instance for creating encrypted ESP-NOW connections.
   * @param espnowHashKeySeed A string containing the seed that will generate the secret key used by this EspnowMeshBackend to generate HMACs for encrypted ESP-NOW connections.
   * @param ssidPrefix The prefix (first part) of the node SSID.
   * @param ssidSuffix The suffix (last part) of the node SSID.
   * @param verboseMode Determines if we should print the events occurring in the library to Serial. Off by default. This setting is shared by all EspnowMeshBackend instances.
   * @param meshWiFiChannel The WiFi channel used by the mesh network. Valid values are integers from 1 to 13. Defaults to 1.
   *                    WARNING: The ESP8266 has only one WiFi channel, and the station/client mode is always prioritized for channel selection.
   *                    This can cause problems if several mesh instances exist on the same ESP8266 and use different WiFi channels. 
   *                    In such a case, whenever the station of one mesh instance connects to an AP, it will silently force the 
   *                    WiFi channel of any active AP on the ESP8266 to match that of the station. This will cause disconnects and possibly 
   *                    make it impossible for other stations to detect the APs whose WiFi channels have changed.
   * 
   */
  EspnowMeshBackend(const requestHandlerType requestHandler, const responseHandlerType responseHandler, const networkFilterType networkFilter, const broadcastFilterType broadcastFilter,
                    const String &meshPassword, const String &espnowEncryptedConnectionKeySeed, const String &espnowHashKeySeed, const String &ssidPrefix, 
                    const String &ssidSuffix, const bool verboseMode = false, const uint8 meshWiFiChannel = 1);

  ~EspnowMeshBackend() override;

  /** 
  * Returns a vector that contains the NetworkInfo for each WiFi network to connect to.
  * This vector is unique for each mesh backend, but NetworkInfo elements can be directly transferred between the vectors as long as both SSID and BSSID are present.
  * The connectionQueue vector is cleared before each new scan and filled via the networkFilter callback function once the scan completes.
  * WiFi connections will start with connectionQueue[0] and then incrementally proceed to higher vector positions. 
  * Note that old network indicies often are invalidated whenever a new WiFi network scan occurs.
  * 
  * Since the connectionQueue() is iterated over during transmissions, always use constConnectionQueue() from callbacks other than NetworkFilter.
  */
  static std::vector<EspnowNetworkInfo> & connectionQueue();

  /**
   * Same as connectionQueue(), but can be called from all callbacks since the returned reference is const.
   */
  static const std::vector<EspnowNetworkInfo> & constConnectionQueue();
  
  /** 
  * Returns a vector with the TransmissionOutcome for each AP to which a transmission was attempted during the latest attemptTransmission call.
  * This vector is unique for each mesh backend.
  * The latestTransmissionOutcomes vector is cleared before each new transmission attempt.
  * Connection attempts are indexed in the same order they were attempted.
  * Note that old network indicies often are invalidated whenever a new WiFi network scan occurs.
  */
  static std::vector<TransmissionOutcome> & latestTransmissionOutcomes();

  /**
   * @return True if latest transmission was successful (i.e. latestTransmissionOutcomes is not empty and all entries have transmissionStatus TransmissionStatusType::TRANSMISSION_COMPLETE). False otherwise.
   *         The result is unique for each mesh backend.
   */
  static bool latestTransmissionSuccessful();

  /**
   * Initialises the node.
   */
  void begin() override;

  /**   
   * This method performs all the background operations for the EspnowMeshBackend. 
   * It is recommended to place it in the beginning of the loop(), unless there is a need to put it elsewhere.
   * Among other things, the method cleans up old Espnow log entries (freeing up RAM) and sends the responses you provide to Espnow requests.
   * Note that depending on the amount of responses to send and their length, this method can take tens or even hundreds of milliseconds to complete.
   * More intense transmission activity and less frequent calls to performEspnowMaintenance will likely cause the method to take longer to complete, so plan accordingly.
   * 
   * Should not be used inside responseHandler, requestHandler, networkFilter or broadcastFilter callbacks since performEspnowMaintenance() can alter the ESP-NOW state.
   * 
   * @param estimatedMaxDuration The desired max duration for the method. If set to 0 there is no duration limit. 
   *                             Note that setting the estimatedMaxDuration too low may result in missed ESP-NOW transmissions because of too little time for maintenance.
   *                             Also note that although the method will try to respect the max duration limit, there is no guarantee. Overshoots by tens of milliseconds are possible.
   */
  static void performEspnowMaintenance(const uint32_t estimatedMaxDuration = 0);

  /**
   * At critical heap level no more incoming requests are accepted.
   */
  static uint32_t criticalHeapLevel();

  /**
   * At critical heap level no more incoming requests are accepted.
   * This method sets the maximum number of bytes above the critical heap level that will trigger an early ESP-NOW log clearing in an attempt to increase available heap size.
   * A too high value may cause very frequent early log clearings, which will slow things down. Especially if you are using a lot of heap in other parts of your program.
   * A too low value may cause some incoming requests to be lost and/or an increase in heap fragmentation,
   * especially if you quickly fill the heap by receiving a lot of large ESP-NOW messages or sending a lot of large ESP-NOW responses.
   * The buffer is set to 6000 bytes by default, which should be enough to prevent lost incoming requests while giving plenty of heap to fill up before early clearing in most circumstances.
   * 
   * The buffer can be set lower than the default if you are running low on heap, since it may otherwise be hard to get responses sent. 
   * However, lower values tend to result in more heap fragmentation during intense transmission activity. 
   * Depending on your situation (message size, transmission frequency), values below 2000-3000 bytes will also start to cause lost incoming requests due to heap shortage.
   * 
   * If the buffer is set to 0 bytes a significant number of incoming requests are likely to be lost during intense transmission activity, 
   * and there is a greater risk of heap space completely running out before log clearing occurs (which may cause crashes or empty transmissions).
   */
  static void setCriticalHeapLevelBuffer(const uint32_t bufferInBytes);
  static uint32_t criticalHeapLevelBuffer();
  
  /**
   * Deactivates Espnow for this node. Call begin() on a EspnowMeshBackend instance to reactivate Espnow.
   * 
   * @return True if deactivation was successful. False otherwise.
   */
  static bool deactivateEspnow();
  
  void attemptTransmission(const String &message, const bool scan = true, const bool scanAllWiFiChannels = false) override;

  /**
   * Transmit message to a single recipient without changing the local transmission state. 
   * Will not change connectionQueue, latestTransmissionOutcomes or stored message.
   * 
   * @param recipientInfo The recipient information.
   */
  TransmissionStatusType attemptTransmission(const String &message, const EspnowNetworkInfo &recipientInfo);
  
  /* 
   * Will ensure that an encrypted connection exists to each target node before sending the message, 
   * establishing a temporary encrypted connection with duration getAutoEncryptionDuration() first if neccessary.
   * If an encrypted connection cannot be established to a target node, no message will be sent to that node.
   * Note that if an encrypted connection to a target node is not present before this method is called, the response from said node will likely not be received
   * since it will be encrypted and the auto encrypted connection to the node is immediately removed after transmission (unless the requestPermanentConnections argument is set to true).
   * Also note that if a temporary encrypted connection already exists to a target node, this method will slightly extend the connection duration 
   * depending on the time it takes to verify the connection to the node. This can substantially increase the connection duration if many auto encrypting 
   * transmissions occurs.
   * 
   * @param message The message to send to other nodes. It will be stored in the class instance until replaced via attemptTransmission or setMessage.
   * @param requestPermanentConnections If true, the method will request that encrypted connections used for this transmission become permanent so they are not removed once the transmission is complete.
   *                                   This means that encrypted responses to the transmission are received, as long as the encrypted connection is not removed by other means.
   *                                   The receiving node has no obligation to obey the request, although it normally will.
   *                                   If encryptedConnectionsSoftLimit() is set to less than 6 for the transmission receiver,
   *                                   it is possible that a short lived autoEncryptionConnection is created instead of a permanent encrypted connection.
   *                                   Note that a maximum of 6 encrypted ESP-NOW connections can be maintained at the same time by the node.
   *                                   Defaults to false.
   * @param scan Scan for new networks and call the networkFilter function with the scan results. When set to false, only the data already in connectionQueue will be used for the transmission.
   * @param scanAllWiFiChannels Scan all WiFi channels during a WiFi scan, instead of just the channel the MeshBackendBase instance is using.
   *                            Scanning all WiFi channels takes about 2100 ms, compared to just 60 ms if only channel 1 (standard) is scanned.
   *                            Note that if the ESP8266 has an active AP, that AP will switch WiFi channel to match that of any other AP the ESP8266 connects to.
   *                            This can make it impossible for other nodes to detect the AP if they are scanning the wrong WiFi channel.
   */
  void attemptAutoEncryptingTransmission(const String &message, const bool requestPermanentConnections = false, const bool scan = true, const bool scanAllWiFiChannels = false);

  /**
   * Transmit message to a single recipient without changing the local transmission state (apart from encrypted connections). 
   * Will not change connectionQueue, latestTransmissionOutcomes or stored message.
   */
  TransmissionStatusType attemptAutoEncryptingTransmission(const String &message, const EspnowNetworkInfo &recipientInfo, const bool requestPermanentConnection = false);

  /**
   * Send a message simultaneously to all nearby nodes which have ESP-NOW activated.
   * A broadcast is always treated as a request by the receiving node. 
   * There is no limit to the number of responses a node can get when sending a broadcast, it will always accept new responses until the broadcastResponseTimeout is reached.
   * This also means that the broadcaster can receive duplicate responses from the same node if transmission conditions are poor and an ack is lost.
   * A broadcast can never be sent encrypted.
   * 
   * Note that the node needs to have its AP active to be able to receive broadcasts. Nodes can send broadcasts even if their AP is off.
   * 
   * @param message The message to send to the other nodes. Unlike the attemptTransmission method, the message will not be stored in the class instance, since there is no certain way to change the message during an ongoing broadcast.
   */
  void broadcast(const String &message);

  /**
   * Set the number of redundant transmissions that will be made for every broadcast. 
   * A greater number increases the likelihood that the broadcast is received, but also means it takes longer time to send.
   * 
   * @param redundancy The number of extra transmissions to make of each broadcast. Defaults to 1.
   */
  void setBroadcastTransmissionRedundancy(const uint8_t redundancy);
  uint8_t getBroadcastTransmissionRedundancy() const;
  
  /**
   * Set the EspnowMeshBackend instance responsible for handling incoming requests. The requestHandler of the instance will be called upon receiving ESP-NOW requests.
   * 
   * Set to nullptr to stop processing the ESP-NOW requests received by this node (requests will be ignored, but still received (ack will be sent)).
   * The node can still send ESP-NOW transmissions to other nodes, even when the espnowRequestManager is nullptr.
   */
  static void setEspnowRequestManager(EspnowMeshBackend *espnowMeshInstance);
  
  static EspnowMeshBackend *getEspnowRequestManager();

  /**
   * Check if this EspnowMeshBackend instance is the espnowRequestManager.
   * 
   * @return True if this EspnowMeshBackend is the espnowRequestManager. False otherwise.
   */
  bool isEspnowRequestManager() const;

  /**
   * Set the duration of most ESP-NOW log entries. Used for all ESP-NOW communication except for broadcasts and encrypted connection requests.
   * Setting the duration too long may cause the node to run out of RAM, especially if there is intense transmission activity.
   * Setting the duration too short may cause ESP-NOW transmissions to stop working, or make the node receive the same transmission multiple times.
   * 
   * Set to 2500 ms by default.
   * 
   * @param logEntryLifetimeMs The duration to use for most ESP-NOW log entries, in milliseconds.
   */
  static void setLogEntryLifetimeMs(const uint32_t logEntryLifetimeMs);
  static uint32_t logEntryLifetimeMs();

  /**
   * Set the duration during which sent ESP-NOW broadcast are stored in the log and can receive responses.
   * This is shorter by default than logEntryLifetimeMs() in order to preserve RAM since broadcasts are always kept in the log until they expire,
   * whereas normal transmissions are only kept till they receive a response.
   * Setting the duration too long may cause the node to run out of RAM, especially if there is intense broadcast activity.
   * Setting the duration too short may cause ESP-NOW broadcasts to stop working, or make the node never receive responses to broadcasts.
   * 
   * Set to 1000 ms by default.
   * 
   * @param broadcastResponseTimeoutMs The duration sent ESP-NOW broadcasts will be stored in the log, in milliseconds.
   */
  static void setBroadcastResponseTimeoutMs(const uint32_t broadcastResponseTimeoutMs);
  static uint32_t broadcastResponseTimeoutMs();

  /** 
   * Change the key used by this EspnowMeshBackend instance for creating encrypted ESP-NOW connections.
   * Will apply to any new received requests for encrypted connection if this EspnowMeshBackend instance is the current request manager. 
   * Will apply to any new encrypted connections requested or added by this EspnowMeshBackend instance.
   * 
   * NOTE: Encrypted connections added before the encryption key change will retain their old encryption key. 
   * Only changes the encryption key used by this EspnowMeshBackend instance, so each instance can use a separate key.
   * Both Kok and encrypted connection key must match in an encrypted connection pair for encrypted communication to be possible. 
   * Otherwise the transmissions will never reach the recipient, even though acks are received by the sender.
   * 
   * @param espnowEncryptedConnectionKey An array containing the encryptedConnectionKeyLength bytes that will be used as the encryption key.
   */
  void setEspnowEncryptedConnectionKey(const uint8_t espnowEncryptedConnectionKey[EspnowProtocolInterpreter::encryptedConnectionKeyLength]);

  /** 
   * Change the key used by this EspnowMeshBackend instance for creating encrypted ESP-NOW connections.
   * Will apply to any new received requests for encrypted connection if this EspnowMeshBackend instance is the current request manager. 
   * Will apply to any new encrypted connections requested or added by this EspnowMeshBackend instance.
   * 
   * NOTE: Encrypted connections added before the encryption key change will retain their old encryption key. 
   * Only changes the encryption key used by this EspnowMeshBackend instance, so each instance can use a separate key.
   * Both Kok and encrypted connection key must match in an encrypted connection pair for encrypted communication to be possible. 
   * Otherwise the transmissions will never reach the recipient, even though acks are received by the sender.
   * 
   * @param espnowHashKeySeed A string that will be used to generate the encryption key. The same string will always generate the same key. 
   *                          A minimum of 8 random characters are recommended to ensure sufficient key variation.
   */
  void setEspnowEncryptedConnectionKey(const String &espnowEncryptedConnectionKeySeed);

  /**
   * Get the encryption key used by this EspnowMeshBackend instance for creating encrypted ESP-NOW connections.
   * 
   * @return The current espnowEncryptedConnectionKey for this EspnowMeshBackend instance.
   */
  const uint8_t *getEspnowEncryptedConnectionKey() const;
  uint8_t *getEspnowEncryptedConnectionKey(uint8_t resultArray[EspnowProtocolInterpreter::encryptedConnectionKeyLength]) const; 

  /** 
   * Change the key used to encrypt/decrypt the encrypted connection key when creating encrypted ESP-NOW connections. (Kok = key of keys, perhaps) If no Kok is provided by the user, a default Kok is used.
   * Will apply to any new encrypted connections.
   * Must be called after begin() to take effect.
   * 
   * NOTE: Encrypted connections added before the Kok change will retain their old Kok. 
   * This changes the Kok for all EspnowMeshBackend instances on this ESP8266.
   * Both Kok and encrypted connection key must match in an encrypted connection pair for encrypted communication to be possible. 
   * Otherwise the transmissions will never reach the recipient, even though acks are received by the sender.
   * 
   * @param espnowEncryptionKok An array containing the encryptedConnectionKeyLength bytes that will be used as the Kok.
   * @return True if Kok was changed successfully. False if Kok was not changed.
   */
  static bool setEspnowEncryptionKok(uint8_t espnowEncryptionKok[EspnowProtocolInterpreter::encryptedConnectionKeyLength]);

  /** 
   * Change the key used to encrypt/decrypt the encryption key when creating encrypted ESP-NOW connections. (Kok = key of keys, perhaps) If no Kok is provided by the user, a default Kok is used.
   * Will apply to any new encrypted connections.
   * Must be called after begin() to take effect.
   * 
   * NOTE: Encrypted connections added before the Kok change will retain their old Kok. 
   * This changes the Kok for all EspnowMeshBackend instances on this ESP8266.
   * Both Kok and encrypted connection key must match in an encrypted connection pair for encrypted communication to be possible. 
   * Otherwise the transmissions will never reach the recipient, even though acks are received by the sender.
   * 
   * @param espnowEncryptionKokSeed A string that will be used to generate the KoK. The same string will always generate the same KoK. 
   *                                A minimum of 8 random characters are recommended to ensure sufficient KoK variation.
   * @return True if Kok was changed successfully. False if Kok was not changed.
   */
  static bool setEspnowEncryptionKok(const String &espnowEncryptionKokSeed);
  
  /**
   * Get the key used to encrypt the encryption keys when creating encrypted ESP-NOW connections. (Kok = key of keys, perhaps) Returns nullptr if no Kok has been provided by the user.
   * 
   * @return nullptr if default Kok is used, or current espnowEncryptionKok if a custom Kok has been set via the setEspnowEncryptionKok method.
   */
  static const uint8_t *getEspnowEncryptionKok();

 /** 
  * Change the secret key used to generate HMACs for encrypted ESP-NOW connections.
  * Will apply to any new received requests for encrypted connection if this EspnowMeshBackend instance is the current request manager. 
  * Will apply to any new encrypted connections requested or added by this EspnowMeshBackend instance.
  * 
  * NOTE: Encrypted connections added before the key change will retain their old key.
  * Only changes the secret hash key used by this EspnowMeshBackend instance, so each instance can use a separate secret key.
  * 
  * @param espnowHashKey An array containing the hashKeyLength bytes that will be used as the HMAC key.
  */
  void setEspnowHashKey(const uint8_t espnowHashKey[EspnowProtocolInterpreter::hashKeyLength]);
  
 /** 
  * Change the secret key used to generate HMACs for encrypted ESP-NOW connections.
  * Will apply to any new received requests for encrypted connection if this EspnowMeshBackend instance is the current request manager. 
  * Will apply to any new encrypted connections requested or added by this EspnowMeshBackend instance.
  * 
  * NOTE: Encrypted connections added before the key change will retain their old key.
  * Only changes the secret hash key used by this EspnowMeshBackend instance, so each instance can use a separate secret key.
  * 
  * @param espnowHashKeySeed A string that will be used to generate the HMAC key. The same string will always generate the same key. 
  *                          A minimum of 8 random characters are recommended to ensure sufficient key variation.
  */
  void setEspnowHashKey(const String &espnowHashKeySeed);
  
  const uint8_t *getEspnowHashKey() const;

  /**
   * If true, AEAD will be used to encrypt/decrypt all messages sent/received by this node via ESP-NOW, regardless of whether the connection is encrypted or not.
   * All nodes this node wishes to communicate with must then also use encrypted messages with the same getEspnowMessageEncryptionKey(), or messages will not be accepted.
   * Note that using encrypted messages will reduce the number of message bytes that can be transmitted.
   *
   * Using AEAD will only encrypt the message content, not the transmission metadata. 
   * The AEAD encryption does not require any pairing, and is thus faster for single messages than establishing a new encrypted connection before transfer.
   * AEAD encryption also works with ESP-NOW broadcasts and supports an unlimited number of nodes, which is not true for encrypted connections.
   * Encrypted ESP-NOW connections do however come with built in replay attack protection, which is not provided by the framework when using AEAD encryption, 
   * and allow EspnowProtocolInterpreter::aeadMetadataSize extra message bytes per transmission.
   * Transmissions via encrypted connections are also slightly faster than via AEAD once a connection has been established.
   * 
   * useEncryptedMessages() is false by default.
   * 
   * @param useEncryptedMessages If true, AEAD encryption/decryption is enabled. If false, AEAD encryption/decryption is disabled.
   */
  static void setUseEncryptedMessages(const bool useEncryptedMessages);
  static bool useEncryptedMessages();
  
  /** 
   * Change the key used to encrypt/decrypt messages when using AEAD encryption.
   * If no message encryption key is provided by the user, a default key consisting of all zeroes is used.
   * 
   * This changes the message encryption key for all EspnowMeshBackend instances on this ESP8266.
   * 
   * @param espnowMessageEncryptionKey An array containing the experimental::crypto::ENCRYPTION_KEY_LENGTH bytes that will be used as the message encryption key.
   */
  static void setEspnowMessageEncryptionKey(const uint8_t espnowMessageEncryptionKey[experimental::crypto::ENCRYPTION_KEY_LENGTH]);

  /** 
   * Change the key used to encrypt/decrypt messages when using AEAD encryption.
   * If no message encryption key is provided by the user, a default key consisting of all zeroes is used.
   * 
   * This changes the message encryption key for all EspnowMeshBackend instances on this ESP8266.
   * 
   * @param espnowMessageEncryptionKeySeed A string that will be used to generate the message encryption key. The same string will always generate the same key. 
   *                                       A minimum of 8 random characters are recommended to ensure sufficient key variation.
   */
  static void setEspnowMessageEncryptionKey(const String &espnowMessageEncryptionKeySeed);
  
  /**
   * Get the key used to encrypt/decrypt messages when using AEAD encryption.
   * 
   * @return An uint8_t array with size experimental::crypto::ENCRYPTION_KEY_LENGTH containing the currently used message encryption key.
   */
  static const uint8_t *getEspnowMessageEncryptionKey();
  
  /**
   * Hint: Use String.length() to get the ASCII length of a String.
   * 
   * @return The maximum number of bytes (or ASCII characters) a transmission can contain. 
   *         Note that non-ASCII characters usually require the space of at least two ASCII characters each.
   *         Also note that this value will be reduced by EspnowProtocolInterpreter::aeadMetadataSize if useEncryptedMessages() is true.
   */
  static uint32_t getMaxMessageBytesPerTransmission();

  /**
   * Set the maximum acceptable message length, in terms of transmissions, when sending a message from this node.
   * This has no effect when receiving messages, the limit for receiving is always 256 transmissions per message. 
   * Note that although values up to 128 are possible, this would in practice fill almost all the RAM available on the ESP8266 with just one message.
   * Thus, if this value is set higher than the default, make sure there is enough heap available to store the messages 
   * and don't send messages more frequently than they can be processed. 
   * Also note that a higher value will make the node less responsive as it will be spending a long time transmitting.
   * 
   * Typical symptoms of running out of heap are crashes and messages that become empty even though they shouldn't be. Keep this in mind if going beyond the default.
   * 
   * @param maxTransmissionsPerMessage The maximum acceptable message length, in terms of transmissions, when sending a message from this node. Valid values are 1 to 128. Defaults to 3.
   */
  static void setMaxTransmissionsPerMessage(const uint8_t maxTransmissionsPerMessage);
  static uint8_t getMaxTransmissionsPerMessage();

  /**
   * Hint: Use String.length() to get the ASCII length of a String.
   * 
   * @return The maximum length in bytes an ASCII message is allowed to be when transmitted/broadcasted by this node. 
   *         Note that non-ASCII characters usually require at least two bytes each.
   *         Also note that this value will be reduced if useEncryptedMessages() is true.
   */
  static uint32_t getMaxMessageLength();

  /**
   * Set whether the normal events occurring in the library will be printed to Serial or not. Off by default.
   * This setting is shared by all EspnowMeshBackend instances.
   * 
   * @param enabled If true, library Serial prints are activated.
   */
  void setVerboseModeState(const bool enabled) override;
  bool verboseMode() const override;
  
  /**
   * Only print stringToPrint if verboseMode() returns true.
   * 
   * @param stringToPrint String to print.
   * @param newline If true, will end the print with a newline. True by default.
   */
  void verboseModePrint(const String &stringToPrint, const bool newline = true) const override;

  /**
   * Same as verboseMode(), but used for printing from static functions.
   * 
   * @return True if the normal events occurring in the library will be printed to Serial. False otherwise.
   */
  static bool staticVerboseMode();
  
  /**
   * Only print stringToPrint if staticVerboseMode() returns true.
   * 
   * @param stringToPrint String to print.
   * @param newline If true, will end the print with a newline. True by default.
   */
  static void staticVerboseModePrint(const String &stringToPrint, const bool newline = true);

  /**
   * Get the message of the response at responseIndex among the responses that are scheduled for transmission from this node.
   * 
   * @param responseIndex The index of the response. Must be lower than numberOfScheduledResponses().
   * @return A String containing the message of the response at responseIndex.
   */
  static String getScheduledResponseMessage(const uint32_t responseIndex);

  /**
   * Get the MAC address for the recipient of the response at responseIndex among the responses that are scheduled for transmission from this node.
   * 
   * @param responseIndex The index of the response. Must be lower than numberOfScheduledResponses().
   * @return An array with six bytes containing the MAC address for the recipient of the response at responseIndex.
   */
  static const uint8_t *getScheduledResponseRecipient(const uint32_t responseIndex);

  /**
   * Get the number of ESP-NOW responses that are scheduled for transmission from this node.
   * 
   * @return The number of ESP-NOW responses scheduled for transmission.
   */
  static uint32_t numberOfScheduledResponses();

  /**
   * Remove all responses that have been scheduled for transmission but are not yet transmitted.
   * Note that cleared responses will not be received by their recipient.
   */
  static void clearAllScheduledResponses();

  /**
   * Remove all responses targeting recipientMac that have been scheduled for transmission but are not yet transmitted.
   * Optionally deletes only responses to encrypted requests.
   * Note that deleted responses will not be received by their recipient.
   * 
   * @param recipientMac The MAC address of the response recipient.
   * @param encryptedOnly If true, only responses to encrypted requests will be deleted.
   */
  static void deleteScheduledResponsesByRecipient(const uint8_t *recipientMac, const bool encryptedOnly);

  /**
   * Set the timeout to use for each ESP-NOW transmission when transmitting. 
   * Note that for multi-part transmissions (where message length is greater than getMaxMessageBytesPerTransmission()), the timeout is reset for each transmission part.
   * The default timeouts should fit most use cases, but in case you do a lot of time consuming processing when the node receives a message, you may need to relax them a bit.
   * 
   * @param timeoutMs The timeout that should be used for each ESP-NOW transmission, in milliseconds. Defaults to 40 ms.
   */
  static void setEspnowTransmissionTimeout(const uint32_t timeoutMs);
  static uint32_t getEspnowTransmissionTimeout();

  /**
   * Set the time to wait for an ack after having made an ESP-NOW transmission. If no ack is received within said time, a new transmission attempt is made.
   * Note that if a retransmission causes duplicate transmissions to reach the receiver, the duplicates will be detected and ignored automatically.
   * The default timeouts should fit most use cases, but in case you do a lot of time consuming processing when the node receives a message, you may need to relax them a bit.
   * 
   * @param intervalMs The time to wait for an ack after having made an ESP-NOW transmission, in milliseconds. Defaults to 15 ms.
   */
  static void setEspnowRetransmissionInterval(const uint32_t intervalMs);
  static uint32_t getEspnowRetransmissionInterval();

  // The maximum amount of time each of the two stages in an encrypted connection request may take.
  static void setEncryptionRequestTimeout(const uint32_t timeoutMs);
  static uint32_t getEncryptionRequestTimeout();
  
  void setAutoEncryptionDuration(const uint32_t duration);
  uint32_t getAutoEncryptionDuration() const;

  void setBroadcastFilter(const broadcastFilterType broadcastFilter);
  broadcastFilterType getBroadcastFilter() const;

  /**
   * Set a function that should be called after each attempted ESP-NOW response transmission.
   * In case of a successful response transmission, the call happens just before the response is removed from the waiting list.
   * Only the hook of the EspnowMeshBackend instance that is getEspnowRequestManager() will be called.
   * 
   * The hook should return a bool. 
   * If this return value is true, the response transmission process will continue with the next response in the waiting list. 
   * If it is false, the response transmission process will stop once processing of the just sent response is complete.
   * The default responseTransmittedHook always returns true.
   */
  void setResponseTransmittedHook(const EspnowTransmitter::responseTransmittedHookType responseTransmittedHook);
  EspnowTransmitter::responseTransmittedHookType getResponseTransmittedHook() const;
  
  /**
   * Get the MAC address of the sender of the most recently received ESP-NOW request, response or broadcast to this EspnowMeshBackend instance. 
   * Returns a String.
   * By default the MAC will be that of the sender's station interface. The only exception is for unencrypted 
   * responses to requests sent to an AP interface, which will return the response sender's AP interface MAC.
   * 
   * @return A String filled with a hexadecimal representation of the MAC, without delimiters.
   */
  String getSenderMac() const;

  /**
   * Get the MAC address of the sender of the most recently received ESP-NOW request, response or broadcast to this EspnowMeshBackend instance. 
   * Returns a uint8_t array.
   * By default the MAC will be that of the sender's station interface. The only exception is for unencrypted 
   * responses to requests sent to an AP interface, which will return the response sender's AP interface MAC.
   * 
   * @param macArray The array that should store the MAC address. Must be at least 6 bytes.
   * @return macArray filled with the sender MAC.
   */
  uint8_t *getSenderMac(uint8_t *macArray) const;

  /**
   * Get the AP MAC address of the sender of the most recently received ESP-NOW request, response or broadcast to this EspnowMeshBackend instance. 
   * Returns a String.
   * 
   * @return A String filled with a hexadecimal representation of the AP MAC, without delimiters.
   */
  String getSenderAPMac() const;

  /**
   * Get the AP MAC address of the sender of the most recently received ESP-NOW request, response or broadcast to this EspnowMeshBackend instance. 
   * Returns a uint8_t array.
   * 
   * @param macArray The array that should store the MAC address. Must be at least 6 bytes.
   * @return macArray filled with the sender AP MAC.
   */
  uint8_t *getSenderAPMac(uint8_t *macArray) const;

  /**
   * Get whether the ESP-NOW request, response or broadcast which was most recently received by this EspnowMeshBackend instance was sent over an encrypted connection or not. 
   * 
   * @return If true, the request, response or broadcast was sent over an encrypted connection. If false, the connection was unencrypted.
   */
  bool receivedEncryptedTransmission() const;

  /**
   * Should be used together with serializeUnencryptedConnection() if the node sends unencrypted transmissions
   * and will go to sleep for less than logEntryLifetimeMs() while other nodes stay awake.
   * Otherwise the message ID will be reset after sleep, which means that the nodes that stayed awake may ignore new unencrypted transmissions until logEntryLifetimeMs() ms has passed.
   *
   * @param serializedConnectionState A serialized state of an unencrypted ESP-NOW connection.
   * 
   * @return True if connection was added. False otherwise (e.g. if there is faulty input).
   */
  static bool addUnencryptedConnection(const String &serializedConnectionState);

  /**
   * Adds a new permanent encrypted ESP-NOW connection, or makes the duration of an existing temporary connection permanent.
   * Note that this will not add an encrypted ESP-NOW connection automatically to the other node. Thus the same method will need to be called on the other node as well to establish an encrypted connection. 
   * Methods such as requestEncryptedConnection creates an encrypted connection automatically in both nodes, but requires information exchange between the nodes before the connection is established (and is thus much slower).
   * 
   * When called, the method will update an existing encrypted ESP-NOW connection with the current stored encrypted connection key. (in case it has changed since the connection was established)
   * 
   * The maximum number of simultaneous encrypted connections is restricted by the ESP-NOW API and is EspnowProtocolInterpreter::maxEncryptedConnections (6 by default).
   * 
   * @param peerStaMac The station MAC of the other node.
   * @param peerApMac The AP MAC of the other node.
   * @param peerSessionKey The session key of the other node. At least one of the leftmost 32 bits should be 1, since the key otherwise indicates the connection is unencrypted.
   * @param peerSessionKey The session key of this node. At least one of the leftmost 32 bits should be 1, since the key otherwise indicates the connection is unencrypted.
   * 
   * @return EncryptedConnectionStatus::CONNECTION_ESTABLISHED if the connection was created. Otherwise another status code based on the outcome.
   */
  EncryptedConnectionStatus addEncryptedConnection(uint8_t *peerStaMac, uint8_t *peerApMac, const uint64_t peerSessionKey, const uint64_t ownSessionKey);

  /**
   * Create an encrypted ESP-NOW connection on this node based on the information stored in serializedConnectionState.
   * Note that this will not add an encrypted ESP-NOW connection automatically to the other node. Thus the same method will need to be called on the other node as well to establish an encrypted connection. 
   * Methods such as requestEncryptedConnection creates an encrypted connection automatically in both nodes, but requires information exchange between the nodes before the connection is established (and is thus much slower).
   * 
   * When called, the method will update an existing encrypted ESP-NOW connection with the current stored encrypted connection key. (in case it has changed since the connection was established)
   * 
   * The maximum number of simultaneous encrypted connections is restricted by the ESP-NOW API and is EspnowProtocolInterpreter::maxEncryptedConnections (6 by default).
   * 
   * Note that the espnowEncryptedConnectionKey, espnowEncryptionKok, espnowHashKey and espnowMessageEncryptionKey are not serialized.
   * These will be set to the values of the EspnowMeshBackend instance that is adding the serialized encrypted connection.
   * 
   * @param serializedConnectionState A String containing the serialized connection state.
   * @param ignoreDuration Ignores any stored duration in serializedConnectionState, guaranteeing that the created connection will be permanent. 
   * 
   * @return EncryptedConnectionStatus::CONNECTION_ESTABLISHED if the connection was created. Otherwise another status code based on the outcome. EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED indicates a malformed serializedConnectionState.
   */
  EncryptedConnectionStatus addEncryptedConnection(const String &serializedConnectionState, const bool ignoreDuration = false);

  /**
   * Adds a new temporary encrypted ESP-NOW connection, or changes the duration of an existing temporary connection (only updates keys, not duration, for existing permanent connections).
   * Note that this will not add an encrypted ESP-NOW connection automatically to the other node. Thus the same method will need to be called on the other node as well to establish an encrypted connection. 
   * Methods such as requestEncryptedConnection creates an encrypted connection automatically in both nodes, but requires information exchange between the nodes before the connection is established (and is thus much slower).
   * 
   * When called, the method will update an existing encrypted ESP-NOW connection with the current stored encrypted connection key. (in case it has changed since the connection was established)
   * 
   * The maximum number of simultaneous encrypted connections is restricted by the ESP-NOW API and is EspnowProtocolInterpreter::maxEncryptedConnections (6 by default).
   * 
   * As with all these methods, changes will only take effect once the requester proves it has the ability to decrypt the session key.
   * 
   * @param peerStaMac The station MAC of the other node.
   * @param peerApMac The AP MAC of the other node.
   * @param peerSessionKey The session key of the other node. At least one of the leftmost 32 bits should be 1, since the key otherwise indicates the connection is unencrypted.
   * @param peerSessionKey The session key of this node. At least one of the leftmost 32 bits should be 1, since the key otherwise indicates the connection is unencrypted.
   * @param duration The desired duration of the connection.
   * 
   * @return EncryptedConnectionStatus::CONNECTION_ESTABLISHED if the connection was created. Otherwise another status code based on the outcome.
   */ 
  EncryptedConnectionStatus addTemporaryEncryptedConnection(uint8_t *peerStaMac, uint8_t *peerApMac, const uint64_t peerSessionKey, const uint64_t ownSessionKey, const uint32_t duration);
  
  /**
   * Create a temporary encrypted ESP-NOW connection on this node based on the information stored in serializedConnectionState.
   * Note that this will not add an encrypted ESP-NOW connection automatically to the other node. Thus the same method will need to be called on the other node as well to establish an encrypted connection. 
   * Methods such as requestEncryptedConnection creates an encrypted connection automatically in both nodes, but requires information exchange between the nodes before the connection is established (and is thus much slower).
   * 
   * When called, the method will update an existing encrypted ESP-NOW connection with the current stored encrypted connection key. (in case it has changed since the connection was established)
   * 
   * The maximum number of simultaneous encrypted connections is restricted by the ESP-NOW API and is EspnowProtocolInterpreter::maxEncryptedConnections (6 by default).
   * 
   * Note that the espnowEncryptedConnectionKey, espnowEncryptionKok, espnowHashKey and espnowMessageEncryptionKey are not serialized.
   * These will be set to the values of the EspnowMeshBackend instance that is adding the serialized encrypted connection.
   * 
   * @param serializedConnectionState A String containing the serialized connection state.
   * @param ignoreDuration Ignores any stored duration in serializedConnectionState, guaranteeing that the created connection will be permanent.
   * @param duration The desired duration of the connection. Overrides any stored duration in the serializedConnectionState.
   * 
   * @return EncryptedConnectionStatus::CONNECTION_ESTABLISHED if the connection was created. Otherwise another status code based on the outcome. EncryptedConnectionStatus::REQUEST_TRANSMISSION_FAILED indicates a malformed serializedConnectionState.
   */
  EncryptedConnectionStatus addTemporaryEncryptedConnection(const String &serializedConnectionState, const uint32_t duration);

  /**
   * Request a permanent encrypted ESP-NOW connection with the node that uses peerMac.
   * If an encrypted connection to peerMac already exists, only connection duration is updated. All other settings are kept as is. Use removeEncryptedConnection/requestEncryptedConnectionRemoval first if encryption keys should be updated.
   * The method makes sure both nodes have an encrypted connection to each other that's permanent.
   * 
   * The maximum number of simultaneous encrypted connections is restricted by the ESP-NOW API and is EspnowProtocolInterpreter::maxEncryptedConnections (6 by default).
   * 
   * @param peerMac The MAC of the other node to which the request should be sent.
   * 
   * @return EncryptedConnectionStatus::CONNECTION_ESTABLISHED if the permanent connection was created. EncryptedConnectionStatus::SOFT_LIMIT_CONNECTION_ESTABLISHED if only a temporary soft limit connection could be established (see the setEncryptedConnectionsSoftLimit method documentation for details). Otherwise another status code based on the outcome.
   */
  EncryptedConnectionStatus requestEncryptedConnection(const uint8_t *peerMac);

  /**
   * Request a temporary encrypted ESP-NOW connection with the node that uses peerMac.
   * If a temporary encrypted connection to peerMac already exists, only connection duration is updated. All other settings are kept as is. Permanent connections are not modified. Use removeEncryptedConnection/requestEncryptedConnectionRemoval first if encryption keys should be updated.
   * The method makes sure both nodes have an encrypted connection to each other that's either permanent or has exactly the duration specified.
   * 
   * The maximum number of simultaneous encrypted connections is restricted by the ESP-NOW API and is EspnowProtocolInterpreter::maxEncryptedConnections (6 by default).
   * 
   * @param peerMac The MAC of the other node to which the request should be sent.
   * @param durationMs The desired duration of the connection.
   * 
   * @return EncryptedConnectionStatus::CONNECTION_ESTABLISHED if the request was succesful. EncryptedConnectionStatus::SOFT_LIMIT_CONNECTION_ESTABLISHED if only a temporary soft limit connection could be established (see the setEncryptedConnectionsSoftLimit method documentation for details). Otherwise another status code based on the outcome.
   */
  EncryptedConnectionStatus requestTemporaryEncryptedConnection(const uint8_t *peerMac, const uint32_t durationMs);

  /**
   * Request a flexible temporary encrypted ESP-NOW connection with the node that uses peerMac.
   * If a temporary encrypted connection to peerMac with a shorter duration already exists, connection duration is updated. All other settings are kept as is. Permanent connections are not modified. Use removeEncryptedConnection/requestEncryptedConnectionRemoval first if encryption keys should be updated.
   * The method makes sure both nodes have an encrypted connection to each other that's either permanent or has at least the duration specified.
   * 
   * The maximum number of simultaneous encrypted connections is restricted by the ESP-NOW API and is EspnowProtocolInterpreter::maxEncryptedConnections (6 by default).
   * 
   * Note that if a temporary encrypted connection already exists to a target node, this method will slightly extend the connection duration 
   * depending on the time it takes to verify the connection to the node.
   * 
   * @param peerMac The MAC of the other node to which the request should be sent.
   * @param minDurationMs The desired minimum duration of the connection.
   * 
   * @return EncryptedConnectionStatus::CONNECTION_ESTABLISHED if the request was succesful. EncryptedConnectionStatus::SOFT_LIMIT_CONNECTION_ESTABLISHED if only a temporary soft limit connection could be established (see the setEncryptedConnectionsSoftLimit method documentation for details). Otherwise another status code based on the outcome.
   */
  EncryptedConnectionStatus requestFlexibleTemporaryEncryptedConnection(const uint8_t *peerMac, const uint32_t minDurationMs);
 
  /**
   * Remove the encrypted ESP-NOW connection to peerMac from this node.
   * Note that this will not remove the encrypted ESP-NOW connection automatically from the other node. Thus the same method will need to be called on the other node as well to complete the encrypted connection removal. 
   * The method requestEncryptedConnectionRemoval removes the encrypted connection automatically in both nodes, but requires extra information exchange between the nodes (and is thus much slower).
   * 
   * @param peerMac The MAC of the other node.
   * 
   * @return EncryptedConnectionRemovalOutcome::REMOVAL_SUCCEEDED if the removal succeeded. EncryptedConnectionRemovalOutcome::REMOVAL_SCHEDULED if the removal is scheduled to occur as soon as it is safe to do so (generally as soon as an ongoing transmission is complete, or at the latest during the next performEspnowMaintenance call). Otherwise another status code based on the outcome.
   */
  static EncryptedConnectionRemovalOutcome removeEncryptedConnection(const uint8_t *peerMac);

  /**
   * Request the removal of the encrypted ESP-NOW connection between this node and the node that uses peerMac.
   * The method makes sure both nodes remove the encrypted connection to each other.
   * 
   * @param peerMac The MAC of the other node to which the request should be sent.
   * 
   * @return EncryptedConnectionRemovalOutcome::REMOVAL_SUCCEEDED if the removal succeeded. Otherwise another status code based on the outcome (never REMOVAL_SCHEDULED).
   */
  EncryptedConnectionRemovalOutcome requestEncryptedConnectionRemoval(const uint8_t *peerMac);

  /**
   * Set whether this EspnowMeshBackend instance will accept ESP-NOW requests from unencrypted connections or not, when acting as EspnowRequestManager. 
   * When set to false and combined with already existing encrypted connections, this can be used to ensure only encrypted transmissions are processed.
   * When set to false it will also make it impossible to send requests for encrypted connection to the node over an unencrypted connection, 
   * which can be useful if too many such requests could otherwise be expected.
   * 
   * True by default.
   * 
   * @param acceptsUnverifiedRequests If and only if true, requests from unencrypted connections will be processed when this EspnowMeshBackend instance is acting as EspnowRequestManager.
   */
  void setAcceptsUnverifiedRequests(const bool acceptsUnverifiedRequests);
  bool acceptsUnverifiedRequests() const;

  /**
   * Set a soft upper limit on the number of encrypted connections this node can have when receiving encrypted connection requests.
   * The soft limit can be used to ensure there is normally a pool of free encrypted connection slots that can be used if required.
   * Each EspnowMeshBackend instance can have a separate value. The value used is that of the current EspnowRequestManager.
   * The hard upper limit is 6 encrypted connections, mandated by the ESP-NOW API.
   * 
   * Default is 6.
   * 
   * When a request for encrypted connection is received from a node to which there is no existing permanent encrypted connection,
   * and the number of encrypted connections exceeds the soft limit,
   * this request will automatically be converted to an autoEncryptionRequest.
   * This means it will be a temporary connection with very short duration (with default framework settings).
   * 
   * @param softLimit The new soft limit. Valid values are 0 to 6.
   */
  void setEncryptedConnectionsSoftLimit(const uint8_t softLimit);
  uint8_t encryptedConnectionsSoftLimit() const;
  
  /**
   * @return The current number of encrypted ESP-NOW connections.
   */
  static uint8_t numberOfEncryptedConnections();

  /** 
   * @return resultArray filled with the MAC to the encrypted interface of the node, if an encrypted connection exists. nulltpr otherwise.
   */
  static uint8_t *getEncryptedMac(const uint8_t *peerMac, uint8_t *resultArray);

  /**
   * Should be used together with addUnencryptedConnection if the node sends unencrypted transmissions
   * and will go to sleep for less than logEntryLifetimeMs() while other nodes stay awake.
   * Otherwise the message ID will be reset after sleep, which means that the nodes that stayed awake may ignore new unencrypted transmissions until logEntryLifetimeMs() ms has passed.
   *
   * @return The serialized state of the unencrypted ESP-NOW connection.
   */
  static String serializeUnencryptedConnection();
 
  /** 
   *  Create a string containing the current state of the encrypted connection for this node. The result can be used as input to addEncryptedConnection.
   *  Note that transferring the serialized state over an unencrypted connection will compromise the security of the stored connection.
   *  Also note that this saves the current state only, so if encrypted communication between the nodes happen after this, the stored state is invalid.
   *  @return A String containing the serialized encrypted connection, or an empty String if there is no matching encrypted connection.
   */
  static String serializeEncryptedConnection(const uint8_t *peerMac);
  static String serializeEncryptedConnection(const uint32_t connectionIndex);

  /**
   * Get information about any current ESP-NOW connection with another node.
   * 
   * @param peerMac The node MAC for which to get information. Both MAC for AP interface and MAC for STA interface can be used (and will yield the same result).
   *                Use the getEncryptedMac method or the indexed based getConnectionInfo if there is a need to find the actual encrypted interface.
   * @param remainingDuration An optional pointer to a uint32_t variable. 
   *                          If supplied and the connection type is ConnectionType::TEMPORARY_CONNECTION the variable will be set to the remaining duration of the connection. 
   *                          Otherwise the variable value is not modified.
   * @return The ConnectionType of the connection with peerMac.
   */
  static ConnectionType getConnectionInfo(uint8_t *peerMac, uint32_t *remainingDuration = nullptr);
  
  /**
   * Get information about any current ESP-NOW connection with another node.
   * 
   * @param connectionIndex The connection index of the node for which to get information. Valid values are limited by numberOfEncryptedConnections().
   * @param remainingDuration An optional pointer to a uint32_t variable. 
   *                          If supplied and the connection type is ConnectionType::TEMPORARY_CONNECTION the variable will be set to the remaining duration of the connection. 
   *                          Otherwise the variable value is not modified.
   * @param peerMac An optional pointer to an uint8_t array with at least size 6. It will be filled with the MAC of the encrypted peer interface if an encrypted connection exists.
   *                Otherwise the array is not modified.
   * @return The ConnectionType of the connection given by connectionIndex.
   */
  static ConnectionType getConnectionInfo(const uint32_t connectionIndex, uint32_t *remainingDuration = nullptr, uint8_t *peerMac = nullptr);

  /**
   * @return The proportion of ESP-NOW requests made by this node that have failed, since power on or latest reset.
   */
  static double getTransmissionFailRate();
  
  /**
   * Reset TransmissionFailRate back to 0.
   */
  static void resetTransmissionFailRate();

  void setWiFiChannel(const uint8 newWiFiChannel) override;
  
protected:

  EspnowDatabase *getDatabase();
  const EspnowDatabase *getDatabaseConst() const;
  EspnowConnectionManager *getConnectionManager();
  const EspnowConnectionManager *getConnectionManagerConst() const;
  EspnowTransmitter *getTransmitter();
  const EspnowTransmitter *getTransmitterConst() const;
  EspnowEncryptionBroker *getEncryptionBroker();
  const EspnowEncryptionBroker *getEncryptionBrokerConst() const;

  bool activateEspnow();
  
  /*
   * Note that ESP-NOW is not perfect and in rare cases messages may be dropped. 
   * This needs to be compensated for in the application via extra verification 
   * (e.g. by always sending a response such as a message hash), if message delivery must be guaranteed.
   * 
   * Note that although responses will generally be sent in the order they were created, this is not guaranteed to be the case.
   * For example, response order will be mixed up if some responses fail to transmit while others transmit successfully.
   * 
   * @param estimatedMaxDurationTracker A pointer to an ExpiringTimeTracker initialized with the desired max duration for the method. If set to nullptr there is no duration limit. 
   *                                    Note that setting the estimatedMaxDuration too low may result in missed ESP-NOW transmissions because of too little time for maintenance.
   *                                    Also note that although the method will try to respect the max duration limit, there is no guarantee. Overshoots by tens of milliseconds are possible.
   */
  static void sendStoredEspnowMessages(const ExpiringTimeTracker *estimatedMaxDurationTracker = nullptr);

  using macAndType_td = EspnowProtocolInterpreter::macAndType_td;
  using messageID_td = EspnowProtocolInterpreter::messageID_td;
  using peerMac_td = EspnowProtocolInterpreter::peerMac_td;

private:

  EspnowMeshBackend(const requestHandlerType requestHandler, const responseHandlerType responseHandler, const networkFilterType networkFilter, const broadcastFilterType broadcastFilter,
                    const String &meshPassword, const String &ssidPrefix, const String &ssidSuffix, const bool verboseMode, const uint8 meshWiFiChannel);

  EspnowDatabase _database;
  EspnowConnectionManager _connectionManager;
  EspnowTransmitter _transmitter;
  EspnowEncryptionBroker _encryptionBroker;

  void prepareForTransmission(const String &message, const bool scan, const bool scanAllWiFiChannels);
  TransmissionStatusType initiateTransmission(const String &message, const EspnowNetworkInfo &recipientInfo);
  TransmissionStatusType initiateTransmissionKernel(const String &message, const uint8_t *targetBSSID);  
  TransmissionStatusType initiateAutoEncryptingTransmission(const String &message, uint8_t *targetBSSID, const EncryptedConnectionStatus connectionStatus);
  void printTransmissionStatistics() const;
  
  // Used for verboseMode printing in attemptTransmission, _AT suffix used to reduce namespace clutter
  uint32_t totalDurationWhenSuccessful_AT = 0;
  uint32_t successfulTransmissions_AT = 0;
  uint32_t maxTransmissionDuration_AT = 0;

  /**
   * We can't feed esp_now_register_recv_cb our EspnowMeshBackend instance's espnowReceiveCallback method directly, so this callback wrapper is a workaround.
   * 
   * This method is very time critical so avoid Serial printing in it and in methods called from it (such as espnowReceiveCallback) as much as possible. 
   * Otherwise transmission fail rate is likely to skyrocket.
   */
  static void espnowReceiveCallbackWrapper(uint8_t *macaddr, uint8_t *dataArray, const uint8_t len);
  void espnowReceiveCallback(const uint8_t *macaddr, uint8_t *data, const uint8_t len);

  broadcastFilterType _broadcastFilter;

  bool _acceptsUnverifiedRequests = true;
};

#endif
