#ifndef __INFO_MAC_H__
#define __INFO_MAC_H__
#include <PiDxe.h>
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
#include <Protocol/LoadedImage.h>
#include <IndustryStandard/Pci.h>
#include <Library/DevicePathLib.h>
#include "InfoPrint.h"


/**
    Get all mac address
    @param  *Size                   - The size of mac address
    @retval UINT8 *                 - The returned ptr of mac address
**/

UINT8 *
InfoGetMacData (
  IN  OUT  UINTN               *Size
  );
#endif