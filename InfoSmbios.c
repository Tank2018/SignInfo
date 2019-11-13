#include "InfoSmbios.h"



UINTN               mMaxLen;
UINT8                mDefaultTypeList[] = {
  SMBIOS_TYPE_BIOS_INFORMATION,
  SMBIOS_TYPE_SYSTEM_INFORMATION,
  SMBIOS_TYPE_BASEBOARD_INFORMATION,
  SMBIOS_TYPE_SYSTEM_ENCLOSURE,
  SMBIOS_TYPE_MEMORY_DEVICE,
  0x7F //end
  };


/**
    return string ptr by the smbios string num
    @param SmBiosPtr            - input pointer of Smbios
    @param StringNum            - input StringNum of Smbios
    @retval CHAR8 *             - matched string of the string number, NULL failed
**/
CHAR8 *
InfoGetStringByNum (
  IN VOID   *SmBiosPtr,
  IN UINT8  StringNum
  ) 
{
  EFI_SMBIOS_TABLE_HEADER   *SmbiosTableHeader;
  CHAR8                     *StringPtr;
  CHAR8                     *String;
  UINTN                     StrSize;
  
  SmbiosTableHeader = (EFI_SMBIOS_TABLE_HEADER *) SmBiosPtr;
  if (SmBiosPtr == NULL) {
    return NULL;
  }
  StringPtr = (CHAR8 *)((UINTN)SmBiosPtr + SmbiosTableHeader->Length);
  while (--StringNum) {
    while (*StringPtr != 0) {
      StringPtr++;
    }
    StringPtr++;
  }

  StrSize = AsciiStrSize (StringPtr);
  String = AllocateZeroPool (StrSize);
  if (String == NULL) {
    return NULL;
  }

  CopyMem (String, StringPtr, StrSize);
  return String;
}

/**
    Get the full size of SMBIOS structure including optional strings that follow the formatted structure.
    @param Head                   Pointer to the beginning of SMBIOS structure.
    @retval Size                  The returned size.
**/
UINTN
InfoGetSmbiosStructureSize (
  IN   EFI_SMBIOS_TABLE_HEADER          *Head
  )
{
  UINTN  Size;
  UINTN  StrLen;
  CHAR8  *CharInStr;
  UINTN  StringsNumber;

  CharInStr = (CHAR8 *)Head + Head->Length;
  Size = Head->Length;
  StringsNumber = 0;
  StrLen = 0;
  //
  // look for the two consecutive zeros, check the string limit by the way.
  //
  while ((*CharInStr != 0) ||(*(CharInStr+1) != 0)) {
    if (*CharInStr == 0) {
      Size += 1;
      CharInStr++;
    }

    for (StrLen = 0 ; StrLen < mMaxLen; StrLen++) {
      if (*(CharInStr+StrLen) == 0) {
        break;
      }
    }

    if (StrLen == mMaxLen) {
      return 0;
    }

    //
    // forward the pointer
    //
    CharInStr += StrLen;
    Size += StrLen;
    StringsNumber += 1;
  }

  //
  // count ending two zeros.
  //
  Size += 2;

  //if (NumberOfStrings != NULL) {
  //  *NumberOfStrings = StringsNumber;
  //}
  return Size;
}

