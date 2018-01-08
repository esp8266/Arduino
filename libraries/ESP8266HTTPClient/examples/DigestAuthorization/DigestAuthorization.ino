/*

   This example is released into public domain,
   or, at your option, CC0 licensed.
 */

#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>

const char* ssid = "........";
const char* password = "........";

const char *username = "admin";
const char *password = "admin";

const char *server = "http://192.168.100.1";
const char *uri = "/api/state?power=1";

String exractParam(String& authReq, const String& param, const char delimit){
	int _begin = authReq.indexOf(param);
	if (_begin==-1) return "";
	return authReq.substring(_begin+param.length(),authReq.indexOf(delimit,_begin+param.length()));
}

void setup() {
	Serial.begin(9600);

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

void loop() {
	HTTPClient http;

	Serial.print("[HTTP] begin...\n");

	// configure traged server and url
	http.begin(String(server) + String(uri));


	const char *keys[] = {"WWW-Authenticate"};
	http.collectHeaders(keys, 1);

	Serial.print("[HTTP] GET...\n");
	// start connection and send HTTP header
	int httpCode = http.GET();

	if (httpCode > 0) {
		String authReq = http.header("WWW-Authenticate");
		Serial.println(authReq);

		// extracting required parameters for RFC 2069 simpler Digest
		String _realm    = _exractParam(authReq, "realm=\"", '"');
		String _nonce    = _exractParam(authReq, "nonce=\"", '"');
		String _opaque   = _exractParam(authReq, "opaque=\"", '"');

		// parameters for the RFC 2617 newer Digest
		MD5Builder md5;
		md5.begin();
		md5.add(String(username) + ":" + _realm + ":" + String(password));  // md5 of the user:realm:user
		md5.calculate();
		String _H1 = md5.toString();

		md5.begin();
		md5.add(String("GET:") + String(uri));
		md5.calculate();
		String _H2 = md5.toString();

		md5.begin();
		md5.add(_H1 + ":" + _nonce + ":" + "00000001" + ":" + "gDBuFY4s" + ":" + "auth" + ":" + _H2);
		md5.calculate();
		String _response = md5.toString();

		http.end();
		http.begin(String(server) + String(uri));

		String authorization = "Digest username=\"admin\", realm=\"" + _realm + "\", nonce=\"" + _nonce + "\", uri=\"" + uri + "\", algorithm=\"MD5\", qop=auth, nc=00000001, cnonce=\"gDBuFY4s\", response=\"" + _response + "\"";
		Serial.println(authorization);
		http.addHeader("Authorization", authorization);

		int httpCode = http.GET();
		if (httpCode > 0) {
			payload = http.getString();
			Serial.println(payload);
		} else {
			Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}
	} else {
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
	delay(10000);
}
