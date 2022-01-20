
// this example sketch in the public domain is also a host and device test

#include <StreamDev.h>
#include <StreamString.h>

void loop()
{
  delay(1000);
}

void checksketch(const char* what, const char* res1, const char* res2)
{
  if (strcmp(res1, res2) == 0)
  {
    Serial << "PASSED: Test " << what << " (result: '" << res1 << "')\n";
  }
  else
  {
    Serial << "FAILED: Test " << what << ": '" << res1 << "' <> '" << res2 << "' !\n";
  }
}

#ifndef check
#define check(what, res1, res2) checksketch(what, res1, res2)
#endif

void testStringPtrProgmem()
{
  static const char inProgmem[] PROGMEM = "I am in progmem";
  auto              inProgmem2          = F("I am too in progmem");

  int               heap                = (int)ESP.getFreeHeap();
  auto              stream1             = StreamConstPtr(inProgmem, sizeof(inProgmem) - 1);
  auto              stream2             = StreamConstPtr(inProgmem2);
  Serial << stream1 << " - " << stream2 << "\n";
  heap -= (int)ESP.getFreeHeap();
  check("NO heap occupation while streaming progmem strings", String(heap).c_str(), "0");
}

void testStreamString()
{
  String       inputString = "hello";
  StreamString result;

  // By default, reading a S2Stream(String) or a StreamString will consume the String.
  // It can be disabled by calling ::resetPointer(), (not default)
  // and re-enabled by calling ::setConsume(). (default)
  //
  // In default consume mode, reading a byte or a block will remove it from
  // the String.  Operations are O(n²).
  //
  // In non-default non-consume mode, it will just move a pointer.  That one
  // can be ::resetPointer(pos) anytime.  See the example below.

  // The String included in 'result' will not be modified by read:
  // (this is not the default)
  result.resetPointer();

  {
    // We use a a lighter StreamConstPtr(input) to make a read-only Stream out of
    // a String that obviously should not be modified during the time the
    // StreamConstPtr instance is used.  It is used as a source to be sent to
    // 'result'.

    result.clear();
    StreamConstPtr(inputString).sendAll(result);
    StreamConstPtr(inputString).sendAll(result);
    StreamConstPtr(inputString).sendAll(result);
    check("StreamConstPtr.sendAll(StreamString)", result.c_str(), "hellohellohello");
  }

  {
    // equivalent of the above

    result.clear();
    result << inputString;
    result << inputString << inputString;
    check("StreamString<<String", result.c_str(), "hellohellohello");
  }

  {
    // Now inputString is made into a Stream using S2Stream,
    // and set in non-consume mode (using ::resetPointer()).

    // Then, after that input is read once, it won't be anymore readable
    // until the pointer is reset.

    S2Stream input(inputString);
    input.resetPointer();

    result.clear();
    input.sendAll(result);
    input.sendAll(result);
    check("S2Stream.sendAll(StreamString)", result.c_str(), "hello");
    check("unmodified String given to S2Stream", inputString.c_str(), "hello");
  }

  {
    // Same as above, with an offset

    result.clear();
    S2Stream input(inputString);
    // stream position set to offset 2 (0 by default)
    input.resetPointer(2);

    input.sendAll(result);
    input.sendAll(result);
    check("S2Stream.resetPointer(2):", result.c_str(), "llo");
  }

  {
    // inputString made into a Stream
    // reading the Stream consumes the String

    result.clear();
    S2Stream input(inputString);
    // reading stream will consume the string
    input.setConsume();  // can be omitted, this is the default

    input.sendSize(result, 1);
    input.sendSize(result, 2);
    check("setConsume(): S2Stream().sendSize(StreamString,3)", result.c_str(), "hel");
    check("setConsume(): String given from S2Stream is swallowed", inputString.c_str(), "lo");
  }

  // Streaming with common String constructors
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

#if !CORE_MOCK

  // A progmem won't use Heap when StringPtr is used
  testStringPtrProgmem();

  // .. but it does when S2Stream or StreamString is used
  {
    int  heap   = (int)ESP.getFreeHeap();
    auto stream = StreamString(F("I am in progmem"));
    Serial << stream << "\n";
    heap -= (int)ESP.getFreeHeap();
    String heapStr(heap);
    if (heap != 0)
    {
      check("heap is occupied by String/StreamString(progmem)", heapStr.c_str(), heapStr.c_str());
    }
    else
    {
      check("ERROR: heap should be occupied by String/StreamString(progmem)", heapStr.c_str(), "-1");
    }
  }

  // (check again to be sure)
  testStringPtrProgmem();

#endif
}

#ifndef TEST_CASE

void setup()
{
  Serial.begin(115200);
  delay(1000);

  testStreamString();

  Serial.printf("sizeof: String:%d Stream:%d StreamString:%d SStream:%d\n",
                (int)sizeof(String), (int)sizeof(Stream), (int)sizeof(StreamString), (int)sizeof(S2Stream));
}

#endif
