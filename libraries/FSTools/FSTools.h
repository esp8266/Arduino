#pragma once

#include <FS.h> 
#include <memory>
#include <functional>
/*
 
	A temporary FS is made between the END of the sketch...  and the start of the partition you try to mount, to maximise the available space for copying the FS. 
	The WORST case this is at 0x40300000 which is for a 3m FS on 4m flash..  leaving 460Kb for copying. 


	I have not worked out a way to prevent linking of ALL the layouts except to use #ifdef guards.  Ideas welcome as it uses 400B ROM. 
 */

namespace FST {

struct layout {
  layout(uint32_t s, uint32_t e, uint32_t p, uint32_t b) : startAddr(s), endAddr(e), page(p), block(b) {};
  uint32_t startAddr{0};
  uint32_t endAddr{0};
  uint32_t page{0};
  uint32_t block{0};
};

enum FS_t : uint8_t {
	SPIFFS,
	LITTLEFS
};

extern const layout layout_512k32;
extern const layout layout_512k64;
extern const layout layout_512k128;

extern const layout layout_1m64;
extern const layout layout_1m128;
extern const layout layout_1m144;
extern const layout layout_1m160;
extern const layout layout_1m192;
extern const layout layout_1m256;
extern const layout layout_1m512;

extern const layout layout_2m64;
extern const layout layout_2m128;
extern const layout layout_2m256;
extern const layout layout_2m512;
extern const layout layout_2m1m;

extern const layout layout_4m1m;
extern const layout layout_4m2m;
extern const layout layout_4m3m;

extern const layout layout_8m6m;
extern const layout layout_8m7m;

extern const layout layout_16m14m;
extern const layout layout_16m15m;


typedef std::function<void(File & f)> FileCb; 


};



//376884
//376452


class FSTools {
public:



	FSTools(); 
	~FSTools(); 
	bool attemptToMountFS(fs::FS & fs); 
	bool mountAlternativeFS(  FST::FS_t type, const FST::layout layout, bool keepMounted = false ); 
	bool mounted(); 
	bool moveFS(fs::FS & destinationFS); 
	void reset(); 
	void fileListIterator(FS & fs, const char * dirName, FST::FileCb Cb ); 

private:
	uint32_t _getStartAddr(const FST::layout & layout); 
	uint32_t _getSize(const FST::layout & layout); 
	void _dumpFileInfo(File & f); 
	bool _copyFS(FS & sourceFS, FS & destFS);

	std::unique_ptr<fs::FS> _pFS;
	bool _mounted{false}; 
	const FST::layout * _layout{nullptr}; 


};