/**
    return Smbios table Ptr by the smbios type
    @param Type                     - input Type of Smbios
    @param This                     - input pointer of EFI_SMBIOS_PROTOCOL
    @param SmbiosTableHeader        - out ptr of the Smbios
    @retval EFI_SUCCESS             - success to found smbios, others failed
**/
EFI_STATUS 
EFIAPI
InfoGetSmbiosByType (
  IN  EFI_SMBIOS_PROTOCOL             *This,
  IN  UINT8                           Type,
  IN  OUT UINTN                       **SmbiosTableHeader,
  IN  OUT UINTN                       *MatchedCount
  )
{
  EFI_STATUS                Status;
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER   *Record;
  UINTN                     Count;
  UINTN                     *SmBiosAddr;
  UINTN                     Index;
  
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Record       = NULL;
  Status       = EFI_NOT_FOUND;
  Count        = 0;
  
  do {
    Status = This->GetNext (This, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (SmbiosHandle == SMBIOS_HANDLE_PI_RESERVED) {
      break;
    }
    //
    // found matched type count;
    //
    if (Record->Type == Type) {
      Count ++;
    }
  } while (!EFI_ERROR (Status));
    
  if (!Count) {
    return EFI_NOT_FOUND;
  } 

  SmBiosAddr = (UINTN *) AllocateZeroPool (sizeof (UINTN) * Count);
  if (SmBiosAddr == NULL) {
    return EFI_NOT_FOUND;
  }
  
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Index = 0;
  do {
    Status = This->GetNext (This, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }
    if (SmbiosHandle == SMBIOS_HANDLE_PI_RESERVED) {
      break;
    }
    //
    // Matched smbios type, than break;
    //
    if (Record->Type == Type) {
      SmBiosAddr[Index] = (UINTN) Record;
      Index ++;
    }
  } while (!EFI_ERROR (Status));

  *SmbiosTableHeader = SmBiosAddr;
  *MatchedCount      = Count;
  return EFI_SUCCESS;
}

/**

    Get Cpu id info.
    @param This                     - input pointer of EFI_SMBIOS_PROTOCOL
    @param  *Size                   - The size of cpu id
    @retval UINT8 *                 - The returned ptr of cpu id.
**/

UINT8 *
InfoGetCpuID (
  IN  EFI_SMBIOS_PROTOCOL             *This,
  IN  OUT UINTN                       *Size
  )
{
  UINT8                   *CpuId;
  UINTN                   *Addr;
  UINTN                   Count;
  SMBIOS_TABLE_TYPE4      *Type4;
  EFI_STATUS               Status;

  if (This == NULL) { //safety check 
    InfoDebugPrint (INFO_ERROR_LEVEL, "%a(%d): safety check error\n", __FUNCTION__, __LINE__);
    return NULL;
  }
  //
  // init
  //
  CpuId = NULL;
  Addr  = NULL;
  Type4 = NULL;
  
  //
  // Get Type4 cpu information
  //
  Status = InfoGetSmbiosByType ( 
             This, 
             SMBIOS_TYPE_PROCESSOR_INFORMATION, 
             &Addr, 
             &Count
             );
  if (EFI_ERROR (Status)) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "%a(%d): Can't load TYPE4\n", __FUNCTION__, __LINE__);
    return NULL;
  }

  //
  // Get Cpu id info
  //
  Type4 = (SMBIOS_TABLE_TYPE4 *)(UINTN) Addr[0];
  *Size = sizeof (PROCESSOR_ID_DATA);
  CpuId = AllocateZeroPool (sizeof (PROCESSOR_ID_DATA));
  if (CpuId == NULL) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "%a(%d): Allocate memory failed\n", __FUNCTION__, __LINE__);
    return NULL;
  }
  CopyMem (CpuId, &Type4->ProcessorId, sizeof (PROCESSOR_ID_DATA));
  InfoDebugPrint (INFO_INFO_LEVEL, "CPU ID:%lx\n", *(UINT64 *)CpuId);
  return CpuId;
}

/**
    Filter the cpu Freq and memory Freq related info
    @param This                     - input pointer of EFI_SMBIOS_PROTOCOL
    @param  *Size                   - The size of cpu id
    @retval UINT8 *                 - The returned ptr of cpu id.
**/
UINT8 *
InfoFilterFreq (
  IN   EFI_SMBIOS_TABLE_HEADER          *Head,
  IN   UINTN                            Size
  ) 
{
  SMBIOS_TABLE_TYPE4      *Type4;
  SMBIOS_TABLE_TYPE17      *Type17;
  
  Type4 = NULL;
  Type17 = NULL;
  if (Head == NULL) { //Safety check
    return NULL; 
  }

  //
  // if it's a cpu information type, need filter External Clock, Max Speed, Current Speed
  // 
  if (Head->Type == SMBIOS_TYPE_PROCESSOR_INFORMATION) {
    Type4 = (SMBIOS_TABLE_TYPE4      *) AllocateZeroPool (Size);
    CopyMem (Type4, Head, Size);
    Type4->ExternalClock = 0;
    Type4->MaxSpeed = 0;
    Type4->CurrentSpeed = 0;
    return (UINT8 *)Type4;
  }

  //
  // if it's a memory information type. need filter clock speed
  // 
  if (Head->Type == SMBIOS_TYPE_MEMORY_DEVICE) {
    Type17 = (SMBIOS_TABLE_TYPE17      *) AllocateZeroPool (Size);
    CopyMem (Type17, Head, Size);
    Type17->ConfiguredMemoryClockSpeed = 0;
    Type17->Speed = 0;
    return (UINT8 *)Type17;
  }

  return NULL;
}
/**
    Get all smbios type data in the type list
    @param This                     - input pointer of EFI_SMBIOS_PROTOCOL
    @param  *Size                   - The size of cpu id
    @retval UINT8 *                 - The returned ptr of cpu id.
**/

