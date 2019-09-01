#ifndef _route_h_
#define _route_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <esp_system.h>

httpd_uri_t *ROUTE_GetUriList(void);


#ifdef __cplusplus
}
#endif

#endif