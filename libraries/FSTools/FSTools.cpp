#include "FSTools.h"
#include "LittleFS.h"
#include <spiffs_api.h>
#include <ESP.h>


namespace FST {

	 const layout layout_512k32  (0x40273000, 0x4027B000, 0x100, 0x1000 ); 
	 const layout layout_512k64  (0x4026B000, 0x4027B000, 0x100, 0x1000 );
	 const layout layout_512k128 (0x4025B000, 0x4027B000, 0x100, 0x1000 );

	 const layout layout_1m64    (0x402EB000, 0x402FB000, 0x100, 0x1000 );
	 const layout layout_1m128   (0x402DB000, 0x402FB000, 0x100, 0x1000 );
	 const layout layout_1m144   (0x402D7000, 0x402FB000, 0x100, 0x1000 );
	 const layout layout_1m160   (0x402D3000, 0x402FB000, 0x100, 0x1000 );
	 const layout layout_1m192   (0x402CB000, 0x402FB000, 0x100, 0x1000 );
	 const layout layout_1m256   (0x402BB000, 0x402FB000, 0x100, 0x1000 );
	 const layout layout_1m512   (0x4027B000, 0x402FB000, 0x100, 0x2000 );

	 const layout layout_2m64    (0x403F0000, 0x403FB000, 0x100, 0x1000 );
	 const layout layout_2m128   (0x403E0000, 0x403FB000, 0x100, 0x1000 );
	 const layout layout_2m256   (0x403C0000, 0x403FB000, 0x100, 0x1000 );
	 const layout layout_2m512   (0x40380000, 0x403FA000, 0x100, 0x2000 );
	 const layout layout_2m1m    (0x40300000, 0x403FA000, 0x100, 0x2000 );

	 const layout layout_4m1m    (0x40500000, 0x405FA000, 0x100, 0x2000 );
	 const layout layout_4m2m    (0x40400000, 0x405FA000, 0x100, 0x2000 );
	 const layout layout_4m3m    (0x40300000, 0x405FA000, 0x100, 0x2000 );

	 const layout layout_8m6m    (0x40400000, 0x409FA000, 0x100, 0x2000 );
	 const layout layout_8m7m    (0x40300000, 0x409FA000, 0x100, 0x2000 );

	 const layout layout_16m14m  (0x40400000, 0x411FA000, 0x100, 0x2000 );
	 const layout layout_16m15m  (0x40300000, 0x411FA000, 0x100, 0x2000 );


};

FSTools::FSTools()
{

}

FSTools::~FSTools()
{
	reset(); 
}


bool FSTools::attemptToMountFS(fs::FS & fs)
{
  LittleFSConfig littleFSCfg(false);
  SPIFFSConfig SPIFFSCfg(false); 
  //  try to apply the "safe" no format config to the FS... doesn't matter which.. just need one to apply.. 
  if (!fs.setConfig(littleFSCfg) && ! fs.setConfig(SPIFFSCfg) )
  {
  	return false; 
  }
  return fs.begin(); 
}


bool FSTools::mountAlternativeFS(  FST::FS_t type, const FST::layout & layout, bool keepMounted )
{
	FSConfig * pCfg{nullptr};
    LittleFSConfig littleFSCfg(false);
    SPIFFSConfig SPIFFSCfg(false); 
    reset();

	switch (type) {
		case FST::SPIFFS : {
			_pFS.reset( new FS(FSImplPtr(new spiffs_impl::SPIFFSImpl    (_getStartAddr(layout) , _getSize(layout) , layout.page, layout.block, 5)))  );
			pCfg = &SPIFFSCfg; 
			break;
		}
		case FST::LITTLEFS : {
			_pFS.reset( new FS(FSImplPtr(new littlefs_impl::LittleFSImpl(_getStartAddr(layout) , _getSize(layout) , layout.page, layout.block, 5)))  );
			pCfg = &littleFSCfg; 
			break;
		}
	};

	if (_pFS && pCfg && _pFS->setConfig(*pCfg) && _pFS->begin()) {
		if (!keepMounted) {
			_pFS->end();
		}
		_mounted = true; 
		_layout = &layout;
		return true; 
	}

	if (_pFS) {
		_pFS.reset();
	}
	_mounted = false; 
	return false; 

}; 


bool FSTools::mounted()
{
	return _mounted; 
};




