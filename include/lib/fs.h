#ifndef LIB_FS_H
#define LIB_FS_H

#include <efi.h>
#include <efilib.h>


/*
 *  Opens a file and returns a pointer
 *  to a buffer.
 */

UINT8* fs_open(CHAR16* path, EFI_HANDLE image);


#endif
