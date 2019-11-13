#include "hal.h"
/** Expands to an integer constant expression that is the maximum value
    returned by the rand function.
**/
#define RAND_MAX  0x7fffffff

/* OpenSSL will use floating point support, and C compiler produces the _fltused
   symbol by default. Simply define this symbol here to satisfy the linker. */
int _fltused1= 1;
/* Copies bytes between buffers */
void * memcpyX (void *dest, const void *src, unsigned int count)
{
  return CopyMem (dest, src, (UINTN)count);
}

/* Sets buffers to a specified character */
void * memsetX (void *dest, int ch, size_t count)
{
  //
  // NOTE: Here we use one base implementation for memset, instead of the direct
  //       optimized SetMem() wrapper. Because the IntrinsicLib has to be built
  //       without whole program optimization option, and there will be some
  //       potential register usage errors when calling other optimized codes.
  //

  //
  // Declare the local variables that actually move the data elements as
  // volatile to prevent the optimizer from replacing this function with
  // the intrinsic memset()
  //
  volatile UINT8  *Pointer;

  Pointer = (UINT8 *)dest;
  while (count-- != 0) {
    *(Pointer++) = (UINT8)ch;
  }

  return dest;
}

/* Compare bytes in two buffers. */
int memcmpX (const void *buf1, const void *buf2, size_t count)
{
  return (int)CompareMem(buf1, buf2, count);
}

int strcmpX (const char *s1, const char *s2)
{
  return (int)AsciiStrCmp(s1, s2);
}
static UINT32 next = 1;

/** Compute a pseudo-random number.
  *
  * Compute x = (7^5 * x) mod (2^31 - 1)
  * without overflowing 31 bits:
  *      (2^31 - 1) = 127773 * (7^5) + 2836
  * From "Random number generators: good ones are hard to find",
  * Park and Miller, Communications of the ACM, vol. 31, no. 10,
  * October 1988, p. 1195.
**/
int
rand()
{
  INT32 hi, lo, x;

  /* Can't be initialized with 0, so use another value. */
  if (next == 0)
    next = 123459876;
  hi = next / 127773;
  lo = next % 127773;
  x = 16807 * lo - 2836 * hi;
  if (x < 0)
    x += 0x7fffffff;
  return ((next = x) % ((UINT32)RAND_MAX + 1));
}

void
srand(unsigned int seed)
{
  next = (UINT32)seed;
}

size_t strlen (const char* str) {
  return AsciiStrLen (str);
}

/**
  [ATTENTION] This function will be deprecated for security reason.

  Copies one Null-terminated ASCII string to another Null-terminated ASCII
  string and returns the new ASCII string.

  This function copies the contents of the ASCII string Source to the ASCII
  string Destination, and returns Destination. If Source and Destination
  overlap, then the results are undefined.

  If Destination is NULL, then ASSERT().
  If Source is NULL, then ASSERT().
  If Source and Destination overlap, then ASSERT().
  If PcdMaximumAsciiStringLength is not zero and Source contains more than
  PcdMaximumAsciiStringLength ASCII characters, not including the Null-terminator,
  then ASSERT().

  @param  Destination A pointer to a Null-terminated ASCII string.
  @param  Source      A pointer to a Null-terminated ASCII string.

  @return Destination

**/
CHAR8 *
EFIAPI
AsciiStringCopy (
  OUT     CHAR8                     *Destination,
  IN      CONST CHAR8               *Source
  )
{
  CHAR8                             *ReturnValue;

  //
  // Destination cannot be NULL
  //
  ASSERT (Destination != NULL);

  //
  // Destination and source cannot overlap
  //
  ASSERT ((UINTN)(Destination - Source) > AsciiStrLen (Source));
  ASSERT ((UINTN)(Source - Destination) > AsciiStrLen (Source));

  ReturnValue = Destination;
  while (*Source != 0) {
    *(Destination++) = *(Source++);
  }
  *Destination = 0;
  return ReturnValue;
}

/**
  [ATTENTION] This function will be deprecated for security reason.

  Concatenates one Null-terminated ASCII string to another Null-terminated
  ASCII string, and returns the concatenated ASCII string.

  This function concatenates two Null-terminated ASCII strings. The contents of
  Null-terminated ASCII string Source are concatenated to the end of Null-
  terminated ASCII string Destination. The Null-terminated concatenated ASCII
  String is returned.

  If Destination is NULL, then ASSERT().
  If Source is NULL, then ASSERT().
  If PcdMaximumAsciiStringLength is not zero and Destination contains more than
  PcdMaximumAsciiStringLength ASCII characters, not including the Null-terminator,
  then ASSERT().
  If PcdMaximumAsciiStringLength is not zero and Source contains more than
  PcdMaximumAsciiStringLength ASCII characters, not including the Null-terminator,
  then ASSERT().
  If PcdMaximumAsciiStringLength is not zero and concatenating Destination and
  Source results in a ASCII string with more than PcdMaximumAsciiStringLength
  ASCII characters, then ASSERT().

  @param  Destination A pointer to a Null-terminated ASCII string.
  @param  Source      A pointer to a Null-terminated ASCII string.

  @return Destination

**/
CHAR8 *
EFIAPI
AsciiStringCat (
  IN OUT CHAR8    *Destination,
  IN CONST CHAR8  *Source
  )
{
  AsciiStringCopy (Destination + AsciiStrLen (Destination), Source);

  //
  // Size of the resulting string should never be zero.
  // PcdMaximumUnicodeStringLength is tested inside StrLen().
  //
  ASSERT (AsciiStrSize (Destination) != 0);
  return Destination;
}


