#include <lib/fs.h>


static uint8_t got_volume = 0;
static EFI_FILE_HANDLE volume;


static EFI_FILE_HANDLE fetch_volume(EFI_HANDLE image) {
  EFI_LOADED_IMAGE* loadedImage = NULL;
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  EFI_FILE_IO_INTERFACE* ioVolume;
  EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
  EFI_FILE_HANDLE volume;

  // Get the loaded image protocol.
  uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void**)&loadedImage);

  // Get the volume handle.
  uefi_call_wrapper(BS->HandleProtocol, 3, loadedImage->DeviceHandle, &fsGuid, (void**)&ioVolume);
  return volume;
}

UINT64 get_file_size(EFI_FILE_HANDLE fhandle) {
  UINT64 size;
  EFI_FILE_INFO* fileInfo = LibFileInfo(fhandle);
  size = fileInfo->FileSize;
  FreePool(fileInfo);
  return size;
}

static void prepare_file_operation(EFI_HANDLE image) {
  if (!(got_volume)) {
    volume = fetch_volume(image);
    got_volume = 1;
  }
}


UINT8* fs_open(CHAR16* path, EFI_HANDLE image) {
  prepare_file_operation(image);
  EFI_FILE_HANDLE fhandle;

  EFI_STATUS s = uefi_call_wrapper(volume->Open, 5, volume, &fhandle, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);

  if (EFI_ERROR(s)) return NULL;

  UINT64 bufsize = get_file_size(fhandle);
  UINT8* buf = AllocatePool(bufsize);
  uefi_call_wrapper(fhandle->Read, 3, fhandle, &bufsize, buf);       // Read file into buffer.
  uefi_call_wrapper(fhandle->Close, 1, fhandle);                     // Close the file.
  return buf;
}