bool FSTools::moveFS(fs::FS & destinationFS)
{
    uint32_t sourceFileCount = 0; 
    uint32_t sourceByteTotal = 0; 
    bool result = false; 

	//  do not init new fs... until old one is copied to test... 
	if (!_mounted || !_pFS) {
		//Serial.println("Source FS not mounted"); 
		return false; 
	}


  	uint32_t startSector = (ESP.getSketchSize() + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1)); 
  	//uint32_t endSector = (uint32_t)&_FS_start - FST::lowestSPIFFSstartAddr;
  	uint32_t lowestFSStart = 0x40300000;
  	
  	 if (_layout) {
  	 	lowestFSStart = _layout->startAddr;  	
  		//Serial.printf("**  _layout->startADDR = 0x%08x\n", _layout->startAddr ); 
  	 }
  	
  	uint32_t endSector = lowestFSStart - 0x40200000;  
  	uint32_t tempFSsize = endSector - startSector;

  	//Serial.printf("TempFS:  start: %u, end: %u, size: %u, sketchSize = %u, _FS_start = %u\n", startSector, endSector, tempFSsize, ESP.getSketchSize(), (uint32_t)&_FS_start );  

    fileListIterator(*_pFS, "/", [&sourceFileCount, &sourceByteTotal, this](File & f) {
      if (f) {
        sourceFileCount++;
        sourceByteTotal += f.size(); 
        //_dumpFileInfo(f); 
    }
    });

    //Serial.printf("%u Files Found Total Size = %u\n", sourceFileCount, sourceByteTotal); 
    //Serial.printf("Size of dummy FS = %u\n", tempFSsize); 

    FS tempFS = FS(FSImplPtr(new littlefs_impl::LittleFSImpl(startSector, tempFSsize, FS_PHYS_PAGE, FS_PHYS_BLOCK, 5)));

     if (tempFS.format() && tempFS.begin()) {
     	 if (_copyFS(*_pFS,tempFS)) {
     	 	//Serial.println("Files copied to temp File System"); 
     	 	reset(); 
     	 	if (destinationFS.format() && destinationFS.begin()) //  must format then mount the new FS
     	 	{
     	 		if (_copyFS(tempFS, destinationFS)) {
     	 			//Serial.println("Files copied back to new FS");    
     	 			result = true;     
     	 		}
     	 	} else {
     	 		//Serial.println("Error Mounting "); 
     	 	}
     	 } else {
     	 	//Serial.println("Copy Failed"); 
     	 }
     	tempFS.end(); 
     } else {
     	//Serial.println("Failed to begin() TempFS"); 
     }
     return result; 
}; 

void FSTools::reset()
{
	_mounted = false;
	_layout = nullptr;
	if (_pFS) {
		_pFS->end();
		_pFS.reset(); 
	}
}

void FSTools::fileListIterator(FS & fs, const char * dirName, FST::FileCb Cb ) 
{
    Dir dir = fs.openDir(dirName);
    while (dir.next()) {
      if (dir.isFile()) {
        File f =  dir.openFile("r");
        if (Cb) {
          Cb(f); 
        }
      } else {
        fileListIterator(fs, dir.fileName().c_str() , Cb);
      }
    }
}


uint32_t FSTools::_getStartAddr(const FST::layout & layout)
{
	return ( layout.startAddr - 0x40200000 ); 
}

uint32_t FSTools::_getSize(const FST::layout & layout)
{
	return (layout.endAddr - layout.startAddr ); 
} 


// void FSTools::_dumpFileInfo(File & f)
// {
//   if (f) {
//     Serial.printf_P(PSTR(" File: %-30s [%8uB]\n"), f.fullName(), f.size() );
//   }
// }

bool FSTools::_copyFS(FS & sourceFS, FS & destFS)
{
    uint32_t sourceFileCount = 0; 
    uint32_t sourceByteTotal = 0; 
    
    fileListIterator(sourceFS, "/", [&sourceFileCount, &sourceByteTotal](File & f) {
      if (f) {
        sourceFileCount++;
        sourceByteTotal += f.size(); 
    }
    });

      size_t count = 0; 
      fileListIterator(sourceFS, "/", [&count, &destFS](File & sourceFile){ 
        if (sourceFile) {
          File destFile = destFS.open(sourceFile.fullName(), "w");
          bool copied = false;
          if (destFile) {
            destFile.setTimeout(5000);
            size_t written = destFile.write(sourceFile);
            if (written == sourceFile.size()) {
              copied = true;
              count++;
            }            
          }
          destFile.close();
          sourceFile.close();
        }
        });

    return (count == sourceFileCount); 
  
}
