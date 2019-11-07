#ifndef _repo_h_
#define _repo_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <esp_system.h>
#include "list.h"
#include "json.h"

#define REPO_MAX_SCHEDULES  5

#define REPO_ROOT       "/spiffs"
#define HOME_PAGE_PATH  REPO_ROOT"/index.html"
#define CFG_PATH        REPO_ROOT"/wificfg.json"
#define SCHEDULE_PATH   REPO_ROOT"/schedule.json"

#define SCHEDULE_T_CHARS    64

typedef struct _schedule_t{
    uint16_t qnt;              //5 chars
    uint8_t repeat;            //3 chars
    uint64_t time;             //20 chars
}schedule_t;


uint32_t REPO_WriteFile(char *filename, char *buf, uint32_t len);
uint32_t REPO_ReadFile(char *filename, char **buf);
uint32_t REPO_ReadConfig(char **buf);
uint32_t REPO_GetHomePage(char **buf);
uint32_t REPO_GetSchedules(char **buf);
uint32_t REPO_PostSchedule(char *data, uint32_t len);
uint32_t REPO_DeleteSchedule(char *data, uint32_t len);

esp_err_t REPO_Init(void);
esp_err_t REPO_ReadWifiConfig(void);
esp_err_t REPO_SaveWifiConfig(void);

schedule_t *REPO_FirstSchedule(void);

#ifdef __cplusplus
}
#endif

#endif
