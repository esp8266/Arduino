
#ifndef __STREAMDEV_H
#define __STREAMDEV_H

#include <Stream.h>

///////////////////////////////////////////////
// - black hole in, swallow everything, availableForWrite = infinite
// - black hole out, nothing to read, available = 0

class StreamNull: public Stream
{
public:

    // Print
    virtual size_t write(uint8_t) override { return 1; }
    virtual size_t write(const uint8_t* buffer, size_t size) override { (void)buffer; return size; }
    virtual int availableForWrite() override { return 32767; }

    // Stream
    virtual int available() override { return 0; }
    virtual int read() override { return -1; }
    virtual int peek() override { return -1; }
    virtual size_t readBytes(char* buffer, size_t len) override { (void)buffer; (void)len; return 0; }
    virtual int readNow(char* buffer, size_t len) override { (void)buffer; (void)len; return 0; }
};

///////////////////////////////////////////////
// /dev/zero
// - black hole in, swallow everything, availableForWrite = infinite
// - white hole out, gives infinity to read, available = infinite

class StreamZero: public StreamNull
{
protected:
    char _x;
public:
    StreamZero (char x = 0): _x(x) { }

    // Stream
    virtual int available() override { return 32767; }
    virtual int read() override { return _x; }
    virtual int peek() override { return _x; }
    virtual size_t readBytes(char* buffer, size_t len) override { memset(buffer, _x, len); return len; }
    virtual int readNow(char* buffer, size_t len) override { memset(buffer, _x, len); return len; }
};

///////////////////////////////////////////////
// static buffer (in flash or ram)
// - black hole in, swallow everything, availableForWrite = infinite
// - Stream buffer out

class StreamPtr: public StreamNull
{
protected:
    const char* _buffer;
    size_t _size;
    bool _in_flash;
    size_t _peekPointer = 0;
    
public:
    StreamPtr (const char* buffer, size_t size, bool in_flash = false): _buffer(buffer), _size(size), _in_flash(in_flash) { }
    StreamPtr (const uint8_t* buffer, size_t size, bool in_flash = false): _buffer((const char*)buffer), _size(size), _in_flash(in_flash) { }
    StreamPtr (const __FlashStringHelper* buffer, size_t size): _buffer(reinterpret_cast<const char*>(buffer)), _size(size), _in_flash(true) { }

    void peekPointerReset (int pointer = 0) { _peekPointer = pointer; }

    // Stream
    virtual int available() override { return availableForPeek(); }
    virtual int read() override { return _peekPointer < _size? _buffer[_peekPointer++]: -1; }
    virtual int peek() override { return _peekPointer < _size? _buffer[_peekPointer]: -1; }
    virtual size_t readBytes(char* buffer, size_t len) override
    {
        if (_peekPointer < _size)
        {
            size_t cpylen = std::min(_size - _peekPointer, len);
            if (_in_flash)
                memcpy_P(buffer, _buffer + _peekPointer, cpylen);
            else
                memcpy(buffer, _buffer + _peekPointer, cpylen);
            _peekPointer += cpylen;
            return cpylen;
        }
        return 0;
    }
    virtual int readNow(char* buffer, size_t len) override { return readBytes(buffer, len); }

    // peekBuffer
    virtual bool peekBufferAPI () const override { return !_in_flash; }
    virtual size_t availableForPeek () override { return _peekPointer < _size? _size - _peekPointer: 0; }
    virtual const char* peekBuffer () { return _peekPointer < _size? _buffer + _peekPointer: nullptr; }
    virtual void peekConsume (size_t consume) { _peekPointer += consume; }
};

///////////////////////////////////////////////
#endif // __STREAMDEV_H
