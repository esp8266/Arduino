/*
 * spiffs.h
 *
 *  Created on: May 26, 2013
 *      Author: petera
 */



#ifndef SPIFFS_H_
#define SPIFFS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "spiffs_config.h"

#define SPIFFS_OK                       0
#define SPIFFS_ERR_NOT_MOUNTED          -10000
#define SPIFFS_ERR_FULL                 -10001
#define SPIFFS_ERR_NOT_FOUND            -10002
#define SPIFFS_ERR_END_OF_OBJECT        -10003
#define SPIFFS_ERR_DELETED              -10004
#define SPIFFS_ERR_NOT_FINALIZED        -10005
#define SPIFFS_ERR_NOT_INDEX            -10006
#define SPIFFS_ERR_OUT_OF_FILE_DESCS    -10007
#define SPIFFS_ERR_FILE_CLOSED          -10008
#define SPIFFS_ERR_FILE_DELETED         -10009
#define SPIFFS_ERR_BAD_DESCRIPTOR       -10010
#define SPIFFS_ERR_IS_INDEX             -10011
#define SPIFFS_ERR_IS_FREE              -10012
#define SPIFFS_ERR_INDEX_SPAN_MISMATCH  -10013
#define SPIFFS_ERR_DATA_SPAN_MISMATCH   -10014
#define SPIFFS_ERR_INDEX_REF_FREE       -10015
#define SPIFFS_ERR_INDEX_REF_LU         -10016
#define SPIFFS_ERR_INDEX_REF_INVALID    -10017
#define SPIFFS_ERR_INDEX_FREE           -10018
#define SPIFFS_ERR_INDEX_LU             -10019
#define SPIFFS_ERR_INDEX_INVALID        -10020
#define SPIFFS_ERR_NOT_WRITABLE         -10021
#define SPIFFS_ERR_NOT_READABLE         -10022
#define SPIFFS_ERR_CONFLICTING_NAME     -10023
#define SPIFFS_ERR_NOT_CONFIGURED       -10024

#define SPIFFS_ERR_NOT_A_FS             -10025
#define SPIFFS_ERR_MOUNTED              -10026
#define SPIFFS_ERR_ERASE_FAIL           -10027
#define SPIFFS_ERR_MAGIC_NOT_POSSIBLE   -10028


#define SPIFFS_ERR_INTERNAL             -10050

#define SPIFFS_ERR_TEST                 -10100


// spiffs file descriptor index type. must be signed
typedef s16_t spiffs_file;
// spiffs file descriptor flags
typedef u16_t spiffs_flags;
// spiffs file mode
typedef u16_t spiffs_mode;
// object type
typedef u8_t spiffs_obj_type;

/* spi read call function type */
typedef s32_t (*spiffs_read)(u32_t addr, u32_t size, u8_t *dst);
/* spi write call function type */
typedef s32_t (*spiffs_write)(u32_t addr, u32_t size, u8_t *src);
/* spi erase call function type */
typedef s32_t (*spiffs_erase)(u32_t addr, u32_t size);

/* file system check callback report operation */
typedef enum {
  SPIFFS_CHECK_LOOKUP = 0,
  SPIFFS_CHECK_INDEX,
  SPIFFS_CHECK_PAGE
} spiffs_check_type;

/* file system check callback report type */
typedef enum {
  SPIFFS_CHECK_PROGRESS = 0,
  SPIFFS_CHECK_ERROR,
  SPIFFS_CHECK_FIX_INDEX,
  SPIFFS_CHECK_FIX_LOOKUP,
  SPIFFS_CHECK_DELETE_ORPHANED_INDEX,
  SPIFFS_CHECK_DELETE_PAGE,
  SPIFFS_CHECK_DELETE_BAD_FILE,
} spiffs_check_report;

/* file system check callback function */
typedef void (*spiffs_check_callback)(spiffs_check_type type, spiffs_check_report report,
    u32_t arg1, u32_t arg2);

#ifndef SPIFFS_DBG
#define SPIFFS_DBG(...) \
    print(__VA_ARGS__)
#endif
#ifndef SPIFFS_GC_DBG
#define SPIFFS_GC_DBG(...) c_printf(__VA_ARGS__)
#endif
#ifndef SPIFFS_CACHE_DBG
#define SPIFFS_CACHE_DBG(...) c_printf(__VA_ARGS__)
#endif
#ifndef SPIFFS_CHECK_DBG
#define SPIFFS_CHECK_DBG(...) c_printf(__VA_ARGS__)
#endif

