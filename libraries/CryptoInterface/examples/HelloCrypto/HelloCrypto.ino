/**
   This example shows the functionality of the CryptoInterface library.
*/

#include <ESP8266WiFi.h>
#include <TypeConversion.h>
#include <CryptoInterface.h>

namespace TypeCast = esp8266::TypeConversion;
using namespace experimental;

/**
   NOTE: Although we could define the strings below as normal String variables,
   here we are using PROGMEM combined with the FPSTR() macro (and also just the F() macro further down in the file).
   The reason is that this approach will place the strings in flash memory which will help save RAM during program execution.
   Reading strings from flash will be slower than reading them from RAM,
   but this will be a negligible difference when printing them to Serial.

   More on F(), FPSTR() and PROGMEM:
   https://github.com/esp8266/Arduino/issues/1143
   https://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html
*/
constexpr char masterKey[] PROGMEM = "w86vn@rpfA O+S"; // Use 8 random characters or more

void setup() {
  // Prevents the flash memory from being worn out, see: https://github.com/esp8266/Arduino/issues/1054 .
  // This will however delay node WiFi start-up by about 700 ms. The delay is 900 ms if we otherwise would have stored the WiFi network we want to connect to.
  WiFi.persistent(false);

  Serial.begin(115200);
  delay(50); // Wait for Serial.

  //yield(); // Use this if you don't want to wait for Serial.

  // The WiFi.disconnect() ensures that the WiFi is working correctly. If this is not done before receiving WiFi connections,
  // those WiFi connections will take a long time to make or sometimes will not work at all.
  WiFi.disconnect();

  Serial.println();
  Serial.println();
}

void loop() {
  // This serves only to demonstrate the library use. See the header file for a full list of functions.

  String exampleData = F("Hello Crypto World!");
  Serial.println(String(F("This is our example data: ")) + exampleData);

  uint8_t resultArray[CryptoInterface::SHA256_NATURAL_LENGTH] { 0 };
  uint8_t derivedKey[CryptoInterface::ENCRYPTION_KEY_LENGTH] { 0 };

  static uint32_t encryptionCounter = 0;


  // Generate the salt to use for HKDF
  uint8_t hkdfSalt[16] { 0 };
  CryptoInterface::getNonceGenerator()(hkdfSalt, sizeof hkdfSalt);

  // Generate the key to use for HMAC and encryption
  CryptoInterface::hkdfInit(FPSTR(masterKey), (sizeof masterKey) - 1, hkdfSalt, sizeof hkdfSalt); // (sizeof masterKey) - 1 removes the terminating null value of the c-string
  CryptoInterface::hkdfProduce(derivedKey, sizeof derivedKey);

  // Hash
  CryptoInterface::sha256Hash(exampleData.c_str(), exampleData.length(), resultArray);
  Serial.println(String(F("\nThis is the SHA256 hash of our example data, in HEX format:\n")) + TypeCast::uint8ArrayToHexString(resultArray, sizeof resultArray));
  Serial.println(String(F("This is the SHA256 hash of our example data, in HEX format, using String output:\n")) + CryptoInterface::sha256Hash(exampleData));


  // HMAC
  // Note that HMAC output length is limited
  CryptoInterface::sha256Hmac(exampleData.c_str(), exampleData.length(), derivedKey, sizeof derivedKey, resultArray, sizeof resultArray);
  Serial.println(String(F("\nThis is the SHA256 HMAC of our example data, in HEX format:\n")) + TypeCast::uint8ArrayToHexString(resultArray, sizeof resultArray));
  Serial.println(String(F("This is the SHA256 HMAC of our example data, in HEX format, using String output:\n")) + CryptoInterface::sha256Hmac(exampleData, derivedKey, sizeof derivedKey, CryptoInterface::SHA256_NATURAL_LENGTH));


  // Authenticated Encryption with Associated Data (AEAD)
  String dataToEncrypt = F("This data is not encrypted.");
  uint8_t resultingNonce[12] { 0 }; // The nonce is always 12 bytes
  uint8_t resultingTag[16] { 0 }; // The tag is always 16 bytes

  Serial.println(String(F("\nThis is the data to encrypt: ")) + dataToEncrypt);

  // Note that the key must be ENCRYPTION_KEY_LENGTH long.
  CryptoInterface::chacha20Poly1305Encrypt(dataToEncrypt.begin(), dataToEncrypt.length(), derivedKey, &encryptionCounter, sizeof encryptionCounter, resultingNonce, resultingTag);
  Serial.println(String(F("Encrypted data: ")) + dataToEncrypt);

  bool decryptionSucceeded = CryptoInterface::chacha20Poly1305Decrypt(dataToEncrypt.begin(), dataToEncrypt.length(), derivedKey, &encryptionCounter, sizeof encryptionCounter, resultingNonce, resultingTag);
  encryptionCounter++;

  if (decryptionSucceeded) {
    Serial.print(F("Decryption succeeded. Result: "));
  } else {
    Serial.print(F("Decryption failed. Result: "));
  }

  Serial.println(dataToEncrypt);


  Serial.println(F("\n##########################################################################################################\n"));

  delay(10000);
}
