
#include "InfoPrint.h"
UINT32  mInfoPrintLevel = INFO_PRINT_LEVEL | INFO_ERROR_LEVEL;

/**

    this function is used to  set print debug level.

    @param  TIDebugLevel   -  system configuration debug level.

    @retval none

**/
void
InfoSetDebugPrintLevel (
  IN  UINT32      Level
  )
{
  mInfoPrintLevel = Level;
}

/**

    this function is used to  set print debug level.

    @param  none.

    @retval TIDebugLevel   -  system configuration debug level.

**/
UINT32
InfoGetDebugPrintLevel (
  VOID
  )
{
  return mInfoPrintLevel;
}

/**

    this function is used to print debug info.

    @param  ScDebugLevel   -  system configuration debug level.
    @param  EfiErrorLevel   - EFI lib print level
    @param  Format          - Format string for the debug message to print.
    @param  ...             - A variable argument list whose contents are accessed based on the format string specified by Format.

    @retval none

**/
void
EFIAPI
InfoDebugPrint (
  IN  UINT32             Level,
  IN  CHAR8       *Format,
  ...
  )
{

  CHAR16        Buffer[0x200];
  VA_LIST       Marker;
  
  if ((Level & mInfoPrintLevel) == 0) {
    return;
  }
  //
  // Convert the DEBUG() message to an ASCII String
  //
  VA_START (Marker, Format);
  UnicodeVSPrintAsciiFormat (Buffer, sizeof (Buffer), Format, Marker);
  VA_END (Marker);

  //
  // Send the print string to the Console Output device
  //
  if ((gST != NULL) && (gST->ConOut != NULL)) {
    gST->ConOut->OutputString (gST->ConOut, Buffer);
  }
}


