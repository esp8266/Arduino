// Hint: On ESP8266, this no longer depends on FunctionalInterrupt, but is rather a feature of
// the standard API in Arduino.h

//#include <FunctionalInterrupt.h>

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
    Button(uint8_t reqPin) : PIN(reqPin) {
      pinMode(PIN, INPUT_PULLUP);
      attachInterrupt(PIN, std::bind(buttonIsr_static, this), FALLING);
    };
    ~Button() {
      detachInterrupt(PIN);
    }

    void IRAM_ATTR buttonIsr() {
      numberKeyPresses += 1;
      pressed = true;
    }

    static void IRAM_ATTR buttonIsr_static(void* const self) {
      static_cast<Button*>(self)->buttonIsr();
    }

    uint32_t checkPressed() {
      if (pressed) {
        Serial.printf("Button on pin %u has been pressed %u times\n", PIN, numberKeyPresses);
        pressed = false;
      }
      return numberKeyPresses;
    }

  private:
    const uint8_t PIN;
    volatile uint32_t numberKeyPresses = 0;
    volatile bool pressed = false;
};

Button* button1;
Button* button2;


void setup() {
  Serial.begin(115200);
  Serial.println("FunctionalInterrupt test/example");

  button1 = new Button(BUTTON1);
  button2 = new Button(BUTTON2);

  Serial.println("setup() complete");
}

void loop() {
  button1->checkPressed();
  if (nullptr != button2 && 10 < button2->checkPressed()) {
    delete button2;
    button2 = nullptr;
  }
}