//
// -- Time Management Routines --
//

#define IsLeap(y)   (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))
#define SECSPERMIN  (60)
#define SECSPERHOUR (60 * 60)
#define SECSPERDAY  (24 * SECSPERHOUR)

//
//  The arrays give the cumulative number of days up to the first of the
//  month number used as the index (1 -> 12) for regular and leap years.
//  The value at index 13 is for the whole year.
//
UINTN CumulativeDaysX[2][14] = {
  {
    0,
    0,
    31,
    31 + 28,
    31 + 28 + 31,
    31 + 28 + 31 + 30,
    31 + 28 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
  },
  {
    0,
    0,
    31,
    31 + 29,
    31 + 29 + 31,
    31 + 29 + 31 + 30,
    31 + 29 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31
  }
};

/* Get the system time as seconds elapsed since midnight, January 1, 1970. */
//INTN time(
//  INTN *timer
//  )
time_t timeX (time_t *timer)
{
  EFI_STATUS  Status;
  EFI_TIME    Time;
  time_t      CalTime;
  UINTN       Year;

  //
  // Get the current time and date information
  //
  Status = gRT->GetTime (&Time, NULL);
  if (EFI_ERROR (Status) || (Time.Year < 1970)) {
    return 0;
  }

  //
  // Years Handling
  // UTime should now be set to 00:00:00 on Jan 1 of the current year.
  //
  for (Year = 1970, CalTime = 0; Year != Time.Year; Year++) {
    CalTime = CalTime + (time_t)(CumulativeDaysX[IsLeap(Year)][13] * SECSPERDAY);
  }

  //
  // Add in number of seconds for current Month, Day, Hour, Minute, Seconds, and TimeZone adjustment
  //
  CalTime = CalTime +
            (time_t)((Time.TimeZone != EFI_UNSPECIFIED_TIMEZONE) ? (Time.TimeZone * 60) : 0) +
            (time_t)(CumulativeDaysX[IsLeap(Time.Year)][Time.Month] * SECSPERDAY) +
            (time_t)(((Time.Day > 0) ? Time.Day - 1 : 0) * SECSPERDAY) +
            (time_t)(Time.Hour * SECSPERHOUR) +
            (time_t)(Time.Minute * 60) +
            (time_t)Time.Second;

  if (timer != NULL) {
    *timer = CalTime;
  }

  return CalTime;
}
#if 0

//
// Convert a time value from type time_t to struct tm.
//
struct tm * gmtime (const time_t *timer)
{
  struct tm  *GmTime;
  UINT16     DayNo;
  UINT16     DayRemainder;
  time_t     Year;
  time_t     YearNo;
  UINT16     TotalDays;
  UINT16     MonthNo;

  if (timer == NULL) {
    return NULL;
  }

  GmTime = malloc (sizeof (struct tm));
  if (GmTime == NULL) {
    return NULL;
  }

  ZeroMem ((VOID *) GmTime, (UINTN) sizeof (struct tm));

  DayNo        = (UINT16) (*timer / SECSPERDAY);
  DayRemainder = (UINT16) (*timer % SECSPERDAY);

  GmTime->tm_sec  = (int) (DayRemainder % SECSPERMIN);
  GmTime->tm_min  = (int) ((DayRemainder % SECSPERHOUR) / SECSPERMIN);
  GmTime->tm_hour = (int) (DayRemainder / SECSPERHOUR);
  GmTime->tm_wday = (int) ((DayNo + 4) % 7);

  for (Year = 1970, YearNo = 0; DayNo > 0; Year++) {
    TotalDays = (UINT16) (IsLeap (Year) ? 366 : 365);
    if (DayNo >= TotalDays) {
      DayNo = (UINT16) (DayNo - TotalDays);
      YearNo++;
    } else {
      break;
    }
  }

  GmTime->tm_year = (int) (YearNo + (1970 - 1900));
  GmTime->tm_yday = (int) DayNo;

  for (MonthNo = 12; MonthNo > 1; MonthNo--) {
    if (DayNo >= CumulativeDays[IsLeap(Year)][MonthNo]) {
      DayNo = (UINT16) (DayNo - (UINT16) (CumulativeDays[IsLeap(Year)][MonthNo]));
      break;
    }
  }

  GmTime->tm_mon  = (int) MonthNo - 1;
  GmTime->tm_mday = (int) DayNo + 1;

  GmTime->tm_isdst  = 0;
  GmTime->tm_gmtoff = 0;
  GmTime->tm_zone   = NULL;

  return GmTime;
}
#endif
