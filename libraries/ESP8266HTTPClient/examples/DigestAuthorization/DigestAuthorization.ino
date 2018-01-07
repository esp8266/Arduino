#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

String _exractParam(String& authReq, const String& param, const char delimit){
	int _begin = authReq.indexOf(param);
	if (_begin==-1) return "";
	return authReq.substring(_begin+param.length(),authReq.indexOf(delimit,_begin+param.length()));
}

void setup() {

	USE_SERIAL.begin(9600);
	// USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	WiFi.mode(WIFI_STA);
	WiFiMulti.addAP("Niligo-Prism-yyr9uma", "");

}

void loop() {
	// wait for WiFi connection
	if((WiFiMulti.run() == WL_CONNECTED)) {

		HTTPClient http;

		USE_SERIAL.print("[HTTP] begin...\n");
		// configure traged server and url


		// http.begin("http://admin:admin@192.168.100.1/api/state?power=1");
		http.begin("http://192.168.100.1/api/state?power=0");
		/*
		// or
		// or
		http.begin("http://192.168.1.12/test.html");
		http.setAuthorization("dXNlcjpwYXN3b3Jk");
		 */


		const char *keys[] = {"WWW-Authenticate", "Server", "Connection"};
		http.collectHeaders(keys, 3);

		USE_SERIAL.print("[HTTP] GET...\n");
		// start connection and send HTTP header
		int httpCode = http.GET();

		if (httpCode > 0) {
			String authReq = http.header("WWW-Authenticate");
			USE_SERIAL.println(authReq);

			// file found at server
			String payload = http.getString();
			USE_SERIAL.println(payload);

			String username = "admin";
			String password = "admin";
			
			// extracting required parameters for RFC 2069 simpler Digest
			// String _realm    = _exractParam(authReq, "realm=\"", '"');
			String _realm = "Protected";
			String _nonce    = _exractParam(authReq, "nonce=\"", '"');
			// String _nonce = "2cadff7e2f160969";
			String _opaque   = _exractParam(authReq, "opaque=\"", '"');

			// parameters for the RFC 2617 newer Digest
			MD5Builder md5;
			md5.begin();
			md5.add(username + ":" + _realm + ":" + password);  // md5 of the user:realm:user
			md5.calculate();
			String _H1 = md5.toString();

			md5.begin();
			md5.add(String("GET:") + String("/api/state?power=0"));
			md5.calculate();
			String _H2 = md5.toString();

			md5.begin();
			md5.add(_H1 + ":" + _nonce + ":" + "00000001" + ":" + "gDBuFY4s" + ":" + "auth" + ":" + _H2);
			md5.calculate();
			String _response = md5.toString();

			http.begin("http://192.168.100.1/api/state?power=0");
			USE_SERIAL.println(_nonce);
			String Author = "Digest username=\"admin\", realm=\"Protected\", nonce=\"" + _nonce + "\", uri=\"/api/state?power=0\", algorithm=\"MD5\", qop=auth, nc=00000001, cnonce=\"gDBuFY4s\", response=\"" + _response + "\"\r\n";
			USE_SERIAL.println(Author);
			http.addHeader("Authorization", Author);

			http.GET();

			payload = http.getString();
			USE_SERIAL.println(payload);
		} else {
			USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}

		http.end();
	}

	delay(10000);
}

