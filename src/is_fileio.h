/* is_fileio.h
 *
 * Infinite Space: File Abstraction
*/
#ifndef IS_FILEIO_H
#define IS_FILEIO_H

typedef FILE*   IS_FileHdl;

IS_FileHdl myopen(const char *fname, const char *flags);
int read_line(IS_FileHdl in, char *out1, char *out2);
int read_line1(IS_FileHdl in, char *out1);
void ik_start_log();
void ik_print_log(const char *ln, ...);

extern IS_FileHdl logfile;
extern int last_logdate;
extern char moddir[256];

#endif /* IS_FILEIO_H */