UINT8 *
InfoGetMeasureTypeData (
  IN  EFI_SMBIOS_PROTOCOL             *This,
  IN  UINT8                           *Typelist,
  IN  OUT UINTN                       *Size
  ) 
{
  UINT8              *Data;
  UINTN               Index;
  UINTN               TempSize;
  UINTN               TypeIndex;
  UINTN               Count;
  UINTN               *Addr;
  EFI_SMBIOS_TABLE_HEADER          *Head;
  UINT8               *TempData;
  UINTN               SumSize;
  EFI_STATUS          Status;
  
  if ((Typelist == NULL) || (This == NULL)) { // Safety check
    InfoDebugPrint (INFO_ERROR_LEVEL, "%a(%d): safety check error\n", __FUNCTION__, __LINE__);
    return NULL;
  }

  //
  //init
  //
  TypeIndex = 0;
  SumSize   = 0;
  Data      = NULL;
  do {
    //
    // If this is a end table, just return
    //
    if (Typelist[TypeIndex] == SMBIOS_TYPE_END_OF_TABLE) {
      break;
    }
    Count = 0;
    Addr = NULL;
    //
    // Get smbios table header
    //
    Status = InfoGetSmbiosByType ( 
             This, 
             Typelist[TypeIndex], 
             &Addr, 
             &Count
             );
    //InfoDebugPrint (INFO_INFO_LEVEL, "Type Count = %x\n", Count);
    if (EFI_ERROR (Status) || (Count == 0)) {
      InfoDebugPrint (INFO_PRINT_LEVEL, "Can't load type %d\n", Typelist[TypeIndex]);
      return NULL;
    }
    for (Index = 0; Index < Count; Index ++) {
      TempSize = SumSize;
      //
      // Calc the size of the smbios full data
      //
      SumSize += InfoGetSmbiosStructureSize ((EFI_SMBIOS_TABLE_HEADER *)Addr[Index]);
      if (Data != NULL) {
        Data = ReallocatePool (TempSize, SumSize, Data);
      } else {
        Data = AllocateZeroPool (SumSize);
      }
      if (Data == NULL) {
        InfoDebugPrint (INFO_ERROR_LEVEL, "%a(%d): Allocate memory failed\n", __FUNCTION__, __LINE__);
        return NULL;
      }
      Head = (EFI_SMBIOS_TABLE_HEADER *)Addr[Index];
      if ((Head->Type == SMBIOS_TYPE_PROCESSOR_INFORMATION) || (Head->Type == SMBIOS_TYPE_MEMORY_DEVICE)) {
        TempData = InfoFilterFreq (Head, SumSize-TempSize);
        if (TempData == NULL) {
          InfoDebugPrint (INFO_ERROR_LEVEL, "%a(%d): filter Freq failed\n", __FUNCTION__, __LINE__);
          continue;
        }
        CopyMem (Data + TempSize, TempData, SumSize-TempSize);
        if (TempData != NULL) {
          FreePool (TempData);
        }
      } else {
        CopyMem (Data + TempSize, (UINT8 *)(UINTN)Addr[Index], SumSize-TempSize);
      }
    }
    if (Addr != NULL) {
      FreePool (Addr);
    }
    TypeIndex ++;
  } while (TRUE);
  *Size = SumSize;
  return Data;
}

/**
    Get the input smbios type list, if no "-t" cmd, used the default type list(0,1,2,3,17)
    @param  ArgStr                     - input pointer of input args
    @param  ArgCount                   - The count of the input args
    @retval UINT8 *                    - The returned ptr of smbios type list, null for failed
**/

