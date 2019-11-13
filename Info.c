

#include "Info.h"
#include "public.h"
//#define MAX_CMD_SUPPORTED  20


/**

    this function is used to hash information

    @param Data           - the data to been hashed
    @param DataSize       - the hashed buffer size
    @param Size           - the output of the hash buffer.
    @retval CHAR8 *       - pointer of the hashed data info
**/

UINT8 *
InfoHashInfoData (
  IN  UINT8                     *Data,
  IN  UINTN                     DataSize,
  IN  OUT  UINTN                *Size
  )
{
  UINT8                Index;
  UINTN                ContextSize;
  VOID                 *HashContext;
  BOOLEAN              HashResult;
  UINT8                *Digest;
  //
  // Init
  //
  Digest = AllocateZeroPool (SHA256_DIGEST_SIZE);
  ZeroMem (Digest, SHA256_DIGEST_SIZE);
  ContextSize = Sha256GetContextSize ();
  HashContext = AllocatePool (ContextSize);

  //
  // SHA256   Init
  //
  HashResult = Sha256Init (HashContext);
  if (!HashResult) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "Sha256Init Failed!\n");
    return NULL;
  }
  
  //
  //Digests the input data and updates SHA-256 context.
  //
  HashResult = Sha256Update (HashContext, Data, DataSize);
  if (!HashResult) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "Sha256Update Failed!\n");
    return NULL;
  }

  //
  //Completes computation of the SHA-256 digest value.
  //
  HashResult = Sha256Final (HashContext, Digest);
  if (!HashResult) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "Sha256Final Failed!\n");
    return NULL;
  }

  InfoDebugPrint (INFO_INFO_LEVEL, "========================Hash=====================\n");
  for (Index = 0; Index < SHA256_DIGEST_SIZE; Index ++) {
    if ((Index != 0) && ((Index%16) == 0)) {
       InfoDebugPrint (INFO_INFO_LEVEL,"\n");
    }
    if(Digest[Index] == 0) {
       Digest[Index]=1;
    }
    InfoDebugPrint (INFO_INFO_LEVEL," %02x", Digest[Index]);
  }
  InfoDebugPrint (INFO_INFO_LEVEL,"\n");
  InfoDebugPrint (INFO_INFO_LEVEL, "========================Hash=====================\n");
  *Size = SHA256_DIGEST_SIZE;
  return Digest;

}


/**
    Get Enc file
    @param  FileName               - file name and path of the enc file
    @param  Size                   - out size of enc file 
    @param  Data                   - the out data of enc file
    @retval EFI_SUCCESS get the file success, other failed.
**/

