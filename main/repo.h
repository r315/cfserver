#ifndef _repo_h_
#define _repo_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <esp_system.h>


#define REPO_ROOT "/spiffs"
#define HOME_PAGE_PATH REPO_ROOT"/index.html"
#define CFG_PATH REPO_ROOT"/wificfg.json"
#define SCHEDULE_PATH REPO_ROOT"/schedule.json"

uint32_t REPO_ReadFile(char *filename, char **buf);
uint32_t REPO_HomePage(char **buf);
uint32_t REPO_ReadConfig(char **buf);
uint32_t REPO_Schedules(char **buf);

esp_err_t REPO_Init(void);
esp_err_t REPO_ReadWifiConfig();
esp_err_t REPO_SaveWifiConfig();

#ifdef __cplusplus
}
#endif

#endif