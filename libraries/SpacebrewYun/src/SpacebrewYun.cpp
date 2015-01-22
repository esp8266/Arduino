#include "SpacebrewYun.h"


SpacebrewYun::SpacebrewYun(const String& _name, const String& _description) {
	name = _name;
	description = _description;
	subscribers = NULL;
	publishers = NULL;

	server = "sandbox.spacebrew.cc";
	port = 9000;

	_started = false;
	_connected = false;
	_verbose = false;
	_error_msg = false;

	sub_name = "";
	sub_msg = "";
	sub_type = "";

	read_name = false;
	read_msg = false;

	connect_attempt = 0;
	connect_attempt_inter = 10000;

	for ( int i = 0; i < pidLength; i++ ) {
		pid [i] = '\0';
	}

	for ( int i = 0; i < sbPidsLen; i++ ) {
		sbPids [i] = '\0';
	}

	Console.buffer(64);

}

int SpacebrewYun::sub_msg_int_max = 6;
int SpacebrewYun::sub_msg_bool_max = 5;
int SpacebrewYun::sub_msg_str_max = 50;
int SpacebrewYun::sub_name_max = 20;
SpacebrewYun::OnBooleanMessage SpacebrewYun::_onBooleanMessage = NULL;
SpacebrewYun::OnRangeMessage SpacebrewYun::_onRangeMessage = NULL;
SpacebrewYun::OnStringMessage SpacebrewYun::_onStringMessage = NULL;
SpacebrewYun::OnCustomMessage SpacebrewYun::_onCustomMessage = NULL;
SpacebrewYun::OnSBOpen SpacebrewYun::_onOpen = NULL;
SpacebrewYun::OnSBClose SpacebrewYun::_onClose = NULL;
SpacebrewYun::OnSBError SpacebrewYun::_onError = NULL;

void SpacebrewYun::onOpen(OnSBOpen function){
	_onOpen = function;
}
void SpacebrewYun::onClose(OnSBClose function){
	_onClose = function;
}
void SpacebrewYun::onRangeMessage(OnRangeMessage function){
	_onRangeMessage = function;
}
void SpacebrewYun::onStringMessage(OnStringMessage function){
	_onStringMessage = function;
}
void SpacebrewYun::onBooleanMessage(OnBooleanMessage function){
	_onBooleanMessage = function;
}
void SpacebrewYun::onCustomMessage(OnCustomMessage function){
	_onCustomMessage = function;
}
void SpacebrewYun::onError(OnSBError function){
	_onError = function;
}

void SpacebrewYun::addPublish(const String& name, const String& type) {
	struct Publisher *p = new Publisher();
	p->name = createString(name.length() + 1);
	p->type = createString(type.length() + 1);
	p->confirmed = false;
	p->time = 0;
	if (type == "range") {
		p->lastMsg = createString(sub_msg_int_max);	
		emptyString(p->lastMsg, sub_msg_int_max);	
	}
	else if (type == "boolean") {
		p->lastMsg = createString(sub_msg_bool_max);		
		emptyString(p->lastMsg, sub_msg_bool_max);	
	}
	else {
		p->lastMsg = createString(sub_msg_str_max);		
		emptyString(p->lastMsg, sub_msg_str_max);	
	}
	name.toCharArray(p->name, name.length() + 1);
	type.toCharArray(p->type, type.length() + 1);

	if (publishers == NULL){
		publishers = p;
	} 
	else {
		struct Publisher *curr = publishers;
		int counter = 1;
		while(curr->next != NULL){
			curr = curr->next;
			counter++;
		}
		curr->next = p;
	}
	p->next = NULL;
}

void SpacebrewYun::addSubscribe(const String& name, const String& type) {
	Subscriber *s = new Subscriber();
	s->name = createString(name.length() + 1);
	s->type = createString(type.length() + 1);
	name.toCharArray(s->name, name.length() + 1);
	type.toCharArray(s->type, type.length() + 1);

	if (subscribers == NULL){
		subscribers = s;
	} 
	else {
		struct Subscriber *curr = subscribers;
		while(curr->next != NULL){
			curr = curr->next;
		}
		curr->next = s;
	}
}

