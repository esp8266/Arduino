// test that we can include umm_malloc.h from sketch (#1652)
#include <umm_malloc/umm_malloc.h>


void setup() {
    Serial.begin(115200);
    delay(1000);
    umm_info(NULL, 1);
}

void loop() {

}
