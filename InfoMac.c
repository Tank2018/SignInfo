
#include "InfoMac.h"

/**
    Check if it's IPV4 device path
    @param  *Size                   - The size of mac address
    @retval UINT8 *                 - The returned ptr of mac address
**/

UINT8
IsEfiNetWorkType (
  IN  EFI_DEVICE_PATH_PROTOCOL   *Node
  )
{
  for (; !IsDevicePathEndType (Node); Node = NextDevicePathNode (Node)) {
    if (DevicePathType (Node) == MESSAGING_DEVICE_PATH) {
      if (DevicePathSubType (Node) == MSG_IPv4_DP) { // Only clac IPV4 address
        return TRUE;
      }
    }
  }
  return FALSE;
}


/**
    Get all mac address
    @param  *Size                   - The size of mac address
    @retval UINT8 *                 - The returned ptr of mac address
**/

UINT8 *
InfoGetMacData (
  IN  OUT  UINTN               *Size
  )
{
  EFI_STATUS                            Status;
  BOOLEAN                               IsEfiNetWork;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_HANDLE                            *Handles;
  UINTN                                 HandleCount;
  UINTN                                 Index;
  UINT8                                 *MacData;
  MAC_ADDR_DEVICE_PATH                  *Mac;
  UINTN                                 TempSize;
  UINTN                                 SumSize;

  //
  // Init
  //
  MacData  = NULL;
  Mac      = NULL;
  SumSize  = 0;
  TempSize = 0;
  //
  // Parse load file, assuming UEFI Network drivers
  //
  gBS->LocateHandleBuffer (
       ByProtocol,
       &gEfiLoadFileProtocolGuid,
       NULL,
       &HandleCount,
       &Handles
       );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol(
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DevicePath
                    );
    if (!EFI_ERROR (Status)) {
      IsEfiNetWork = IsEfiNetWorkType (DevicePath);

      while (!IsDevicePathEnd (DevicePath) &&
           ((DevicePathType (DevicePath) != MESSAGING_DEVICE_PATH) ||
            (DevicePathSubType (DevicePath) != MSG_MAC_ADDR_DP))
           ) {
        DevicePath = NextDevicePathNode (DevicePath);
      }

      if (IsDevicePathEnd (DevicePath)) {
        return NULL;
      }

      Mac = (MAC_ADDR_DEVICE_PATH *) DevicePath;
      if (!IsEfiNetWork) {
        continue;
      }
      TempSize = SumSize;
      if (MacData == NULL) {
        MacData = AllocateZeroPool (6); //Get 6 bytes of mac address
        SumSize += 6;
      } else {
        MacData = ReallocatePool (TempSize, SumSize, MacData);
        SumSize += 6;
      }
      CopyMem (MacData + TempSize, Mac->MacAddress.Addr, 6);
      *Size = SumSize;
      InfoDebugPrint (INFO_INFO_LEVEL, "Mac DATA %02x-%02x-%02x-%02x-%02x-%02x\n", 
           Mac->MacAddress.Addr[0], Mac->MacAddress.Addr[1], Mac->MacAddress.Addr[2],
            Mac->MacAddress.Addr[3], Mac->MacAddress.Addr[4], Mac->MacAddress.Addr[5]);
    }
  }
  if (SumSize == 0) {
    return NULL;
  }
  return MacData;
}
