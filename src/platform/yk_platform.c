#include <platform/yk_platform.h>

#if _WIN32
#include <Windows.h>
#endif


void platform_innit()
{
    // FUCK MICROSOFT (John Malkovitch voice)
#if _WIN32
    SetProcessDPIAware();
#endif
    
}