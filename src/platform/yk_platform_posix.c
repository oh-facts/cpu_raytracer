

#if defined(__APPLE__)
    #define YK_UNIX 1
#elif defined(__linux__)
    #define YK_UNIX 1
#endif

#if YK_UNIX
#include <platform/yk_platform.h>

void yk_clock_innit(struct YkClockRaw *self)
{
   // printf("bleh bleh bleh unimplemented");
}

f64 yk_get_time_since(struct YkClockRaw *self)
{
   // printf("bleh bleh bleh unimplemented");
    return 69;
}
#endif




