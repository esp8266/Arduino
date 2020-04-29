/**
   This example demonstrates the usage of the ESP8266 Crypto implementation, which aims to contain easy-to-use cryptographic functions.
   Crypto is currently primarily a frontend for the cryptographic library BearSSL which is used by `BearSSL::WiFiClientSecure` and `BearSSL::WiFiServerSecure` in the ESP8266 Arduino Core.
   Extensive documentation can be found in the Crypto source code files and on the [BearSSL homepage](https://www.bearssl.org).
*/

#include <ESP8266WiFi.h>
#include <TypeConversion.h>
#include <Crypto.h>

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

  Serial.println();
  Serial.println();
}

void loop() {
  // This serves only to demonstrate the library use. See the header file for a full list of functions.

  String exampleData = F("Hello Crypto World!");
  Serial.println(String(F("This is our example data: ")) + exampleData);

  uint8_t resultArray[crypto::SHA256::NATURAL_LENGTH] { 0 };
  uint8_t derivedKey[crypto::ENCRYPTION_KEY_LENGTH] { 0 };

  static uint32_t encryptionCounter = 0;


  // Generate the salt to use for HKDF
  uint8_t hkdfSalt[16] { 0 };
  crypto::getNonceGenerator()(hkdfSalt, sizeof hkdfSalt);

  // Generate the key to use for HMAC and encryption
  crypto::HKDF hkdfInstance(FPSTR(masterKey), (sizeof masterKey) - 1, hkdfSalt, sizeof hkdfSalt); // (sizeof masterKey) - 1 removes the terminating null value of the c-string
  hkdfInstance.produce(derivedKey, sizeof derivedKey);

  // Hash
  crypto::SHA256::hash(exampleData.c_str(), exampleData.length(), resultArray);
  Serial.println(String(F("\nThis is the SHA256 hash of our example data, in HEX format:\n")) + TypeCast::uint8ArrayToHexString(resultArray, sizeof resultArray));
  Serial.println(String(F("This is the SHA256 hash of our example data, in HEX format, using String output:\n")) + crypto::SHA256::hash(exampleData));


  // HMAC
  // Note that HMAC output length is limited
  crypto::SHA256::hmac(exampleData.c_str(), exampleData.length(), derivedKey, sizeof derivedKey, resultArray, sizeof resultArray);
  Serial.println(String(F("\nThis is the SHA256 HMAC of our example data, in HEX format:\n")) + TypeCast::uint8ArrayToHexString(resultArray, sizeof resultArray));
  Serial.println(String(F("This is the SHA256 HMAC of our example data, in HEX format, using String output:\n")) + crypto::SHA256::hmac(exampleData, derivedKey, sizeof derivedKey, crypto::SHA256::NATURAL_LENGTH));


  // Authenticated Encryption with Associated Data (AEAD)
  String dataToEncrypt = F("This data is not encrypted.");
  uint8_t resultingNonce[12] { 0 }; // The nonce is always 12 bytes
  uint8_t resultingTag[16] { 0 }; // The tag is always 16 bytes

  Serial.println(String(F("\nThis is the data to encrypt: ")) + dataToEncrypt);

  // Note that the key must be ENCRYPTION_KEY_LENGTH long.
  crypto::ChaCha20Poly1305::encrypt(dataToEncrypt.begin(), dataToEncrypt.length(), derivedKey, &encryptionCounter, sizeof encryptionCounter, resultingNonce, resultingTag);
  Serial.println(String(F("Encrypted data: ")) + dataToEncrypt);

  bool decryptionSucceeded = crypto::ChaCha20Poly1305::decrypt(dataToEncrypt.begin(), dataToEncrypt.length(), derivedKey, &encryptionCounter, sizeof encryptionCounter, resultingNonce, resultingTag);
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
