/* ioapi_mem.h -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API

   This version of ioapi is designed to access memory rather than files.
   We do use a region of memory to put data in to and take it out of. We do
   not have auto-extending buffers and do not inform anyone else that the
   data has been written. It is really intended for accessing a zip archive
   embedded in an application such that I can write an installer with no
   external files. Creation of archives has not been attempted, although
   parts of the framework are present.

   
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zlib.h"
#include "ioapi.h"

#ifdef __cplusplus
extern "C" {
#endif


voidpf ZCALLBACK fopen_mem_func OF((voidpf opaque,const char* filename,int mode));
voidpf ZCALLBACK fopendisk_mem_func OF((voidpf opaque, voidpf stream, int number_disk, int mode));
uLong ZCALLBACK fread_mem_func OF((voidpf opaque,voidpf stream,void* buf,uLong size));
uLong ZCALLBACK fwrite_mem_func OF((voidpf opaque,voidpf stream,const void* buf,uLong size));
long ZCALLBACK ftell_mem_func OF((voidpf opaque,voidpf stream));
long ZCALLBACK fseek_mem_func OF((voidpf opaque,voidpf stream,uLong offset,int origin));
long ZCALLBACK fseek64_mem_func OF((voidpf opaque,voidpf stream,ZPOS64_T offset,int origin));
int ZCALLBACK fclose_mem_func OF((voidpf opaque,voidpf stream));
int ZCALLBACK ferror_mem_func OF((voidpf opaque,voidpf stream));

typedef struct ourmemory_s {
  char *base;       /* Base of the region of memory we're using */
  uLong size;       /* Size of the region of memory we're using */
  uLong limit;      /* Furthest we've written */
  uLong cur_offset; /* Current offset in the area */
} ourmemory_t;

void fill_memory_filefunc OF((zlib_filefunc_def* pzlib_filefunc_def, ourmemory_t *ourmem));
void fill_memory_filefunc64 OF((zlib_filefunc64_def* pzlib_filefunc_def, ourmemory_t *ourmem));

#ifdef __cplusplus
}
#endif
