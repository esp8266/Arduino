
#ifndef YUNSPACEBREW_H
#define YUNSPACEBREW_H

#include "Arduino.h"
#include <Bridge.h>
#include <Console.h>
#include <Process.h>

enum SBmsg { 
	CONNECTION_START = char(28), 
	CONNECTION_END = char(27), 
	CONNECTION_ERROR = char(26), 
	MSG_CONFIRM = char(7), 
	MSG_START = char(29), 
	MSG_DIV = char(30), 
	MSG_END = char(31) 
};

struct Publisher {
	char *name;
	char *type;
	char *lastMsg;
	Publisher *next;
	int confirmed;
	long time;
};

struct Subscriber{
	char *name;
	char *type;
	Subscriber *next;
};

int const pidLength = 6;
int const sbPidsLen = 4;

class SpacebrewYun {

	public:

		SpacebrewYun(const String&, const String&);
		void addPublish(const String&, const String&);
		void addSubscribe(const String&, const String&);

		void connect(String, int);
		void connect() { connect(server, port); };
		void connect(String _server) { connect(String(_server), port); };

		void monitor();
		void onMessage();
		void onConfirm();

		boolean connected();

		void send(const String&, const String&);
		void send(const String& name, char * value) { send(name, String(value)); }
		void send(const String& name, bool value){ send(name, (value ? String("true") : String("false"))); };
		void send(const String& name, int value) { send(name, String(value)); };
		void send(const String& name, long value) { send(name, String(value)); };
		void send(const String& name, float value) { send(name, String(value)); };

		void verbose(boolean);

		typedef void (*OnBooleanMessage)(String name, boolean value);
		typedef void (*OnRangeMessage)(String name, int value);
		typedef void (*OnStringMessage)(String name, String value);
		typedef void (*OnCustomMessage)(String name, String value, String type);
		typedef void (*OnSBOpen)();
		typedef void (*OnSBClose)();
		typedef void (*OnSBError)(int code, String message);

		void onOpen(OnSBOpen function);
		void onClose(OnSBClose function);
		void onRangeMessage(OnRangeMessage function);
		void onStringMessage(OnStringMessage function);
		void onBooleanMessage(OnBooleanMessage function);
		void onCustomMessage(OnCustomMessage function);
		void onError(OnSBError function);

	private:

		Process brew;
		String name;
		String server;
		String description;
		boolean _started;
		boolean _connected;
		boolean _error_msg;
		boolean _verbose;
		int port;

		/**Output should be at least 5 cells**/
		static OnBooleanMessage _onBooleanMessage;
		static OnRangeMessage _onRangeMessage;
		static OnStringMessage _onStringMessage;
		static OnCustomMessage _onCustomMessage;
		static OnSBOpen _onOpen;
		static OnSBClose _onClose;
		static OnSBError _onError;

		Subscriber * subscribers;
		Publisher * publishers;
		String sub_name;
		String sub_msg;
		String sub_type;

		boolean read_name;
		boolean read_msg;
		boolean read_confirm;
		static int sub_name_max;
		static int sub_msg_str_max;
		static int sub_msg_int_max;
		static int sub_msg_bool_max;

		long connect_attempt;
		int connect_attempt_inter;

		Process pids;
		char pid [6];
		int sbPids [4];

		void killPids();
		void getPids();

		static char * createString(int len){
			char * out = ( char * ) malloc ( len + 1 );
			return out;
		}		

		static void emptyString(char * str, int len){
			for (int i = 0; i < len; i++) {
				str[i] = '\0';
			}
		}

};

#endif
