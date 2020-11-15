#pragma once

#include <FS.h>
#include <memory>
#include <functional>
/*

    A temporary FS is made between the END of the sketch...  and the start of the partition you try to mount, to maximise the available space for copying the FS.
    The WORST case this is at 0x40300000 which is for a 3m FS on 4m flash..  leaving 460Kb for copying.

*/



namespace FST
{

    struct layout
    {
        constexpr layout(uint32_t s, uint32_t e, uint32_t p, uint32_t b) : startAddr(s), endAddr(e), page(p), block(b) {};
        const uint32_t startAddr{0};
        const uint32_t endAddr{0};
        const uint32_t page{0};
        const uint32_t block{0};
    };

    enum FS_t : uint8_t
    {
        SPIFFS,
        LITTLEFS
    };

    static constexpr layout layout_512k32  = { 0x40273000, 0x4027B000, 0x100, 0x1000 };
    static constexpr layout layout_512k64  = { 0x4026B000, 0x4027B000, 0x100, 0x1000 };
    static constexpr layout layout_512k128 = { 0x4025B000, 0x4027B000, 0x100, 0x1000 };

    static constexpr layout layout_1m64    = { 0x402EB000, 0x402FB000, 0x100, 0x1000 };
    static constexpr layout layout_1m128   = { 0x402DB000, 0x402FB000, 0x100, 0x1000 };
    static constexpr layout layout_1m144   = { 0x402D7000, 0x402FB000, 0x100, 0x1000 };
    static constexpr layout layout_1m160   = { 0x402D3000, 0x402FB000, 0x100, 0x1000 };
    static constexpr layout layout_1m192   = { 0x402CB000, 0x402FB000, 0x100, 0x1000 };
    static constexpr layout layout_1m256   = { 0x402BB000, 0x402FB000, 0x100, 0x1000 };
    static constexpr layout layout_1m512   = { 0x4027B000, 0x402FB000, 0x100, 0x2000 };

    static constexpr layout layout_2m64    = { 0x403F0000, 0x403FB000, 0x100, 0x1000 };
    static constexpr layout layout_2m128   = { 0x403E0000, 0x403FB000, 0x100, 0x1000 };
    static constexpr layout layout_2m256   = { 0x403C0000, 0x403FB000, 0x100, 0x1000 };
    static constexpr layout layout_2m512   = { 0x40380000, 0x403FA000, 0x100, 0x2000 };
    static constexpr layout layout_2m1m    = { 0x40300000, 0x403FA000, 0x100, 0x2000 };

    static constexpr layout layout_4m1m    = { 0x40500000, 0x405FA000, 0x100, 0x2000 };
    static constexpr layout layout_4m2m    = { 0x40400000, 0x405FA000, 0x100, 0x2000 };
    static constexpr layout layout_4m3m    = { 0x40300000, 0x405FA000, 0x100, 0x2000 };

    static constexpr layout layout_8m6m    = { 0x40400000, 0x409FA000, 0x100, 0x2000 };
    static constexpr layout layout_8m7m    = { 0x40300000, 0x409FA000, 0x100, 0x2000 };

    static constexpr layout layout_16m14m  = { 0x40400000, 0x411FA000, 0x100, 0x2000 };
    static constexpr layout layout_16m15m  = { 0x40300000, 0x411FA000, 0x100, 0x2000 };

    typedef std::function<void(File & f)> FileCb;

};


class FSTools
{
public:

    FSTools();
    ~FSTools();
    bool attemptToMountFS(fs::FS & fs);
    bool mountAlternativeFS(FST::FS_t type, const FST::layout & layout, bool keepMounted = false);
    bool mounted();
    bool moveFS(fs::FS & destinationFS);
    void reset();
    void fileListIterator(FS & fs, const char * dirName, FST::FileCb Cb);

private:
    uint32_t _getStartAddr(const FST::layout & layout);
    uint32_t _getSize(const FST::layout & layout);
#ifdef DEBUG_ESP_CORE
    void _dumpFileInfo(File & f);
#endif
    bool _copyFS(FS & sourceFS, FS & destFS);

    std::unique_ptr<fs::FS> _pFS;
    bool _mounted{false};
    const FST::layout * _layout{nullptr};

};