/* Any write to the filehandle is appended to end of the file */
#define SPIFFS_APPEND                   (1<<0)
/* If the opened file exists, it will be truncated to zero length before opened */
#define SPIFFS_TRUNC                    (1<<1)
/* If the opened file does not exist, it will be created before opened */
#define SPIFFS_CREAT                    (1<<2)
/* The opened file may only be read */
#define SPIFFS_RDONLY                   (1<<3)
/* The opened file may only be writted */
#define SPIFFS_WRONLY                   (1<<4)
/* The opened file may be both read and writted */
#define SPIFFS_RDWR                     (SPIFFS_RDONLY | SPIFFS_WRONLY)
/* Any writes to the filehandle will never be cached */
#define SPIFFS_DIRECT                   (1<<5)

#define SPIFFS_SEEK_SET                 (0)
#define SPIFFS_SEEK_CUR                 (1)
#define SPIFFS_SEEK_END                 (2)

#define SPIFFS_TYPE_FILE                (1)
#define SPIFFS_TYPE_DIR                 (2)
#define SPIFFS_TYPE_HARD_LINK           (3)
#define SPIFFS_TYPE_SOFT_LINK           (4)

#ifndef SPIFFS_LOCK
#define SPIFFS_LOCK(fs)
#endif

#ifndef SPIFFS_UNLOCK
#define SPIFFS_UNLOCK(fs)
#endif

// phys structs

// spiffs spi configuration struct
typedef struct {
  // physical read function
  spiffs_read hal_read_f;
  // physical write function
  spiffs_write hal_write_f;
  // physical erase function
  spiffs_erase hal_erase_f;
#if SPIFFS_SINGLETON == 0
  // physical size of the spi flash
  u32_t phys_size;
  // physical offset in spi flash used for spiffs,
  // must be on block boundary
  u32_t phys_addr;
  // physical size when erasing a block
  u32_t phys_erase_block;

  // logical size of a block, must be on physical
  // block size boundary and must never be less than
  // a physical block
  u32_t log_block_size;
  // logical size of a page, must be at least
  // log_block_size / 8
  u32_t log_page_size;
#endif
} spiffs_config;

typedef struct {
  // file system configuration
  spiffs_config cfg;
  // number of logical blocks
  u32_t block_count;

  // cursor for free blocks, block index
  spiffs_block_ix free_cursor_block_ix;
  // cursor for free blocks, entry index
  int free_cursor_obj_lu_entry;
  // cursor when searching, block index
  spiffs_block_ix cursor_block_ix;
  // cursor when searching, entry index
  int cursor_obj_lu_entry;

  // primary work buffer, size of a logical page
  u8_t *lu_work;
  // secondary work buffer, size of a logical page
  u8_t *work;
  // file descriptor memory area
  u8_t *fd_space;
  // available file descriptors
  u32_t fd_count;

  // last error
  s32_t err_code;

  // current number of free blocks
  u32_t free_blocks;
  // current number of busy pages
  u32_t stats_p_allocated;
  // current number of deleted pages
  u32_t stats_p_deleted;
  // flag indicating that garbage collector is cleaning
  u8_t cleaning;
  // max erase count amongst all blocks
  spiffs_obj_id max_erase_count;

#if SPIFFS_GC_STATS
  u32_t stats_gc_runs;
#endif

#if SPIFFS_CACHE
  // cache memory
  void *cache;
  // cache size
  u32_t cache_size;
#if SPIFFS_CACHE_STATS
  u32_t cache_hits;
  u32_t cache_misses;
#endif
#endif

  // check callback function
  spiffs_check_callback check_cb_f;

  // mounted flag
  u8_t mounted;
  // config magic
  u32_t config_magic;
} spiffs;

/* spiffs file status struct */
typedef struct {
  spiffs_obj_id obj_id;
  u32_t size;
  spiffs_obj_type type;
  u8_t name[SPIFFS_OBJ_NAME_LEN];
} spiffs_stat;

struct spiffs_dirent {
  spiffs_obj_id obj_id;
  u8_t name[SPIFFS_OBJ_NAME_LEN];
  spiffs_obj_type type;
  u32_t size;
  spiffs_page_ix pix;
};

typedef struct {
  spiffs *fs;
  spiffs_block_ix block;
  int entry;
} spiffs_DIR;

// functions

/**
 * Initializes the file system dynamic parameters and mounts the filesystem.
 * If SPIFFS_USE_MAGIC is enabled the mounting may fail with SPIFFS_ERR_NOT_A_FS
 * if the flash does not contain a recognizable file system.
 * In this case, SPIFFS_format must be called prior to remounting.
 * @param fs            the file system struct
 * @param config        the physical and logical configuration of the file system
 * @param work          a memory work buffer comprising 2*config->log_page_size
 *                      bytes used throughout all file system operations
 * @param fd_space      memory for file descriptors
 * @param fd_space_size memory size of file descriptors
 * @param cache         memory for cache, may be null
 * @param cache_size    memory size of cache
 * @param check_cb_f    callback function for reporting during consistency checks
 */
