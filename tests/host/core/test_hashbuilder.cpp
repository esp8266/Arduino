/*
 test_hashBuilder.cpp - HashBuilder tests
 Copyright © 2017 Sven Eliasson

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 */

#include <catch.hpp>
#include <HashBuilder.h>
#include <iostream>
#include <chrono>
#include <StreamString.h>


TEST_CASE("HashLib: can be instantiated"){
  class MockBuilder: public HashBuilder{
    void _init(void){ return;}
    void _update(uint8_t * data, uint16_t len){ return;}
    void _final(){ return; }
    std::vector<uint8_t> &_result(){ return resultVec; }
    std::vector<uint8_t> resultVec = {} ;
  public:
    bool ok(void){ return true;}
  };
  MockBuilder t;
  REQUIRE(t.ok());
}


TEST_CASE("HashLib: _byteVecToString Method"){
  class MockBuilder: public HashBuilder{
    void _init(void){ return;}
    void _update(uint8_t * data, uint16_t len){ return;}
    void _final(){ return; }
    std::vector<uint8_t> &_result(){ return resultVec; }
    std::vector<uint8_t> resultVec = {209, 86, 166, 229};
  public:
    String test_byteVecToString( std::vector<uint8_t> &res){
      return _byteVecToString(res);
    }
  };

  MockBuilder t;
  std::vector<uint8_t> v = {80, 120, 0, 93, 47};
  const String expectedHashString = "5078005d2f";
  REQUIRE( t.test_byteVecToString(v) == expectedHashString);
}

TEST_CASE("HashLib: toString Method"){
  class MockBuilder: public HashBuilder{
    void _init(void){ return;}
    void _update(uint8_t * data, uint16_t len){ return;}
    void _final(){ return; }
    std::vector<uint8_t> &_result(){ return resultVec; }
    std::vector<uint8_t> resultVec = {209, 86, 166, 229};
  };

  MockBuilder t;
  const String falseHexString = "5078005d2f";
  const String expectedResult = "d156a6e5";
  REQUIRE( t.toString() == expectedResult);
  REQUIRE_FALSE( t.toString() == falseHexString );
}

TEST_CASE("HashLib: getChars"){
  class MockBuilder: public HashBuilder{
    void _init(void){ return;}
    void _update(uint8_t * data, uint16_t len){ return;}
    void _final(){ return; }
    std::vector<uint8_t> &_result(){ return resultVec; }
    std::vector<uint8_t> resultVec = {209, 86, 166, 229};
  };
  MockBuilder t;
  char buff[100];
  t.getChars(buff);
  const String expectedResult = "d156a6e5";
  String res = String(buff);
  REQUIRE(res ==  expectedResult);
}


TEST_CASE("HashLib: getBytes"){
  class MockBuilder: public HashBuilder{
    void _init(void){ return;}
    void _update(uint8_t * data, uint16_t len){ return;}
    void _final(){ return; }
    std::vector<uint8_t> &_result(){ return resultVec; }
  public:
    std::vector<uint8_t> resultVec = {209, 86, 166, 229};
  };
  const String expectedResult = "d156a6e5";

  MockBuilder t;
  uint8_t buff[100];
  t.getBytes(buff);
  for( int it = 0; it < 4 ; it++){
    REQUIRE( buff[it] == t.resultVec[it] );
  }
}

TEST_CASE("HashLib: _hexCharToByte"){
  class MockBuilder: public HashBuilder{
    void _init(void){ return;}
    void _update(uint8_t * data, uint16_t len){ return;}
    void _final(){ return; }
    std::vector<uint8_t> &_result(){ return resultVec; }
  public:
    std::vector<uint8_t> resultVec = {209, 86, 166, 229};
    uint8_t test_hexCharToByte(uint8_t c){
      return _hexCharToByte(c);
    };
  };
  const char hex[] = "0123456789abcdef";
  MockBuilder t;
  for(int i = 0; i < 16; i++){
    REQUIRE( t.test_hexCharToByte( hex[i] ) == i );
  }
}

TEST_CASE("HashLib: addHexString"){
  class MockBuilder: public HashBuilder{
    void _init(void){ return;}
    void _update(uint8_t * data, uint16_t len){
      for( int i =0; i < len; i++){
        payloadDump.push_back(data[i]);
      }
    }
    void _final(){ return; }
    std::vector<uint8_t> &_result(){ return resultVec; }

  public:
    std::vector<uint8_t> payloadDump;
    String returnPayloadDumpAsHex( ){
      return _byteVecToString(payloadDump);
    }
    std::vector<uint8_t> resultVec = {209, 86, 166, 229};
  };

  MockBuilder t;
  SECTION("Using a c-string"){
    char payload[] = "1234567890abcdeffedcba98765432106469676974616c7369676e61747572656170706c69636174696f6e73";
    t.addHexString(payload);
    auto str = t.returnPayloadDumpAsHex();
    REQUIRE( str == payload  );
  }

  SECTION("Using a String"){
    const String payload = "1234567890abcdeffedcba98765432106469676974616c7369676e61747572656170706c69636174696f6e73";
    t.addHexString(payload);
    auto str = t.returnPayloadDumpAsHex();
    REQUIRE( str == payload  );
  }
}


  TEST_CASE("HashLib: addStream"){
  class MockBuilder: public HashBuilder{
    void _init(void){ return;}
    void _update(uint8_t * data, uint16_t len){
      for( int i =0; i < len; i++){
        payloadDump.push_back(data[i]);
      }
    }
    void _final(){ return; }
    std::vector<uint8_t> &_result(){ return resultVec; }

  public:
    std::vector<uint8_t> resultVec = {209, 86, 166, 229};
    std::vector<uint8_t> payloadDump;
  };

  MockBuilder t;
  const char* payload = "HASHLIB::addStream_works_longlonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglong";
  StreamString stream;
  stream.print(payload);
  t.addStream(stream, stream.available());

  String reRead = "";
  for( auto c : t.payloadDump ){
    reRead += char(c);
  }
  REQUIRE( reRead == String(payload)  );

}
