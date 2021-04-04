
uint32_t cyclesToRead1Kx32(unsigned int *x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += *(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite1Kx32(unsigned int *x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += i;
    *(x++) = sum;
  }
  return ESP.getCycleCount() - b;
}


uint32_t cyclesToRead1Kx16(unsigned short *x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += *(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite1Kx16(unsigned short *x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += i;
    *(x++) = sum;
  }
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToRead1Kx8(unsigned char*x, uint32_t *res) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += *(x++);
  }
  *res = sum;
  return ESP.getCycleCount() - b;
}

uint32_t cyclesToWrite1Kx8(unsigned char*x) {
  uint32_t b = ESP.getCycleCount();
  uint32_t sum = 0;
  for (int i = 0; i < 1024; i++) {
    sum += i;
    *(x++) = sum;
  }
  return ESP.getCycleCount() - b;
}

void setup() {
  Serial.begin(115200);
  Serial.printf("\n");

  // Enabling VM does not change malloc to use the external region.  It will continue to
  // use the normal RAM until we request otherwise.
  uint32_t *mem = (uint32_t *)malloc(1024 * sizeof(uint32_t));
  Serial.printf("Internal buffer: Address %p, free %d\n", mem, ESP.getFreeHeap());

  // Now request from the VM heap
  ESP.setExternalHeap();
  uint32_t *vm = (uint32_t *)malloc(1024 * sizeof(uint32_t));
  Serial.printf("External buffer: Address %p, free %d\n", vm, ESP.getFreeHeap());
  // Make sure we go back to the internal heap for other allocations.  Don't forget to ESP.resetHeap()!
  ESP.resetHeap();

  uint32_t res;
  uint32_t t;
  t = cyclesToWrite1Kx32(vm);
  Serial.printf("Virtual Memory Write:   %d cycles for 4K\n", t);
  t = cyclesToWrite1Kx32(mem);
  Serial.printf("Physical Memory Write:  %d cycles for 4K\n", t);

  t = cyclesToRead1Kx32(vm, &res);
  Serial.printf("Virtual Memory Read:   %d cycles for 4K (sum %08x)\n", t, res);
  t = cyclesToRead1Kx32(mem, &res);
  Serial.printf("Physical Memory Read:  %d cycles for 4K (sum %08x)\n", t, res);

  t = cyclesToWrite1Kx16((uint16_t*)vm);
  Serial.printf("Virtual Memory Write:   %d cycles for 2K by 16\n", t);
  t = cyclesToWrite1Kx16((uint16_t*)mem);
  Serial.printf("Physical Memory Write:  %d cycles for 2K by 16\n", t);

  t = cyclesToRead1Kx16((uint16_t*)vm, &res);
  Serial.printf("Virtual Memory Read:   %d cycles for 2K by 16 (sum %08x)\n", t, res);
  t = cyclesToRead1Kx16((uint16_t*)mem, &res);
  Serial.printf("Physical Memory Read:  %d cycles for 2K by 16 (sum %08x)\n", t, res);

  t = cyclesToWrite1Kx8((uint8_t*)vm);
  Serial.printf("Virtual Memory Write:   %d cycles for 1K by 8\n", t);
  t = cyclesToWrite1Kx8((uint8_t*)mem);
  Serial.printf("Physical Memory Write:  %d cycles for 1K by 8\n", t);

  t = cyclesToRead1Kx8((uint8_t*)vm, &res);
  Serial.printf("Virtual Memory Read:   %d cycles for 1K by 8 (sum %08x)\n", t, res);
  t = cyclesToRead1Kx8((uint8_t*)mem, &res);
  Serial.printf("Physical Memory Read:  %d cycles for 1K by 8 (sum %08x)\n", t, res);

  // Let's use external heap to make a big ole' String
  ESP.setExternalHeap();
  String s = "";
  for (int i = 0; i < 100; i++) {
    s += i;
    s += ' ';
  }
  ESP.resetHeap();
  Serial.printf("Internal free: %d\n", ESP.getFreeHeap());
  ESP.setExternalHeap();
  Serial.printf("External free: %d\n", ESP.getFreeHeap());
  ESP.resetHeap();
  Serial.printf("String: %s\n", s.c_str());

  // Note that free/realloc will all use the heap specified when the pointer was created.
  // No need to change heaps to delete an object, only to create it.
  free(vm);
  free(mem);

  Serial.printf("Internal free: %d\n", ESP.getFreeHeap());
  ESP.setExternalHeap();
  Serial.printf("External free: %d\n", ESP.getFreeHeap());
  ESP.resetHeap();
}

void loop() {

}
