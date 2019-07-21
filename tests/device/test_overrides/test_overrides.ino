#include <BSTest.h>
BS_ENV_DECLARE();

ADC_MODE(ADC_VCC);
RF_MODE(RF_CAL);

static int rf_pre_init_flag = 0;

RF_PRE_INIT()
{
    rf_pre_init_flag = 42;
}

static unsigned setup_micros;

void setup()
{
    setup_micros = micros();
    Serial.begin(115200);
    BS_RUN(Serial);
}

bool pretest()
{
    return true;
}

TEST_CASE("ADC_MODE override works", "[core]")
{
    auto vcc = ESP.getVcc();
    Serial.printf("VCC: %d\r\n", vcc);
    Serial.printf("A0: %d\r\n", analogRead(A0));
    CHECK(vcc > 3000 && vcc < 3600);
}

TEST_CASE("RF_PRE_INIT override works", "[core]")
{
    CHECK(rf_pre_init_flag == 42);
}

void loop()
{
}