void SpacebrewYun::connect(String _server, int _port) {
	Serial.println(F("v2.3"));
	_started = true;
	server = _server;
	port = _port; 
	connect_attempt = millis();

	killPids();

	brew.begin("run-spacebrew"); // Process should launch the "curl" command
	// brew.begin("python"); // Process should launch the "curl" command
	// brew.addParameter("/usr/lib/python2.7/spacebrew/spacebrew.py"); // Process should launch the "curl" command
	brew.addParameter("--server");
	brew.addParameter(server);
	brew.addParameter("--port");
	brew.addParameter(String(port));
	brew.addParameter("-n");
	brew.addParameter(name);
	brew.addParameter("-d");
	brew.addParameter(description);

	if (subscribers != NULL) {
		struct Subscriber *curr = subscribers;
		while(curr != NULL){
			if (_verbose) {
				Serial.print(F("Creating subscribers: "));
				Serial.print(curr->name);
				Serial.print(F(" of type: "));
				Serial.println(curr->type);
			}

			brew.addParameter("-s"); // Add the URL parameter to "curl"
			brew.addParameter(curr->name); // Add the URL parameter to "curl"
			brew.addParameter(","); // Add the URL parameter to "curl"
			brew.addParameter(curr->type); // Add the URL parameter to "curl"

			// if (curr->next == NULL) curr = NULL;
			// else curr = curr->next;

			curr = curr->next;
		}
	}
	if (publishers != NULL) {
		struct Publisher *curr = publishers;
		while(curr != NULL){
			if (_verbose) {
				Serial.print(F("Creating publishers: "));
				Serial.print(curr->name);
				Serial.print(F(" of type: "));
				Serial.println(curr->type);
			}
			brew.addParameter("-p"); // Add the URL parameter to "curl"
			brew.addParameter(curr->name); // Add the URL parameter to "curl"
			brew.addParameter(","); // Add the URL parameter to "curl"
			brew.addParameter(curr->type); // Add the URL parameter to "curl"

			curr = curr->next;
		}
	}

	Console.begin();
	if (_verbose) {
		Serial.println(F("Console started "));
	}

	brew.runAsynchronously();

	if (_verbose) {
		Serial.println(F("Brew started "));
	}
	while (!Console) { ; }
}

void SpacebrewYun::monitor() {

	// if not connected try to reconnect after appropriate interval
	if (_started && !_connected) {
		if ((millis() - connect_attempt) > connect_attempt_inter) {
			connect(server, port);
		}
	}

	// if message received from console, then process it
	while (Console.available() > 0) {
		char c = Console.read();

		if (c == char(CONNECTION_START) && _started && !_connected) {
			if (_verbose) {
				Serial.print(F("Connected to: "));
				Serial.println(server);
				Serial.print(F("Application name: "));
				Serial.println(name);
			}
			if (_onOpen != NULL){
				_onOpen();
			}
			_connected = true;
		} 	    

		else if (c == char(CONNECTION_END) && _connected) {
			_connected = false;
			if (_verbose) {
				Serial.print(F("Disconnected from: "));
				Serial.println(server);
			}
			if (_onClose != NULL){
				_onClose();
			}
		} 	 

		if (_verbose) {
			if (c == char(CONNECTION_ERROR)) {
				_error_msg = true;
				Serial.println(F("ERROR :: Spacebrew.py ::"));
			}
			else if (_error_msg && c == char(MSG_END)) {
				_error_msg = false;
				Serial.println();
			}
			if (_error_msg) {
			    Serial.print(c);			
			}
		}

		if (_connected) {
			// set flag to read data message name
			if (c == char(MSG_START)) {
				read_name = true;

			// set flag to read data message payload
			} else if (c == char(MSG_DIV) || sub_name.length() > sub_name_max) {
				read_name = false;
				read_msg = true;

			// set flag to read confirm message
			} else if (c == char(MSG_CONFIRM)) {
				read_confirm = true;

			// process data or confirm message, or reset message
			} else if (c == char(MSG_END) || sub_msg.length() > sub_msg_str_max) {
				if (read_msg == true) {
					onMessage();
				}
				if (read_confirm == true) {
					onConfirm();
					delay(2);
				}

				read_confirm = false;
				read_msg = false;
				sub_name = "";
				sub_msg = "";
				sub_type = "";

				// send a message received confirmation
				Console.print(char(7));

			// read message body
			} else {
				if (read_name == true) {
					sub_name += c;
				} else if (read_confirm == true) {
					sub_name += c;
				} else if (read_msg == true) {
					sub_msg += c;
				} 
				else if (_verbose) {
					Serial.print(c);
				}	    	
			}
		}
	}	

	// check if received confirmation that Linux received messages
	if (publishers != NULL && _connected) {
		struct Publisher *curr = publishers;
		while((curr != NULL)){

			if ( (curr->confirmed == 0) && ((millis() - curr->time) > 50) ) {
				if (_verbose) {
					Serial.print(F("resending msg: ")); 
					Serial.println(curr->name);
				} 
				send(curr->name, curr->lastMsg);
			}
			curr = curr->next;
		}
	}

	// Serial.println(F(" - END monitor"));

}

