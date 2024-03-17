#include <final_platform_layer.h>
#include <stdio.h>

#include <yk_common.h>
#include <math.h>
int main(int argc, char* argv[])
{
    
    fplSettings settings = fplMakeDefaultSettings();
    settings.video.backend = fplVideoBackendType_Software;
    
    fplCopyString("Software Rendering Example", settings.window.title, fplArrayCount(settings.window.title));
    
    if (fplPlatformInit(fplInitFlags_All, &settings)) {
        // your code here
        while(fplWindowUpdate())
        {
            
            fplPollEvents();
            
            fplVideoBackBuffer* buffer = fplGetVideoBackBuffer();
            
            for(u32 y = 0; y < buffer->height; y ++)
            {
                
                for(u32 x = 0; x < buffer->width; x ++)
                {
                    buffer->pixels[y*buffer->width + x] = (0xFF << 24) | (rand() << 16) | (rand() << 8) | rand();
                }
                
            }
            fplVideoFlip();
            
        }
        fplPlatformRelease();
    } else {
        fplPlatformResultType initResult = fplGetPlatformResult();
        const char *initResultStr = fplPlatformGetResultName(initResult);
        const char *errStr = fplGetLastError();
        fplConsoleFormatError("FPL-ERROR[%s]: %s\n", initResultStr, errStr);
    }
    
    return 0;
}