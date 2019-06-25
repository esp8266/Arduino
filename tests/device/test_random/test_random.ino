#include <BSTest.h>

BS_ENV_DECLARE();

void setup()
{
    Serial.begin(115200);
    BS_RUN(Serial);
}

bool pretest()
{
    return true;
}


TEST_CASE("If randomSeed is not called, random() uses hardware PRNG", "[random]")
{
    int32_t data[32];
    srand(10);
    for (int i = 0; i < sizeof(data)/sizeof(data[0]); ++i) {
        data[i] = random(0x7fffffff);
    }
    srand(10);
    for (int i = 0; i < sizeof(data)/sizeof(data[0]); ++i) {
        CHECK(random(0x7fffffff) != data[i]);
    }
}


TEST_CASE("If randomSeed is called, we get same random sequence every time", "[random]")
{
    const int32_t reference_sequence[] = {
        2104627054, 2013331137, 258660947, 107061148, 
        317460219, 663931879, 307161078, 1718702872, 
        1306951058, 1066376876, 624381721, 850811527, 
        329784053, 726742932, 182903521, 787925035, 
        1364123723, 198878220, 1117075042, 1108236242, 
        1775000610, 500470195, 896676389, 6341838, 
        785214762, 1084946248, 1601419914, 2058135092, 
        1671754873, 1952290050, 1572975837, 1596343802, 
        240941423, 1843946550, 793779187, 441773333, 
        884819086, 590861527, 1676358848, 2132930493, 
        969627641, 930717537, 195748182, 2064531490, 
        1195920216, 347502525, 584628940, 1938341337, 
        642503024, 915004020, 2034338438, 1690522669, 
        1805037441, 1904039418, 1491310300, 227517325, 
        17943876, 570537582, 1409581066, 1819703730, 
        730240988, 786466794, 1411137128, 1680096093, 
    };
    randomSeed(42);
    for (int i = 0; i < sizeof(reference_sequence)/sizeof(reference_sequence[0]); ++i) {
        CHECK(random(0x7fffffff) == reference_sequence[i]);
    }
}


void loop() {}
