


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
//#include <Protocol/ShellParameters.h>
//#include <Protocol/Shell.h>
#include <Protocol/Smbios.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/BaseCryptLib.h>
//#include <Library/ShellLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/FileHandleLib.h>
#include <Guid/FileInfo.h>

#include <Library/DxeServicesLib.h>
#include <Library/DxeServicesTableLib.h>

#include <Library/UefiDriverEntryPoint.h>
#include "fv.h"


#include "InfoSmbios.h"
#include "InfoPrint.h"
#include "InfoMac.h"

#endif