void SpacebrewYun::onConfirm() {
	if (publishers != NULL) {
		struct Publisher *curr = publishers;
		while((curr != NULL)){
			if (sub_name.equals(curr->name) == true) {
				curr->confirmed = true;
				break;
			}
			curr = curr->next;
		}
	}
}

boolean SpacebrewYun::connected() {
	return SpacebrewYun::_connected;
}

void SpacebrewYun::verbose(boolean verbose = true) {
	_verbose = verbose;
}

void SpacebrewYun::onMessage() {
	Serial.print(F("onMessage: name "));
	Serial.print(sub_name);
	Serial.print(F(", value "));
	Serial.print(sub_msg);

	if (subscribers != NULL && sub_name.equals("") == false) {
		struct Subscriber *curr = subscribers;
		while((curr != NULL) && (sub_type.equals("") == true)){
			if (sub_name.equals(curr->name) == true) {
				sub_type = curr->type;
			}
			curr = curr->next;
		}
	}

	Serial.print(F(", type "));
	Serial.println(sub_type);

	if ( sub_type.equals("range") ) {
		if (_onRangeMessage != NULL) {
			_onRangeMessage( sub_name, int(sub_msg.toInt()) );
		} else {
			Serial.println(F("ERROR :: Range message, no callback"));
		}
	} else if ( sub_type.equals("boolean") ) {
		if (_onBooleanMessage != NULL) {
			_onBooleanMessage( sub_name, ( sub_msg.equals("false") ? false : true ) );
		} else {
			Serial.println(F("ERROR :: Boolean message, no callback"));
		}
	} else if ( sub_type.equals("string") ) {
		if (_onStringMessage != NULL) {
			_onStringMessage( sub_name, sub_msg );	
		} else {
			Serial.println(F("ERROR :: String message, no callback"));
		}
	} else if ( sub_type.equals("custom") )  {
		if (_onCustomMessage != NULL) {
			_onCustomMessage( sub_name, sub_msg, sub_type );
		} else {
			Serial.println(F("ERROR :: Custom message, no callback"));
		}
	}

}


void SpacebrewYun::send(const String& name, const String& value){
	if (publishers != NULL) {

		Console.print(char(29));
		Console.print(name);
		Console.print(char(30));
		Console.print(value);
		Console.print(char(31));
		Console.flush();			

		struct Publisher *curr = publishers;
		while(curr != NULL){
			if (name.equals(curr->name) == true) {
				int msg_len = 0;

				if (curr->type == "range") msg_len = sub_msg_int_max;	
				else if (curr->type == "boolean") msg_len = sub_msg_bool_max;		
				else msg_len = sub_msg_str_max;		

				if (value.length() < msg_len) msg_len = value.length() + 1;
				value.toCharArray(curr->lastMsg, msg_len);

				curr->confirmed = false;
				curr->time = millis();

			}
			curr = curr->next;
		}	
	}	
}


/**
 * method that gets the pid of all spacebrew.py instances running on Linux.
 */
void SpacebrewYun::getPids() {

	// request the pid of all python processes
	// brew.begin("run-getsbpids"); // Process should launch the "curl" command
	pids.begin("python");
	pids.addParameter("/usr/lib/python2.7/spacebrew/getprocpid.py"); // Process should launch the "curl" command
	pids.run();

	if (_verbose) {
		Serial.println(F("Checking if spacebrew running"));
	}

	int sbPidsIndex = 0;
	int pidCharIndex = 0;
	char c = '\0';

	while ( pids.available() > 0 ) {

	    c = pids.read();

		if ( c >= '0' && c <= '9' ) {
			pid[pidCharIndex] = c;
			pidCharIndex = (pidCharIndex + 1) % pidLength;
		} 

		else if ( (c == ' ' || c == '\n') && pidCharIndex > 0) {
			sbPids[sbPidsIndex] = atoi(pid);
			if ( sbPidsIndex < (sbPidsLen - 1) ) sbPidsIndex = (sbPidsIndex + 1);    		

			for( int i = 0; i < pidLength; i++ ){ 
				pid[i] = '\0';
				pidCharIndex = 0;
			}
		}
	}
}

/**
 * method that kills all of the spacebrew.py instances that are running 
 * on Linux.
 */
void SpacebrewYun::killPids() {
	getPids();
	delay(400);

	for (int i = 0; i < sbPidsLen; i ++) {
		if (sbPids[i] > 0) {
			char * newPID = itoa(sbPids[i], pid, 10);

			if (_verbose) {
				Serial.print(F("Stopping existing processes: "));
				Serial.println(newPID);
			}

			Process p;
			p.begin("kill");
			p.addParameter("-9");
			p.addParameter(newPID);		// Process should launch the "curl" command
			p.run();            		// Run the process and wait for its termination	

			delay(400);						
		}
	}
}


