
#ifndef EEPROM_MOCK
#define EEPROM_MOCK

class EEPROMClass {
public:
  EEPROMClass(uint32_t sector);
  EEPROMClass(void);
  ~EEPROMClass();

  void begin(size_t size);
  uint8_t read(int address);
  void write(int address, uint8_t val);
  bool commit();
  void end();

  template<typename T> 
  T& get(int const address, T& t)
  {
    if (address < 0 || address + sizeof(T) > _size)
      return t;
    for (size_t i = 0; i < sizeof(T); i++)
        ((uint8_t*)&t)[i] = read(i);
    return t;
  }

  template<typename T> 
  const T& put(int const address, const T& t)
  {
    if (address < 0 || address + sizeof(T) > _size)
      return t;
    for (size_t i = 0; i < sizeof(T); i++)
        write(i, ((uint8_t*)&t)[i]);
    return t;
  }

  size_t length() { return _size; }

  //uint8_t& operator[](int const address) { return read(address); }
  uint8_t operator[] (int address) { return read(address); }

protected:
  size_t _size = 0;
  int _fd = -1;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
extern EEPROMClass EEPROM;
#endif

#endif
