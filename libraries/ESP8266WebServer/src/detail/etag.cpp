#include <ESP8266WebServer.h>

namespace esp8266webserver {

// calculate an ETag for a file in filesystem based on md5 checksum
// that can be used in the http headers - include quotes.
String calcETag(FS &fs, const String &path) {
    String result;

    // calculate eTag using md5 checksum
    uint8_t md5_buf[16];
    File f = fs.open(path, "r");
    MD5Builder calcMD5;
    calcMD5.begin();
    calcMD5.addStream(f, f.size());
    calcMD5.calculate();
    calcMD5.getBytes(md5_buf);
    f.close();
    // create a minimal-length eTag using base64 byte[]->text encoding.
    result = "\"" + base64::encode(md5_buf, 16, false) + "\"";
    return(result);
}

} // namespace esp8266webserver
