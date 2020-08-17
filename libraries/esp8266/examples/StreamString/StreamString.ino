
#include <StreamDev.h>
#include <StreamString.h>

void loop() {
  delay(1000);
}

void checksketch(const char* what, const char* res1, const char* res2) {
  if (strcmp(res1, res2) == 0) {
    Serial << "PASSED: Test " << what << " (result: '" << res1 << "')\n";
  } else {
    Serial << "FAILED: Test " << what << ": '" << res1 << "' <> '" << res2 << "' !\n";
  }
}

#ifndef check
#define check(what, res1, res2) checksketch(what, res1, res2)
#endif

void testProgmem() {
  static const char inProgmem [] PROGMEM = "I am in progmem";
  auto inProgmem2 = F("I am too in progmem");

  int heap = (int)ESP.getFreeHeap();
  auto stream1 = StreamPtr(inProgmem, sizeof(inProgmem) - 1, true);
  auto stream2 = StreamPtr(inProgmem2);
  Serial << stream1 << " - " << stream2 << "\n";
  heap -= (int)ESP.getFreeHeap();
  check("NO heap occupation while streaming progmem strings", String(heap).c_str(), "0");
}

void testStream() {
  String inputString = "hello";
  StreamString result;

  {
    // use StreamString or S2Stream(String) to make a r/w Stream out of a String,
    // prefer the lighter StreamPtr(String) to make a read-only Stream out of a String

    result.clear();
    StreamPtr(inputString).toAll(result);
    StreamPtr(inputString).toAll(result);
    StreamPtr(inputString).toAll(result);
    check("StreamPtr.toAll(StreamString)", result.c_str(), "hellohellohello");
  }

  {
    // equivalent of the above

    result.clear();
    result << inputString;
    result << inputString << inputString;
    check("StreamString<<String", result.c_str(), "hellohellohello");
  }

  {
    // inputString made into a Stream
    // after input is loaded once, there's nothing to get from the stream
    // but the String is left untouched

    result.clear();
    S2Stream input(inputString);

    input.toAll(result);
    input.toAll(result);
    check("S2Stream.toAll(StreamString)", result.c_str(), "hello");
    check("unmodified String given to S2Stream", inputString.c_str(), "hello");
  }

  {
    // inputString made into a Stream, with an offset
    // after input is loaded once, there's nothing to get from the stream
    // but the String is left untouched

    result.clear();
    S2Stream input(inputString);
    // stream position set to offset 2 (0 by default)
    input.reset(2);

    input.toAll(result);
    input.toAll(result);
    check("S2Stream.reset(2):", result.c_str(), "llo");
  }

  {
    // inputString made into a Stream
    // reading the Stream consumes the String

    result.clear();
    S2Stream input(inputString);
    // reading stream will consume the string
    input.setConsume();

    input.toSize(result, 1);
    input.toSize(result, 2);
    check("setConsume(): S2Stream().toSize(StreamString,3)", result.c_str(), "hel");
    check("setConsume(): String given from S2Stream is swallowed", inputString.c_str(), "lo");
  }

  {
    StreamString cons(inputString);
    check("StreamString(String)", cons.c_str(), inputString.c_str());
  }
  {
    StreamString cons(result);
    check("StreamString(char*)", cons.c_str(), result.c_str());
  }
  {
    StreamString cons("abc");
    check("StreamString(char*)", cons.c_str(), "abc");
  }
  {
    StreamString cons(F("abc"));
    check("StreamString(F())", cons.c_str(), "abc");
  }
  {
    StreamString cons(23);
    check("StreamString(int)", cons.c_str(), "23");
  }
  {
    StreamString cons('a');
    check("StreamString(char)", cons.c_str(), "a");
  }
  {
    StreamString cons(23.2);
    check("StreamString(float)", cons.c_str(), "23.20");
  }

  {
    StreamString result("abc");
    result = inputString;
    check("StreamString = String", inputString.c_str(), result.c_str());

    StreamString ss2 = "abc";
    result = ss2;
    check("StreamString = StreamString", result.c_str(), ss2.c_str());

    result = "abc";
    check("StreamString = char*", result.c_str(), "abc");

    result = F("abc");
    check("StreamString = F()", result.c_str(), "abc");

    result = 23;
    check("StreamString = int", result.c_str(), "23");

    result = 'a';
    check("StreamString = char", result.c_str(), "a");

    result = 23.2;
    check("StreamString = float", result.c_str(), "23.20");
  }

#if !CORE_MOCK

  testProgmem();

  {
    int heap = (int)ESP.getFreeHeap();
    auto stream = StreamString(F("I am in progmem"));
    Serial << stream << "\n";
    heap -= (int)ESP.getFreeHeap();
    String heapStr(heap);
    if (heap != 0) {
      check("heap is occupied by String/StreamString(progmem)", heapStr.c_str(), heapStr.c_str());
    } else {
      check("ERROR: heap should be occupied by String/StreamString(progmem)", heapStr.c_str(), "-1");
    }
  }

  testProgmem();

#endif
}

#ifndef TEST_CASE

void setup() {
  Serial.begin(115200);
  delay(1000);

  testStream();

  Serial.printf("sizeof: String:%d Stream:%d StreamString:%d SStream:%d\n",
                (int)sizeof(String), (int)sizeof(Stream), (int)sizeof(StreamString), (int)sizeof(S2Stream));
}

#endif
