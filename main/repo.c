
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



uint32_t REPO_HomePage(char **buf){
    return REPO_ReadFile((char*)HOME_PAGE_PATH, buf);    
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
#if 0
    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    ESP_LOGI(TAG, "Opening file");
    FILE* f = fopen("/spiffs/hello.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_ERR_NOT_FOUND;
    }
    fprintf(f, "SPIFFS says: Hello World!\n");
    fclose(f);
    ESP_LOGI(TAG, "File written");

    // Check if destination file exists before renaming
    struct stat st;
    if (stat("/spiffs/foo.txt", &st) == 0) {
        // Delete it if it exists
        unlink("/spiffs/foo.txt");
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return ESP_ERR_INVALID_ARG;
    }
    // All done, unmount partition and disable SPIFFS
    //esp_vfs_spiffs_unregister(NULL);
    //ESP_LOGI(TAG, "SPIFFS unmounted");
#endif

    //char *buf;
    //repoReadFile((char*)"/spiffs/index.html", &buf);
    return ESP_OK;
}



uint32_t REPO_ReadFile(char *filename, char **buf){
 // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file \"%s\"", filename);
    
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return 0;
    }
    
    //fgets(dst, len, f);
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    ESP_LOGI(TAG, "File size %u", size);
    fseek(fp, 0, SEEK_SET);

    *buf = (char *)malloc(size);
    if (*buf == NULL) {
        ESP_LOGE("SPIFFS", "Failed to allocate memory");
        return ESP_FAIL;
    }

    int br = fread(*buf, 1, size, fp);

    fclose(fp);

    if(br == 0)
    {
        ESP_LOGW(TAG, "No bytes read %u", ferror(fp));
        free(*buf);
        return 0;
    }

    ESP_LOGI(TAG, "%u bytes read", br);
    //ESP_LOGI(TAG, "Read from file: '%s'", file_buf);
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