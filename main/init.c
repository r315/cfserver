#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include "stepper.h"
#include "repo.h"

static const char *TAG="INIT";

void server_init();
void app();
void initialize_console();

void app_main()
{  

    ESP_LOGI(TAG, "Initializing file system");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(REPO_Init());

    ESP_LOGI(TAG, "Starting wifi");
    server_init(); 

    ESP_LOGI(TAG, "Initializing console");
    initialize_console();   
   
    ESP_LOGI(TAG, "Initializing stepper driver");
    STEP_Init();

    app();
}
