

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>


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
  Print (L"HELLOWORLD\n");
  return EFI_SUCCESS;
}