s32_t SPIFFS_mount(spiffs *fs, spiffs_config *config, u8_t *work,
    u8_t *fd_space, u32_t fd_space_size,
    void *cache, u32_t cache_size,
    spiffs_check_callback check_cb_f);

/**
 * Unmounts the file system. All file handles will be flushed of any
 * cached writes and closed.
 * @param fs            the file system struct
 */
void SPIFFS_unmount(spiffs *fs);

/**
 * Creates a new file.
 * @param fs            the file system struct
 * @param path          the path of the new file
 * @param mode          ignored, for posix compliance
 */
s32_t SPIFFS_creat(spiffs *fs, char *path, spiffs_mode mode);

/**
 * Opens/creates a file.
 * @param fs            the file system struct
 * @param path          the path of the new file
 * @param flags         the flags for the open command, can be combinations of
 *                      SPIFFS_APPEND, SPIFFS_TRUNC, SPIFFS_CREAT, SPIFFS_RD_ONLY,
 *                      SPIFFS_WR_ONLY, SPIFFS_RDWR, SPIFFS_DIRECT
 * @param mode          ignored, for posix compliance
 */
spiffs_file SPIFFS_open(spiffs *fs, char *path, spiffs_flags flags, spiffs_mode mode);


/**
 * Opens a file by given dir entry.
 * Optimization purposes, when traversing a file system with SPIFFS_readdir
 * a normal SPIFFS_open would need to traverse the filesystem again to find
 * the file, whilst SPIFFS_open_by_dirent already knows where the file resides.
 * @param fs            the file system struct
 * @param path          the dir entry to the file
 * @param flags         the flags for the open command, can be combinations of
 *                      SPIFFS_APPEND, SPIFFS_TRUNC, SPIFFS_CREAT, SPIFFS_RD_ONLY,
 *                      SPIFFS_WR_ONLY, SPIFFS_RDWR, SPIFFS_DIRECT.
 *                      SPIFFS_CREAT will have no effect in this case.
 * @param mode          ignored, for posix compliance
 */
spiffs_file SPIFFS_open_by_dirent(spiffs *fs, struct spiffs_dirent *e, spiffs_flags flags, spiffs_mode mode);

/**
 * Reads from given filehandle.
 * @param fs            the file system struct
 * @param fh            the filehandle
 * @param buf           where to put read data
 * @param len           how much to read
 * @returns number of bytes read, or -1 if error
 */
s32_t SPIFFS_read(spiffs *fs, spiffs_file fh, void *buf, s32_t len);

/**
 * Writes to given filehandle.
 * @param fs            the file system struct
 * @param fh            the filehandle
 * @param buf           the data to write
 * @param len           how much to write
 * @returns number of bytes written, or -1 if error
 */
s32_t SPIFFS_write(spiffs *fs, spiffs_file fh, void *buf, s32_t len);

/**
 * Moves the read/write file offset
 * @param fs            the file system struct
 * @param fh            the filehandle
 * @param offs          how much/where to move the offset
 * @param whence        if SPIFFS_SEEK_SET, the file offset shall be set to offset bytes
 *                      if SPIFFS_SEEK_CUR, the file offset shall be set to its current location plus offset
 *                      if SPIFFS_SEEK_END, the file offset shall be set to the size of the file plus offset
 */
s32_t SPIFFS_lseek(spiffs *fs, spiffs_file fh, s32_t offs, int whence);

/**
 * Removes a file by path
 * @param fs            the file system struct
 * @param path          the path of the file to remove
 */
s32_t SPIFFS_remove(spiffs *fs, char *path);

/**
 * Removes a file by filehandle
 * @param fs            the file system struct
 * @param fh            the filehandle of the file to remove
 */
s32_t SPIFFS_fremove(spiffs *fs, spiffs_file fh);

/**
 * Gets file status by path
 * @param fs            the file system struct
 * @param path          the path of the file to stat
 * @param s             the stat struct to populate
 */
s32_t SPIFFS_stat(spiffs *fs, char *path, spiffs_stat *s);

/**
 * Gets file status by filehandle
 * @param fs            the file system struct
 * @param fh            the filehandle of the file to stat
 * @param s             the stat struct to populate
 */