EFI_STATUS
InfoGetEncFile (
  IN CHAR16         *FileName,
  IN OUT  UINTN     *Size,
  IN OUT  UINT8     **Data
)
{
  EFI_STATUS                        Status;
  UINTN                             Index;
  UINTN                             FileSystemHandlesNum;
  EFI_HANDLE                       *FileSystemHandles;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *SimpleFileSystem;
  EFI_FILE_HANDLE                  RootHandle;
  EFI_FILE_HANDLE                  ThisFileHandle;
  UINT8                            IsFound;
  UINT8                            *Buffer;
  //EFI_FILE_INFO                    FileInfo;
  UINTN                            FileSize;

  FileSize             = 0;
  FileSystemHandlesNum = 0;
  FileSystemHandles    = NULL;
  SimpleFileSystem     = NULL;
  RootHandle           = NULL;
  ThisFileHandle       = NULL;
  IsFound              = 0;
  Status               = EFI_SUCCESS;
  //
  // locate all simple file system handles
  // 
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &FileSystemHandlesNum,
                  &FileSystemHandles
                  );

  if (EFI_ERROR (Status) || (FileSystemHandlesNum == 0)) {
    return Status;
  }

  for (Index = 0; Index < FileSystemHandlesNum; Index++) {
    //
    //get simple file system protocol
    // 
    Status = gBS->HandleProtocol (
                    FileSystemHandles[Index],
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID **)&SimpleFileSystem
                    );

    if (EFI_ERROR (Status)) {
      continue;   
    }
    //
    //found the file
    // 
    Status = SimpleFileSystem->OpenVolume (
                                 SimpleFileSystem,
                                 &RootHandle
                                 );

    if (EFI_ERROR (Status)) {
      RootHandle = NULL;
      continue;   
    }

    Status = RootHandle->Open (
                           RootHandle,
                           &ThisFileHandle,
                           FileName,
                           EFI_FILE_MODE_READ,
                           0
                           );

    if (EFI_ERROR (Status)) {
      RootHandle     = NULL;
      ThisFileHandle = NULL;    
      continue;
    }

    IsFound = TRUE;
    break;
  }

  if (!IsFound) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "Didn't find %s, Please add it !!!\n", FileName);
    return EFI_UNSUPPORTED;
  }
  //
  //Get the file size and allocate pool
  // 
  Status = FileHandleGetSize (ThisFileHandle, &FileSize);
  if (FileSize == 0) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "%s size is zero !!!\n", FileName);
    return EFI_UNSUPPORTED;
  }

  Buffer = AllocateZeroPool (FileSize);
  if (Buffer == NULL) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "%s size is zero !!!\n", FileName);
    return EFI_UNSUPPORTED;
  }

  //
  //Read the file data
  // 
  if (EFI_ERROR(FileHandleRead (ThisFileHandle, &FileSize, Buffer))) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "%s read failed\n", FileName);
    return EFI_UNSUPPORTED;

  }
  *Size = FileSize;
  *Data = Buffer;
  if (FileSystemHandles) {
    FreePool (FileSystemHandles);
  }

  return Status;

}
/**
    Info Entry 
    @param  List               - NULL means 0,1,2,3,17 List need a 0x7F by end
    @param  List               - the output the SHA256 hash
    @retval CHAR8 *       NULL means failed, other means success
**/
UINT8 *
InfoEntry (
  IN  UINT8       *List,
  IN  OUT  UINTN  *HashSize
  )
{
  UINT8                         *Str;
  UINTN                         Size;
  UINT8                         *Mac;
  UINTN                         MacSize;
  UINT8                         *HashStr;
  //
  // Init
  //
  MacSize      = 0;
  Size         = 0;
  Str          = NULL;
  Mac          = NULL;

  //
  // Get Mac address
  //
  Mac = InfoGetMacData (&MacSize);
  if (Mac == NULL) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "Can't get mac address\n");
  }

  //
  // Get smbios data, NULL means type 0,1,2,3,17
  // list need a 0x7f smbios type end in the end
  //
  Str = InfoSmbiosRelatedData_ (List, &Size);
  if (Str == NULL) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "Can't get smbios related data \n");
    return NULL;
  }

  //
  // If smbios data not null, copy it and hash it
  //
  if (Mac != NULL) {
    Str = ReallocatePool (Size, MacSize+Size, Str);
    CopyMem (Str+Size, Mac, MacSize);
    Size += MacSize;
  }

  //
  // Hash the information
  //
  HashStr = InfoHashInfoData (Str, Size, HashSize);
  if (HashStr == NULL) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "hash info failed\n");
    return NULL;
  }
  return HashStr;
}
/**
    Rsa 4096 decrypt 
    @param  Data               - the data of enc data
    @param  Size               - the size of enc data
    @param  Hash               - the hash of the mainboard
    @retval bpolean             true is pass, fail is hash and dec hash not matched
**/

BOOLEAN
Rsa4096Dec (
  IN  UINT8   *Data,
  IN  UINTN   Size,
  IN  UINT8   *Hash
  )
{
  UINT8     msg[512];
  char      base64[4096] = {0};
//  UINTN     Index;
  if (Size > 4096) { //safety check
    InfoDebugPrint (INFO_PRINT_LEVEL,"the size is too big\n");
  }

  CopyMem (base64, Data, Size); // the end must been L'\0'
  memsetX (msg, 0, 512);
  public_decrypt((char *)base64, msg);//decrypt

  if (CompareMem (msg, Hash, SHA256_DIGEST_SIZE) == 0) {
    return TRUE;
  }
  return FALSE;
}

