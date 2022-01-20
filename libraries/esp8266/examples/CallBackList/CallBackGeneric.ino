#include <Arduino.h>
#include <Ticker.h>
#include "CallBackList.h"

using namespace experimental::CBListImplentation;

class exampleClass {
  public:
    exampleClass() {};

    using exCallBack = std::function<void(int)>;
    using exHandler  = CallBackList<exCallBack>::CallBackHandler;

    CallBackList<exCallBack> myHandlers;

    exHandler setHandler(exCallBack cb) {
      return myHandlers.add(cb);
    }

    void removeHandler(exHandler hnd) {
      myHandlers.remove(hnd);
    }

    void trigger(int t) {
      myHandlers.execute(t);
    }
};

exampleClass myExample;

void cb1(int in) {
  Serial.printf("Callback 1, in = %d\n", in);
}

void cb2(int in) {
  Serial.printf("Callback 2, in = %d\n", in);
}

void cb3(int in, int s) {
  Serial.printf("Callback 3, in = %d, s = %d\n", in, s);
}

Ticker tk, tk2, tk3;
exampleClass::exHandler e1 = myExample.setHandler(cb1);
exampleClass::exHandler e2 = myExample.setHandler(cb2);
exampleClass::exHandler e3 = myExample.setHandler(std::bind(cb3, std::placeholders::_1, 10));

void setup() {
  Serial.begin(115200);

  tk.attach_ms(2000, []() {
    Serial.printf("trigger %d\n", (uint32_t)millis());
    myExample.trigger(millis());
  });
  tk2.once_ms(10000, []() {
    myExample.removeHandler(e2);
  });
  tk3.once_ms(20000, []() {
    e3.reset();
  });
}

void loop() {
}
