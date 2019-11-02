
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <esp_log.h>
#include <esp_system.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "repo.h"



uint32_t REPO_ReadConfig(char **buf){
    return REPO_ReadFile((char*)CFG_PATH, buf);    
}

uint32_t REPO_HomePage(char **buf){
    return REPO_ReadFile((char*)HOME_PAGE_PATH, buf);
}

uint32_t REPO_GetSchedules(char **buf){
    return REPO_ReadFile((char*)SCHEDULE_PATH, buf);
}

uint32_t REPO_PostSchedule(){
    return 0;
}

uint32_t REPO_DeleteSchedule(){
    return 0;
}

/**
 * https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/api-guides/partition-tables.html
 * https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/spiffs.html
 * $ make partition_table
 * $ make partition_table-flash
 * $ make partition_data-flash
 * 
 * 
 * mkspiffs ver. 0.2.3-6-g983970e
 * Build configuration name: generic
 * SPIFFS ver. 0.3.7-5-gf5e26c4
 * Extra build flags: (none)
 * SPIFFS configuration:
 * SPIFFS_OBJ_NAME_LEN: 32
 * SPIFFS_OBJ_META_LEN: 0
 * SPIFFS_USE_MAGIC: 1
 * SPIFFS_USE_MAGIC_LENGTH: 1
 * SPIFFS_ALIGNED_OBJECT_INDEX_TABLES: 4
 * 
 */



static const char *TAG = "REPO";

static esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
};

esp_err_t REPO_Init(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");   
    
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}

/**
 * Note This adds one extra byte to the file content for ending strings.
 * for data files the returned sized must be used
 * */
uint32_t REPO_ReadFile(char *filename, char **buf){
 
    char *ptr;

    ESP_LOGI(TAG, "Reading file \"%s\"", filename);
    
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return 0;
    }
    
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    //ESP_LOGI(TAG, "File size %u", size);
    fseek(fp, 0, SEEK_SET);

    ptr = (char *)malloc(size + 1);
    if (ptr == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory");
        return ESP_FAIL;
    }

    int br = fread(ptr, 1, size, fp);
    ptr[size] = '\0';   // End file, note for data files the returned sized must be used

    fclose(fp);

    if(br == 0)
    {
        ESP_LOGW(TAG, "No bytes read %u", ferror(fp));
        free(ptr);
        return 0;
    }

    ESP_LOGI(TAG, "%u bytes read", br);
    *buf = ptr;
    return size;
}


/*

esp_err_t repoFileResponse(httpd_req_t *req, char *filename){
    ESP_LOGI(TAG, "Reading file \"%s\"", filename);
    
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_ERR_NOT_FOUND;
    }

    //https://github.com/espressif/esp-idf/issues/3363
    return ESP_OK;
}

*/