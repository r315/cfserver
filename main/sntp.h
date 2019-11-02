#ifndef _sntp_h_
#define _sntp_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

void SNTP_Init(void);
void SNTP_PrintTime(void);
time_t *SNTP_GetTime(void);


#ifdef __cplusplus
}
#endif


#endif