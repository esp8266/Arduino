#include <ScheduledInterrupts.h>

#ifndef IRAM_ATTR
#define IRAM_ATTR ICACHE_RAM_ATTR
#endif

#if defined(ESP32)
#define BUTTON1 16
#define BUTTON2 17
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#define BUTTON1 D4
#define BUTTON2 D3
#else
#define BUTTON1 2
#define BUTTON2 0
#endif

class Button {
  public:
    Button(const uint8_t reqPin) : _PIN(reqPin) {
      pinMode(_PIN, INPUT_PULLUP);
      attachInterrupt(_PIN, std::bind(&Button::buttonIsr, this), FALLING);
    };
    ~Button() {
      detachInterrupt(_PIN);
    }

    void IRAM_ATTR buttonIsr() {
      _numberKeyPresses += 1;
      _pressed = true;
    }

    uint32_t testResetPressed() {
      if (_pressed) {
        Serial.printf("Button on pin %u has been pressed %u times\n", _PIN, _numberKeyPresses);
        _pressed = false;
      }
      return _numberKeyPresses;
    }

  private:
    const uint8_t _PIN;
    volatile uint32_t _numberKeyPresses = 0;
    volatile bool _pressed = false;
};

// Pointers and "new" in setup() are used in this example to simply test
// and demonstrate how an ISR object can be constructed and destructed at runtime,
// including the detach of the ISR from the GPIO.
Button* button1 = nullptr;
Button* button2 = nullptr;

void setup() {
  Serial.begin(115200);
  Serial.println("ScheduledInterrupts test/example");

  button1 = new Button(BUTTON1);
  button2 = new Button(BUTTON2);

  Serial.println("setup() complete");
}

void loop() {
  button1->testResetPressed();
  if (nullptr != button2 && 10 < button2->testResetPressed()) {
    delete button2;
    button2 = nullptr;
  }
}
