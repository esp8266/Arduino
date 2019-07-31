
// show arduino_new benefits
// released to public domain
// result is below

class SomeClass {
  public:
    SomeClass(const String& s1 = emptyString, const String& s2 = emptyString) {
      Serial.printf("SomeClass@%p(%s)(%s)\n", this, s1.c_str(), s2.c_str());
    }

    ~SomeClass() {
      Serial.printf("~ SomeClass @%p\n", this);
    }
};

class oom {
  private:
    char large [65000];
  public:
    oom() {
      Serial.printf("this constructor should not be called\n");
    }
};

void setup() {

  Serial.begin(115200);
  Serial.printf("\n\narduino_new benefits\n\n");
  delay(5000); // avoid too frequent bootloop

  // arduino_new / arduino_newarray api

  Serial.printf("\n----- arduino_new:\n");
  auto an = arduino_new(SomeClass);
  delete an;

  Serial.printf("\n----- arduino_new with oom:\n");
  auto anoom = arduino_new(oom);
  Serial.printf("nullptr: %p\n", anoom);
  delete anoom;

  Serial.printf("\n----- arduino_new with constructor parameters:\n");
  auto ancp = arduino_new(SomeClass, "param1", "param2");
  delete ancp;

  Serial.printf("\n----- arduino_newarray[2]\n");
  auto ana2 = arduino_newarray(SomeClass, 2);
  Serial.printf("@:%p s=%zd s(2)=%zd\n", ana2, sizeof(SomeClass), sizeof(SomeClass[2]));
  Serial.printf("0: %p\n", &ana2[0]);
  Serial.printf("1: %p\n", &ana2[1]);
  delete [] ana2;

  Serial.printf("\n----- arduino_newarray[2] (with constructor parameters)\n");
  auto ana2cp = arduino_newarray(SomeClass, 2, "param1");
  Serial.printf("@:%p s=%zd s(2)=%zd\n", ana2cp, sizeof(SomeClass), sizeof(SomeClass[2]));
  Serial.printf("0: %p\n", &ana2cp[0]);
  Serial.printf("1: %p\n", &ana2cp[1]);
  delete [] ana2cp;

  Serial.printf("\n----- arduino_newarray[100000]\n");
  auto anaX = arduino_newarray(SomeClass, 100000);
  Serial.printf("@:%p\n", anaX);

  // standard c++ api for new and new[]

  Serial.printf("\n----- new\n");
  auto sn = new SomeClass;
  delete sn;

  Serial.printf("\n----- new with oom: (abort() with option 'Exceptions: Disabled (new can abort)'\n");
  auto snoom = new oom;
  Serial.printf("nullptr: %p\n", snoom);
  delete snoom;

  Serial.printf("\n----- new[2]\n");
  auto sna2 = new SomeClass[2];
  Serial.printf("@:%p s=%zd s(2)=%zd\n", sna2, sizeof(SomeClass), sizeof(SomeClass[2]));
  Serial.printf("0: %p\n", &sna2[0]);
  Serial.printf("1: %p\n", &sna2[1]);
  delete [] sna2;

  Serial.printf("\n----- new[10000] (badly fails with 'Exceptions: Legacy' or '...Disabled'\n");
  auto snaX = new SomeClass[100000];
  Serial.printf("@:%p\n", snaX);
}

void loop() {
}

//////////////////////////////
#if 0

Result with:
Exceptions: Legacy(new can return nullptr)

//////////////////////////////

arduino_new benefits

---- - arduino_new:
SomeClass@0x3fff1864()()
~ SomeClass @0x3fff1864

---- - arduino_new with oom:
nullptr: 0

---- - arduino_new with constructor parameters:
SomeClass@0x3fff1864(param1)(param2)
~ SomeClass @0x3fff1864

---- - arduino_newarray[2]
SomeClass@0x3fff1868()()
SomeClass@0x3fff1869()()
@: 0x3fff1868 s = 1 s(2) = 2
                           0: 0x3fff1868
                           1: 0x3fff1869
                           ~ SomeClass @0x3fff1869
                           ~ SomeClass @0x3fff1868

                           ---- - arduino_newarray[2](with constructor parameters)
                           SomeClass@0x3fff1868(param1)()
                           SomeClass@0x3fff1869(param1)()
                           @: 0x3fff1868 s = 1 s(2) = 2
                               0: 0x3fff1868
                               1: 0x3fff1869
                               ~ SomeClass @0x3fff1869
                               ~ SomeClass @0x3fff1868

                               ---- - arduino_newarray[100000]
                               @: 0

                               ---- - new
                               SomeClass@0x3fff1864()()
                               ~ SomeClass @0x3fff1864

                               ----- new with oom: (abort() with option 'Exceptions: Disabled (new can abort)'
                                   this constructor should not be called
                                   nullptr: 0

                                   ---- - new[2]
                                   SomeClass@0x3fff1868()()
                                   SomeClass@0x3fff1869()()
                                   @:0x3fff1868 s = 1 s(2) = 2
                                       0: 0x3fff1868
                                       1: 0x3fff1869
                                       ~ SomeClass @0x3fff1869
                                       ~ SomeClass @0x3fff1868

                                       ---- - new[10000](badly fails with 'Exceptions: Legacy' or '...Disabled'

                                           Exception(29):
                                           epc1 = 0x402013de epc2 = 0x00000000 epc3 = 0x00000000 excvaddr = 0x00000000 depc = 0x00000000

                                               >>> stack >>>

                                               ...
                                               //////////////////////////////

#endif
