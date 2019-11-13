#ifndef __RSA_HAL_H__
#define __RSA_HAL_H__

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>

//
// Structures Definitions
//
struct tm {
  int   tm_sec;     /* seconds after the minute [0-60] */
  int   tm_min;     /* minutes after the hour [0-59] */
  int   tm_hour;    /* hours since midnight [0-23] */
  int   tm_mday;    /* day of the month [1-31] */
  int   tm_mon;     /* months since January [0-11] */
  int   tm_year;    /* years since 1900 */
  int   tm_wday;    /* days since Sunday [0-6] */
  int   tm_yday;    /* days since January 1 [0-365] */
  int   tm_isdst;   /* Daylight Savings Time flag */
  long  tm_gmtoff;  /* offset from CUT in seconds */
  char  *tm_zone;   /* timezone abbreviation */
};
#define strchr(str,ch)                    ScanMem8((VOID *)(str),AsciiStrSize(str),(UINT8)ch)

typedef INT32          time_t;
typedef UINT64 dbn_t;
typedef UINT32 bn_t;
typedef INT8     int8_t;
typedef INT16    int16_t;
typedef INT32    int32_t;
typedef INT64    int64_t;
typedef UINT8    uint8_t;
typedef UINT16   uint16_t;
typedef UINT32   uint32_t;
typedef UINT64   uint64_t;
typedef UINTN  size_t;
void * memcpyX (void *dest, const void *src, unsigned int count);
void * memsetX (void *dest, int ch, size_t count);
int strcmpX (const char *s1, const char *s2);
time_t timeX (time_t *timer);
void srand(unsigned int seed);
int rand();
size_t strlen (const char* str);
CHAR8 *
EFIAPI
AsciiStringCat (
  IN OUT CHAR8    *Destination,
  IN CONST CHAR8  *Source
  );

#endif

