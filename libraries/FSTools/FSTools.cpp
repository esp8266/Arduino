#include "FSTools.h"
#include "LittleFS.h"
#include <spiffs_api.h>
#include <Esp.h>



#if defined(DEBUG_ESP_CORE)
#define FSTOOLSDEBUG(_1, ...) { DEBUG_ESP_PORT.printf_P( PSTR(_1),##__VA_ARGS__); }
#else
#define FSTOOLSDEBUG(...) {}
#endif


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
    //  try to apply the "safe" no format config to the FS... doesn't matter which.. just need one to apply correctly..
    if (!fs.setConfig(littleFSCfg) && ! fs.setConfig(SPIFFSCfg))
    {
        return false;
    }
    return fs.begin();
}


bool FSTools::mountAlternativeFS(FST::FS_t type, const FST::layout & layout, bool keepMounted)
{
    FSConfig * pCfg{nullptr};
    LittleFSConfig littleFSCfg(false);
    SPIFFSConfig SPIFFSCfg(false);
    reset();

    switch (type)
    {
    case FST::SPIFFS :
    {
        _pFS.reset(new FS(FSImplPtr(new spiffs_impl::SPIFFSImpl(_getStartAddr(layout), _getSize(layout), layout.page, layout.block, 5))));
        pCfg = &SPIFFSCfg;
        break;
    }
    case FST::LITTLEFS :
    {
        _pFS.reset(new FS(FSImplPtr(new littlefs_impl::LittleFSImpl(_getStartAddr(layout), _getSize(layout), layout.page, layout.block, 5))));
        pCfg = &littleFSCfg;
        break;
    }
    };

    if (_pFS && pCfg && _pFS->setConfig(*pCfg) && _pFS->begin())
    {
        if (!keepMounted)
        {
            _pFS->end();
        }
        _mounted = true;
        _layout = &layout;
        return true;
    }

    if (_pFS)
    {
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

    if (!_mounted || !_pFS)
    {
        FSTOOLSDEBUG("Source FS not mounted\n");
        return false;
    }

    uint32_t startSector = (ESP.getSketchSize() + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));
    uint32_t lowestFSStart = 0x40300000;

    if (_layout)
    {
        lowestFSStart = _layout->startAddr;
        FSTOOLSDEBUG("_layout->startADDR = 0x%08x\n", _layout->startAddr);
    }

    uint32_t endSector = lowestFSStart - 0x40200000;
    uint32_t tempFSsize = endSector - startSector;

    FSTOOLSDEBUG("TempFS:  start: %u, end: %u, size: %u, sketchSize = %u, _FS_start = %u\n", startSector, endSector, tempFSsize, ESP.getSketchSize(), (uint32_t)&_FS_start);

    fileListIterator(*_pFS, "/", [&sourceFileCount, &sourceByteTotal, this](File & f)
    {
        if (f)
        {
            sourceFileCount++;
            sourceByteTotal += f.size();

#ifdef DEBUG_ESP_CORE
            _dumpFileInfo(f);
#endif

        }
    });

    FSTOOLSDEBUG("%u Files Found Total Size = %u\n", sourceFileCount, sourceByteTotal);
    FSTOOLSDEBUG("Size of dummy FS = %u\n", tempFSsize);

    FS tempFS = FS(FSImplPtr(new littlefs_impl::LittleFSImpl(startSector, tempFSsize, FS_PHYS_PAGE, FS_PHYS_BLOCK, 5)));

    if (tempFS.format() && tempFS.begin())
    {
        if (_copyFS(*_pFS, tempFS))
        {
            FSTOOLSDEBUG("Files copied to temp File System\n");
            reset();
            if (destinationFS.format() && destinationFS.begin()) //  must format then mount the new FS
            {
                if (_copyFS(tempFS, destinationFS))
                {
                    FSTOOLSDEBUG("Files copied back to new FS\n");
                    result = true;
                }
            }
            else
            {
                FSTOOLSDEBUG("Error Mounting\n");
            }
        }
        else
        {
            FSTOOLSDEBUG("Copy Failed\n");
        }
        tempFS.end();
    }
    else
    {
        FSTOOLSDEBUG("Failed to begin() TempFS\n");
    }
    return result;
};

void FSTools::reset()
{
    _mounted = false;
    _layout = nullptr;
    if (_pFS)
    {
        _pFS->end();
        _pFS.reset();
    }
}

void FSTools::fileListIterator(FS & fs, const char * dirName, FST::FileCb Cb)
{
    Dir dir = fs.openDir(dirName);
    while (dir.next())
    {
        if (dir.isFile())
        {
            File f =  dir.openFile("r");
            if (Cb)
            {
                Cb(f);
            }
        }
        else
        {
            fileListIterator(fs, dir.fileName().c_str(), Cb);
        }
    }
}


uint32_t FSTools::_getStartAddr(const FST::layout & layout)
{
    return (layout.startAddr - 0x40200000);
}

uint32_t FSTools::_getSize(const FST::layout & layout)
{
    return (layout.endAddr - layout.startAddr);
}

#ifdef DEBUG_ESP_CORE
void FSTools::_dumpFileInfo(File & f)
{
    if (f)
    {
        DEBUG_ESP_PORT.printf_P(PSTR(" File: %-30s [%8uB]\n"), f.fullName(), f.size());
    }
}
#endif

bool FSTools::_copyFS(FS & sourceFS, FS & destFS)
{
    uint32_t sourceFileCount = 0;
    uint32_t sourceByteTotal = 0;

    fileListIterator(sourceFS, "/", [&sourceFileCount, &sourceByteTotal](File & f)
    {
        if (f)
        {
            sourceFileCount++;
            sourceByteTotal += f.size();
        }
    });

    size_t count = 0;
    fileListIterator(sourceFS, "/", [&count, &destFS](File & sourceFile)
    {
        if (sourceFile)
        {
            File destFile = destFS.open(sourceFile.fullName(), "w");
            if (destFile)
            {
                destFile.setTimeout(5000); //  this value was chosen empirically as it failed with default timeout. 
                size_t written = destFile.write(sourceFile);
                if (written == sourceFile.size())
                {
                    count++;
                }
            }
            destFile.close();
            sourceFile.close();
            yield();
        }
    });

    return (count == sourceFileCount);

}