UINT8 *
InfoBuildMeasureTypeList (
  IN  CHAR16             **ArgStr,
  IN  UINTN              ArgCount
  ) 
{
  UINT8             *TypeList;
  UINTN             ArgIndex;
  UINTN             TypeIndex;
  BOOLEAN           IsFound;
  UINT8             Type;
  UINTN             Index;
  if (ArgStr == NULL) { //safety check
    return mDefaultTypeList;
  }

  //
  // Init
  //
  ArgIndex  = 0;
  TypeIndex = 0;
  Index     = 0;
  IsFound   = FALSE;
  //
  // Get the "-t" cmd
  //
  do {
    if (StrCmp (ArgStr[ArgIndex], L"-t") == 0) {
      IsFound = TRUE;
      TypeIndex = ArgIndex+1;
    }
    if (IsFound) {
      if ((StrCmp(ArgStr[ArgIndex], L"-s") == 0) || (StrCmp(ArgStr[ArgIndex], L"-f") == 0) ||
          (StrCmp(ArgStr[ArgIndex], L"-o") == 0)) {
        break;
      }
    }
    ArgIndex++;
  } while (ArgIndex < ArgCount);
  //InfoDebugPrint (INFO_INFO_LEVEL, "TypeIndex:%d, ArgIndex:%d, ArgCount:%d\n",TypeIndex, ArgIndex,ArgCount);
  //
  //if not found the "-t" cmd, return default
  //
  if (!IsFound) {
    InfoDebugPrint (INFO_INFO_LEVEL, "no smbios input, used default(0,1,2,3,17)\n");
    return mDefaultTypeList;
  }
  if (ArgIndex < TypeIndex) {
    InfoDebugPrint (INFO_PRINT_LEVEL, "error input of the args\n");
    return NULL;
  }
  
  //
  //Prase the arg of smbios type
  TypeList = AllocateZeroPool (ArgIndex-TypeIndex + 1);
  if (TypeList == NULL) {
    InfoDebugPrint (INFO_ERROR_LEVEL, "%a(%d): Allocate memory failed\n", __FUNCTION__, __LINE__);
    return NULL;
  }
  Index = 0;
  do {
    if (ArgStr[TypeIndex] == NULL) { //safety check
      break;
    }
    Type = (UINT8)StrDecimalToUintn (ArgStr[TypeIndex]); // transfer decimal number to vaule
    if (Type > 0x7F) {
      InfoDebugPrint (INFO_PRINT_LEVEL, "Input type = %d begin than 0x7f\n", Type);
      return NULL;
    }
    TypeList[Index] = Type;
    Index ++;
    TypeIndex ++;
  } while (TypeIndex < ArgIndex );
  TypeList[Index] = 0x7f; //End of type list
  if (Index == 0) {
    InfoDebugPrint (INFO_PRINT_LEVEL, "error input of the args for cmd -t\n");
    return NULL;
  }
  InfoDebugPrint (INFO_INFO_LEVEL, "smbios type list = ");
  Index = 0;
  while (TRUE) {
    if (TypeList[Index] == 0x7F) {
      break;
    }
    InfoDebugPrint (INFO_INFO_LEVEL, "%d ", TypeList[Index]);
    Index ++;
  }
  InfoDebugPrint (INFO_INFO_LEVEL, "\n");
  return TypeList;
}


/**
    Get all smbios related data 
    @param This                        - input pointer of EFI_SMBIOS_PROTOCOL
    @param  ArgStr                     - input pointer of input args
    @param  ArgCount                   - The count of the input args
    @param Size                        - The size of data
    @retval UINT8 *                    - The returned ptr of data
**/

UINT8  *
InfoSmbiosRelatedData (
  IN  CHAR16             **ArgStr,
  IN  UINTN              ArgCount,
  IN  OUT  UINTN         *Size
  )
{
  UINT8                 *CpuData;
  UINT8                 *SmbiosData;
  UINTN                 TempSize;
  UINTN                 SumSize;
  EFI_STATUS            Status;
  EFI_SMBIOS_PROTOCOL   *Smbios;
  UINT8                 *TypeList;

  //
  //Init 
  //
  CpuData    = NULL;
  SmbiosData = NULL;
  TempSize   = 0;
  SumSize    = 0;

  //
  // Get smbios protocol and smbios max len
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  ASSERT_EFI_ERROR (Status);
  //InfoDebugPrint (INFO_INFO_LEVEL, "The Smbios Table Version: %x.%x\n", Smbios->MajorVersion, Smbios->MinorVersion);

  if (Smbios->MajorVersion < 2 || (Smbios->MajorVersion == 2 && Smbios->MinorVersion < 7)){
    mMaxLen = SMBIOS_STRING_MAX_LENGTH;
  } else if (Smbios->MajorVersion < 3) {
    //
    // Reference SMBIOS 2.7, chapter 6.1.3, it will have no limit on the length of each individual text string.
    // However, the length of the entire structure table (including all strings) must be reported
    // in the Structure Table Length field of the SMBIOS Structure Table Entry Point,
    // which is a WORD field limited to 65,535 bytes.
    //
    mMaxLen = SMBIOS_TABLE_MAX_LENGTH;
  } else {
    //
    // SMBIOS 3.0 defines the Structure table maximum size as DWORD field limited to 0xFFFFFFFF bytes.
    // Locate the end of string as long as possible.
    //
    mMaxLen = SMBIOS_3_0_TABLE_MAX_LENGTH;
  }

  //
  // Get smbios type4 cpu id
  //
  CpuData = InfoGetCpuID (Smbios, &TempSize);
  if (CpuData == NULL) {
    InfoDebugPrint (INFO_PRINT_LEVEL, "Can't get cpu id information\n");
    return NULL;
  }
  SumSize += TempSize;

  //
  // Build smbios type input list
  //
  TypeList = InfoBuildMeasureTypeList (ArgStr, ArgCount);
  if (TypeList == NULL) {
    InfoDebugPrint (INFO_PRINT_LEVEL, "Error prase the input type\n");
    return NULL;
  }

  //
  // Get the input smbios type  all data
  //
  SmbiosData = InfoGetMeasureTypeData (Smbios, TypeList, &TempSize);
  if (CpuData == NULL) {
    InfoDebugPrint (INFO_PRINT_LEVEL, "Can't get cpu id information\n");
    return NULL;
  }
  SumSize += TempSize;

  //
  //Add Smbios + CPUID
  //
  SmbiosData = ReallocatePool (TempSize, SumSize, SmbiosData);
  CopyMem (SmbiosData+TempSize, CpuData, SumSize-TempSize);
  *Size = SumSize;
  return SmbiosData;
}

