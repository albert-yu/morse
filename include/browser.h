#ifndef BROWSER_H
#define BROWSER_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opens the default browser with the
 * specified URL
 */
void browser_openurl(const char *url_str);


#ifdef __cplusplus
}
#endif

#endif

