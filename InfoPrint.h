
#ifndef __TI_DEBUG_LIB_H__
#define __TI_DEBUG_LIB_H__

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugPrintErrorLevelLib.h>

//
// Debug print level
//
#define INFO_ALL_LEVEL               0xFF
#define INFO_ERROR_LEVEL             0x04
#define INFO_WARNING_LEVEL           0x02
#define INFO_INFO_LEVEL              0x01
#define INFO_PRINT_LEVEL             0x10




/**

    this function is used to  set print debug level.

    @param  TIDebugLevel   -  system configuration debug level.

    @retval none

**/
void
InfoSetDebugPrintLevel (
  IN  UINT32      Level
  );


/**

    this function is used to  set print debug level.

    @param  none.

    @retval TIDebugLevel   -  system configuration debug level.

**/
UINT32
InfoGetDebugPrintLevel (
  VOID
  );

/**

    this function is used to print debug info.

    @param  ScDebugLevel   -  system configuration debug level.
    @param  EfiErrorLevel   -  EFI lib print level
    @param  Format       - Format string for the debug message to print.
    @param  ...          - A variable argument list whose contents are accessed based on the format string specified by Format.

    @retval none

**/
void
EFIAPI
InfoDebugPrint (
  IN  UINT32              Level,
  IN  CHAR8       *Format,
  ...
  );



#endif

