
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
#include "list.h"
#include "json.h"
#include "dal.h"

static node_t list;
static const char *TAG = "REPO";

/**
 * Private API
 * */
static int32_t REPO_InsertScheduleFromJson(Json *js, node_t *head);
static uint32_t REPO_SaveSchedules(node_t *head);
uint32_t REPO_FreeSchedules(void){
    return REPO_MAX_SCHEDULES - countNodes(&list);
}

/**
 * Public API
 * */
uint32_t REPO_ReadConfig(char **buf){
    return REPO_ReadFile((char*)CFG_PATH, buf);    
}

uint32_t REPO_GetHomePage(char **buf){
    return REPO_ReadFile((char*)HOME_PAGE_PATH, buf);
}

uint32_t REPO_GetSchedules(char **buf){
	char *jstr = DAL_ListToJson(&list);
	if(jstr != NULL){
		*buf = jstr;
		return strlen(jstr);
	}
	return 0;
}

uint32_t REPO_PostSchedule(char *data, uint32_t len){
Json js;

    // json expected, ensure terminated string
    data[len] = '\0';

    if(REPO_FreeSchedules()){
        if(JSON_init(&js, data) == ESP_OK){
            //ESP_LOGI(TAG,"%s\n",data);
        	if(REPO_InsertScheduleFromJson(&js, &list) >= 0)        		
        		return REPO_SaveSchedules(&list);
    }
    }
    return 0;
}

uint32_t REPO_DeleteSchedule(){
    return 0;
}


/**
 * Return the first schedule on the list. The list is not public, so public 
 * API does not require passing a list
 * \return pointer to first schedule on list, or null is list is empty
 * */
schedule_t *REPO_FirstSchedule(void){
node_t *node = list.next;
    if(node == NULL)
        return NULL;
    return (schedule_t*)(node->value);
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
static esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
};

/**
 * Reads schedules from json file and 
 * loads them into local list
 * \param   head  pointer to list head
 * */
static void REPO_LoadSchedules(node_t *head, char *filename){
char *jstr;
Json js;
    if(REPO_ReadFile(filename, &jstr) > 0){
        ESP_ERROR_CHECK(JSON_init(&js, jstr));        
        while(JSON_nextToken(&js, JSMN_OBJECT)){
            REPO_InsertScheduleFromJson(&js, head);    
        }
        free(jstr);
    }
}

/**
 * 
 * */
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

    ESP_LOGI(TAG, "Loading schedules");
    REPO_LoadSchedules(&list, (char*)SCHEDULE_PATH);
    return ESP_OK;
}

/**
 * Read file content to allocated memory, must be freed after used
 * Note This adds one extra byte to the file content for ending strings.
 * for data files the returned sized must be used
 *
 * \param filename  name of the file to be accessed
 * \param buf       pointer to destination buffer
 * \return          number of bytes read
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

/**
 * \param filename  name of the file to be accessed
 * \param buf       pointer to destination buffer
 * \return          number of bytes written
 * */
uint32_t REPO_WriteFile(char *filename, char *buf, uint32_t len){
 
    ESP_LOGI(TAG, "Writing to file \"%s\"", filename);
    
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to open file");
        return 0;
    }    

    int bw = fwrite(buf, 1, len, fp);

    fclose(fp);

    if(bw == 0)
    {
        ESP_LOGW(TAG, "No bytes written %u", ferror(fp));        
        return 0;
    }

    ESP_LOGI(TAG, "%u bytes written", bw);
    return bw;    
}

/**
 * Try to create and insert an schedule into the schedules list
 * node and schedule should be free when removed from list
 * 
 * \param js    pointer to json object
 * \param head  head of the list to insert schedule
 * \return      index of inserted position, -1 if fail
 * */
static int32_t REPO_InsertScheduleFromJson(Json *js, node_t *head){
schedule_t *sch = DAL_JsonToSchedule(js);
int32_t inserted = -1;

    if(sch == NULL){
        ESP_LOGE(TAG,"Unable to create schedule");			    
    }else{                
        node_t *node = createNode(sch);
        if(node == NULL){
            ESP_LOGE(TAG,"Unable to create node");
            free(sch);
        }else{
            inserted = insertTail(head, node);
            ESP_LOGI(TAG,"Schedule count :%d", countNodes(head));
        }
    }
    return inserted;
}

/**
 * Convert a schedules list to JSON and save to file on spiffs
 * 
 * \param head	list to be saved
 * \return number of bytes written to file, 0 if fail
 */
static uint32_t REPO_SaveSchedules(node_t *head){
char *jstr = DAL_ListToJson(head);
uint32_t count = 0;
	if(jstr != NULL){		
		count = strlen(jstr);
		count = REPO_WriteFile((char*)SCHEDULE_PATH, jstr, count);
		free(jstr);
	}
	return count;
}

