#ifndef _repo_h_
#define _repo_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <esp_system.h>


esp_err_t REPO_Init(void);
uint32_t REPO_ReadFile(char *filename, char **buf);
uint32_t REPO_HomePage(char **buf);
uint32_t REPO_ReadConfig(char **buf);
esp_err_t REPO_ReadWifiConfig();
esp_err_t REPO_SaveWifiConfig();

#define REPO_ROOT "/spiffs"
#define HOME_PAGE_PATH REPO_ROOT"/index.html"
#define CFG_PATH REPO_ROOT"/wificfg.json"

#ifdef __cplusplus
}
#endif

#endif