/*
  Small math example, checking whether we can support various Arduino math operations.
  Meant for debugging **only**

  Released to public domain
*/

void setup() {
  Serial.begin(115200);

  // checking if we can support Arduino functions
  Serial.printf("abs(-3)\n = %d", abs(-3));
  Serial.printf("abs(+3)\n = %d", abs(3));
  Serial.printf("abs(-3.5)\n = %f", abs(-3.5));
  Serial.printf("abs(+3.5)\n = %f", abs(3.5));
  Serial.printf("round(2.9)\n = %f", round(2.9));
  Serial.printf("constrain(5, 1, 15) = %d\n", constrain(5, 1, 15));
  Serial.printf("constrain(16, 1, 15) = %d\n", constrain(16, 1, 15));

  // the same thing, but with c++ std::..., which should not cause any conflicts
  Serial.printf("std::abs(-3) = %d\n", std::abs(-3));
  Serial.printf("std::abs(+3) = %d\n", std::abs(3));
  Serial.printf("std::abs(-3.5) = %f\n", std::abs(-3.5));
  Serial.printf("std::abs(+3.5) = %f\n", std::abs(3.5));
  Serial.printf("std::round(2.9) = %f\n", std::round(2.9));
}

void loop() {
}

