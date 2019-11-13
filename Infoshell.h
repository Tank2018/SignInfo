#ifndef  ___INFO_H__ //special for name
#define  ___INFO_H__

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Guid/GlobalVariable.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/Shell.h>
#include <Protocol/Smbios.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/BaseCryptLib.h>
#include <Library/ShellLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/FileHandleLib.h>
#include <Guid/FileInfo.h>

#ifndef EFI_SHELL_APP
  #include <Library/DxeServicesLib.h>
    #include <Library/DxeServicesTableLib.h>

  #include <Library/UefiDriverEntryPoint.h>
#include "fv.h"

#endif

#include "InfoSmbios.h"
#include "InfoPrint.h"
#include "InfoMac.h"


static CHAR8  *sInfo      = "Info tool:\n";
static CHAR8  *sInfoUsage  =   "\n\
  Usage:\n\
    -s ---------- set debug level\n\
    -t ---------- smbios type list\n\
    -f ---------- save the hash data into txt\n\
    -o ---------- save the output SHA256 hash into txt\n\
    -e ---------- enc the hash info into txt\n\
    -r----------- reboot the system\n\
  ex:\n\
    info.efi -s ff\n\
    info.efi -h \n\
    info.efi -f save.txt -o hash.txt\n\
    info.efi -t 1 3 17 -f save.txt -o hash.txt\n\
    info.efi -e enc.txt\n\
    info.efi -r\n\
  debug level:\n\
    All   message:  0xFF\n\
    Info  message:  0x04\n\
    Error message: 0x02\n\
    Print message: 0x10\n"; 
#endif
