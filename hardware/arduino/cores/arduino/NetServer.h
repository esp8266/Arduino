#ifndef netserver_h
#define netserver_h

class NetClient;

class NetServer {
public:
  virtual void begin() =0;
};

#endif
