#include "browser.h"

#ifdef __MACH__  // macOS

#include <CoreFoundation/CFBundle.h>
#include <ApplicationServices/ApplicationServices.h>


void browser_openurl(const char *url_str) {
    CFIndex length = (CFIndex)strlen(url_str);
    CFURLRef url = CFURLCreateWithBytes(
        NULL,                        
        (UInt8*)url_str,                 // URLBytes
        length,            
        kCFStringEncodingASCII,      // encoding
        NULL                         // baseURL
    );
    LSOpenCFURLRef(url, 0);
    CFRelease(url);
}

#endif  /* __MACH__ */


#ifdef _WIN32   // Windows (32 or 64-bit)

#error Windows is not yet supported!
#include <windows.h>
#include <ShellApi.h>

/**
 * Tested? Absolutely not.
 */
void browser_openurl(const char *url_str) {
    ShellExecute(NULL, "open", "http://url", NULL, NULL, SW_SHOWNORMAL);
}

#endif /* _WIN32 */


#ifdef __linux__
#error Linux is not yet supported!
#endif /* __linux__ */