s32_t SPIFFS_fstat(spiffs *fs, spiffs_file fh, spiffs_stat *s);

/**
 * Flushes all pending write operations from cache for given file
 * @param fs            the file system struct
 * @param fh            the filehandle of the file to flush
 */
s32_t SPIFFS_fflush(spiffs *fs, spiffs_file fh);

/**
 * Closes a filehandle. If there are pending write operations, these are finalized before closing.
 * @param fs            the file system struct
 * @param fh            the filehandle of the file to close
 */
void SPIFFS_close(spiffs *fs, spiffs_file fh);

/**
 * Renames a file
 * @param fs            the file system struct
 * @param old           path of file to rename
 * @param newPath       new path of file
 */
s32_t SPIFFS_rename(spiffs *fs, char *old, char *newPath);

/**
 * Returns last error of last file operation.
 * @param fs            the file system struct
 */
s32_t SPIFFS_errno(spiffs *fs);

/**
 * Clears last error.
 * @param fs            the file system struct
 */
void SPIFFS_clearerr(spiffs *fs);

/**
 * Opens a directory stream corresponding to the given name.
 * The stream is positioned at the first entry in the directory.
 * On hydrogen builds the name argument is ignored as hydrogen builds always correspond
 * to a flat file structure - no directories.
 * @param fs            the file system struct
 * @param name          the name of the directory
 * @param d             pointer the directory stream to be populated
 */
spiffs_DIR *SPIFFS_opendir(spiffs *fs, char *name, spiffs_DIR *d);

/**
 * Closes a directory stream
 * @param d             the directory stream to close
 */
s32_t SPIFFS_closedir(spiffs_DIR *d);

/**
 * Reads a directory into given spifs_dirent struct.
 * @param d             pointer to the directory stream
 * @param e             the dirent struct to be populated
 * @returns null if error or end of stream, else given dirent is returned
 */
struct spiffs_dirent *SPIFFS_readdir(spiffs_DIR *d, struct spiffs_dirent *e);

/**
 * Runs a consistency check on given filesystem.
 * @param fs            the file system struct
 */
s32_t SPIFFS_check(spiffs *fs);


/**
 * Returns number of total bytes available and number of used bytes.
 * This is an estimation, and depends on if there a many files with little
 * data or few files with much data.
 * NB: If used number of bytes exceeds total bytes, a SPIFFS_check should
 * run. This indicates a power loss in midst of things. In worst case
 * (repeated powerlosses in mending or gc) you might have to delete some files.
 *
 * @param fs            the file system struct
 * @param total         total number of bytes in filesystem
 * @param used          used number of bytes in filesystem
 */
s32_t SPIFFS_info(spiffs *fs, u32_t *total, u32_t *used);

/**
 * Formats the entire file system. All data will be lost.
 * The filesystem must not be mounted when calling this.
 *
 * NB: formatting is awkward. Due to backwards compatibility, SPIFFS_mount
 * MUST be called prior to formatting in order to configure the filesystem.
 * If SPIFFS_mount succeeds, SPIFFS_unmount must be called before calling
 * SPIFFS_format.
 * If SPIFFS_mount fails, SPIFFS_format can be called directly without calling
 * SPIFFS_unmount first.
 */
s32_t SPIFFS_format(spiffs *fs);

/**
 * Returns nonzero if spiffs is mounted, or zero if unmounted.
 */
u8_t SPIFFS_mounted(spiffs *fs);

/**
 * Check if EOF reached.
 * @param fs            the file system struct
 * @param fh            the filehandle of the file to check
 */
s32_t SPIFFS_eof(spiffs *fs, spiffs_file fh);

/**
 * Get the current position of the data pointer.
 * @param fs            the file system struct
 * @param fh            the filehandle of the open file
 */
s32_t SPIFFS_tell(spiffs *fs, spiffs_file fh);

#if SPIFFS_TEST_VISUALISATION
/**
 * Prints out a visualization of the filesystem.
 * @param fs            the file system struct
 */
s32_t SPIFFS_vis(spiffs *fs);
#endif

#if SPIFFS_BUFFER_HELP
/**
 * Returns number of bytes needed for the filedescriptor buffer given
 * amount of file descriptors.
 */
u32_t SPIFFS_buffer_bytes_for_filedescs(spiffs *fs, u32_t num_descs);

#if SPIFFS_CACHE
/**
 * Returns number of bytes needed for the cache buffer given
 * amount of cache pages.
 */
u32_t SPIFFS_buffer_bytes_for_cache(spiffs *fs, u32_t num_pages);
#endif
#endif


#ifdef __cplusplus
}
#endif

#endif /* SPIFFS_H_ */
