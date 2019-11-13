#ifndef __INFO_SMBIOS_H__
#define __INFO_SMBIOS_H__
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/FileHandleLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Protocol/Smbios.h>
//#include <IndustryStandard/Smbios.h>
#include "InfoPrint.h"

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
  );

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
  );

#endif
