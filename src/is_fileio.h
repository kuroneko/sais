/* is_fileio.h
 *
 * Infinite Space: File Abstraction
*/
#ifndef IS_FILEIO_H
#define IS_FILEIO_H

#include <physfs.h>

typedef PHYSFS_File *   IS_FileHdl;

inline IS_FileHdl IS_Open_Read(const char *filename) {
    return PHYSFS_openRead(filename);
}

inline IS_FileHdl IS_Open_Write(const char *filename) {
    return PHYSFS_openWrite(filename);
}

inline IS_FileHdl IS_Open_Append(const char *filename) {
    return PHYSFS_openAppend(filename);
}

extern void IS_Close(IS_FileHdl fileHandle);

inline size_t IS_Read(void *dest, size_t elem, size_t count, IS_FileHdl fileHandle) {
    return PHYSFS_readBytes(fileHandle, dest, elem*count);
}

inline size_t IS_Write(void *src, size_t elem, size_t count, IS_FileHdl fileHandle) {
    return PHYSFS_writeBytes(fileHandle, src, elem*count);
}

inline bool IS_EOF(IS_FileHdl fileHandle) {
    return PHYSFS_eof(fileHandle);
}

inline bool IS_exists(const char *filename) {
    return PHYSFS_exists(filename);
}

inline void IS_Skip(IS_FileHdl fileHdl, size_t offset) {
    PHYSFS_seek(fileHdl, PHYSFS_tell(fileHdl) + offset);
}

extern int IS_Printf(IS_FileHdl fileHdl, const char *format, ...);
extern int IS_VPrintf(IS_FileHdl fileHdl, const char *format, va_list arglist);

int read_line(IS_FileHdl in, char *out1, char *out2);
int read_line1(IS_FileHdl in, char *out1);
void ik_start_log();
void ik_print_log(const char *format, ...);

extern IS_FileHdl logfile;
extern int last_logdate;
extern char moddir[256];

#endif /* IS_FILEIO_H */