/**
    Info Entry point

    @param  ImageHandle    - The image handle.
    @param  SystemTable    - The system table.

    @retval EFI_SUCCESS            Command completed successfully.
**/
EFI_STATUS
EFIAPI
InfoEntryPointDxeEntry(
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                Status;
  UINTN                     Size;
  UINT8                     *Data;
  UINTN                     FileSize;
  UINT8                     *FileData;
//  UINTN                     Index;
  BOOLEAN                   EncPass = FALSE;
  EFI_GUID                  gGuid = {0xabef5300, 0x3494, 0x48c1, {0xac, 0x92, 0xab, 0x85, 0x45, 0xaa, 0x8e, 0x65}}; //gGuid = {0xEC87D643, 0xEBA4, 0x4BB5, {0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0x99}};

 //if you want to change smbios type list
 //UINT8                     List[] = {0,1,2,3,17,0x7f}; 

  //
  // Get the info hash
  //

  Data = InfoEntry (NULL, &Size);
  if (Data == NULL) {
    InfoDebugPrint (INFO_INFO_LEVEL, "Errored \n");
  }
 
  //
  // print the hash by 2*2
  //
/*
  InfoDebugPrint (INFO_PRINT_LEVEL, "========================Hash=====================\n");
  for (Index = 0; Index < SHA256_DIGEST_SIZE; Index ++) {
    if ((Index != 0) && ((Index%16) == 0)) {
       InfoDebugPrint (INFO_PRINT_LEVEL,"\n");
    }
    InfoDebugPrint (INFO_PRINT_LEVEL," %02x", Data[Index]);
  }
  InfoDebugPrint (INFO_PRINT_LEVEL,"\n");
  InfoDebugPrint (INFO_PRINT_LEVEL, "========================Hash=====================\n");
*/
  //
  // Get section saw by guid abef5300-3494-48c1-ac92-ab8545aa8e65
  //
  Status = GetSectionFromAnyFv (&gGuid, EFI_SECTION_RAW, 0, (VOID **) &FileData, &FileSize);
  if (EFI_ERROR (Status)) {
    if (FileData != NULL) {
        FreePool (FileData);
    }

    //
    // if can't found enc data in section, dispatch \efi\enc.txt file
    //
    Status = InfoGetEncFile (L"\\efi\\key.txt", &FileSize, &FileData);
    if (EFI_ERROR(Status)) {
      InfoDebugPrint (INFO_ERROR_LEVEL, "Errored load key data \n");
      return EFI_SUCCESS;
    }
  }

  //
  // Check if the hash is matched with the hash dec by enc file
  //
  EncPass = Rsa4096Dec (FileData, FileSize, Data);
  InfoDebugPrint (INFO_PRINT_LEVEL, "Key pass %a\n", EncPass?"TRUE":"FALSE");
  if (FileData != NULL) {
      FreePool (FileData);
  }

  if (EncPass) {
    EFI_HANDLE                           FvHandle = NULL;
    //
    // Process defined fv data in fv.h
    //
    Status = gDS->ProcessFirmwareVolume((VOID*)FvData, sizeof(FvData), &FvHandle);
    if (EFI_ERROR (Status)) {
      //InfoDebugPrint (INFO_PRINT_LEVEL, "ProcessFirmwareVolume Status %r\n", Status);
             return EFI_SUCCESS;
    }
    //
    // Dispatch fv
    //
    Status = gDS->Dispatch ();
    //InfoDebugPrint (INFO_PRINT_LEVEL, "gDS->Dispatch Status %r\n", Status);
  }
  return EFI_SUCCESS;
}