/**
    Get all smbios related data 
    @param  TypeList                   - The typelist of smbios
    @param  Size                        - The size of data
    @retval UINT8 *                    - The returned ptr of data
**/

UINT8  *
InfoSmbiosRelatedData_ (
  IN  UINT8              *TypeList,
  IN  OUT  UINTN         *Size
  )
{
  UINT8                 *CpuData;
  UINT8                 *SmbiosData;
  UINTN                 TempSize;
  UINTN                 SumSize;
  EFI_STATUS            Status;
  EFI_SMBIOS_PROTOCOL   *Smbios;
  UINT8                 *List;

  //
  //Init 
  //
  CpuData    = NULL;
  SmbiosData = NULL;
  TempSize   = 0;
  SumSize    = 0;

  //
  // Get smbios protocol and smbios max len
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  ASSERT_EFI_ERROR (Status);
  //InfoDebugPrint (INFO_INFO_LEVEL, "The Smbios Table Version: %x.%x\n", Smbios->MajorVersion, Smbios->MinorVersion);

  if (Smbios->MajorVersion < 2 || (Smbios->MajorVersion == 2 && Smbios->MinorVersion < 7)){
    mMaxLen = SMBIOS_STRING_MAX_LENGTH;
  } else if (Smbios->MajorVersion < 3) {
    //
    // Reference SMBIOS 2.7, chapter 6.1.3, it will have no limit on the length of each individual text string.
    // However, the length of the entire structure table (including all strings) must be reported
    // in the Structure Table Length field of the SMBIOS Structure Table Entry Point,
    // which is a WORD field limited to 65,535 bytes.
    //
    mMaxLen = SMBIOS_TABLE_MAX_LENGTH;
  } else {
    //
    // SMBIOS 3.0 defines the Structure table maximum size as DWORD field limited to 0xFFFFFFFF bytes.
    // Locate the end of string as long as possible.
    //
    mMaxLen = SMBIOS_3_0_TABLE_MAX_LENGTH;
  }

  //
  // Get smbios type4 cpu id
  //
  CpuData = InfoGetCpuID (Smbios, &TempSize);
  if (CpuData == NULL) {
    InfoDebugPrint (INFO_PRINT_LEVEL, "Can't get cpu id information\n");
    return NULL;
  }
  SumSize += TempSize;

  //
  // Build smbios type input list
  //
  //TypeList = InfoBuildMeasureTypeList (ArgStr, ArgCount);
  //if (TypeList == NULL) {
  ///  InfoDebugPrint (INFO_PRINT_LEVEL, "Error prase the input type\n");
  //  return NULL;
  //}
  //
  //Debug meassge
  //
  List =  (TypeList == NULL)?mDefaultTypeList:TypeList;
  InfoDebugPrint (INFO_INFO_LEVEL, "SMBIOS type = ");
  while (TRUE) {
    if (*List == 0x7F) {
      break;
    }
    InfoDebugPrint (INFO_INFO_LEVEL, "%d ", *List);
    List ++;
  }
  InfoDebugPrint (INFO_INFO_LEVEL, "\n");
  //
  // Get the input smbios type  all data
  //
  SmbiosData = InfoGetMeasureTypeData (Smbios, (TypeList == NULL)?mDefaultTypeList:TypeList, &TempSize);
  if (CpuData == NULL) {
    InfoDebugPrint (INFO_PRINT_LEVEL, "Can't get cpu id information\n");
    return NULL;
  }
  SumSize += TempSize;

  //
  //Add Smbios + CPUID
  //
  SmbiosData = ReallocatePool (TempSize, SumSize, SmbiosData);
  CopyMem (SmbiosData+TempSize, CpuData, SumSize-TempSize);
  *Size = SumSize;
  return SmbiosData;
}

