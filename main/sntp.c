#include "sntp.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <esp_system.h>
#include <esp_log.h>

#include "lwip/apps/sntp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char *TAG = "SNTP";
static char *SERVER_NAME = "pool.ntp.org";

static struct tm timeinfo;
time_t now;

time_t SNTP_GetTime(void){
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        ESP_LOGI(TAG, "Initializing SNTP");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, SERVER_NAME);
        sntp_init();
         // wait for time to be set
    
        memset(&timeinfo, 0, sizeof(struct tm));
        int retry = 0;
        const int retry_count = 10;

        while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
            ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            time(&now);
            localtime_r(&now, &timeinfo);
        }
    }
    return now;
}

void SNTP_PrintTime(void){
char strftime_buf[64];
    
    SNTP_GetTime();

    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGE(TAG, "The current date/time error");
    } else {
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI(TAG, "The current date/time in Portugal is: %s", strftime_buf);
    }
}

void SNTP_Init(void){

    SNTP_GetTime();    
    
     // Set timezone to Portugal
    setenv("TZ", "UTC+0", 1);
    tzset();

    SNTP_PrintTime();
}