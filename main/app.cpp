/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/



#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "cfserver.h"
#include "stepper.h"
#include "repo.h"
#include "json.h"
#include "route.h"
#include "sntp.h"

static const char *TAG="App";
static Cfserver server;

typedef struct _schedule_t{
    uint16_t qnt;
    uint64_t time;
    uint8_t repeat;
}schedule_t;

#define MAX_SCHEDULES 5

QueueHandle_t schedulers;


void onWifiConnected(void){
    SNTP_Init();
}

void onWifiDisconnect(void){
    
}

extern "C" void server_init(void)
{    
    char *ptr;
    uint8_t tmp[64];
    Json json;    

    if(REPO_ReadConfig(&ptr) > 0){
        ESP_ERROR_CHECK(json.init(ptr));
        if(json.string("ssid", tmp) > 0){
            server.set_ssid(tmp);
        }

        if(json.string("password", tmp) > 0){
            server.set_pass(tmp);
        }
        free(ptr);
    }

    server.tag = "CFSERVER";
    server.init();
    server.setUriList(ROUTE_GetUriList());
    server.onConnect = onWifiConnected;
    server.onDisconnect = onWifiDisconnect;
}


static void loadSchedules(void){
char *ptr;
uint8_t str[20];
Json json;

    if(schedulers == NULL){
        schedulers = xQueueCreate(MAX_SCHEDULES, sizeof(schedule_t));
    }

    if(schedulers == NULL){
        ESP_LOGE(TAG, "Unable to create schedulers queue");
        return;
    }

    if(REPO_GetSchedules(&ptr) > 0){
        ESP_ERROR_CHECK(json.init(ptr));
        
        while(json.nextToken(JSMN_OBJECT)){
            if(json.string("qnt", str) > 0){
                printf("qnt: %s\n", str);
            }

            if(json.string("repeat", str) > 0){
                printf("repeat: %s\n", str);
            }
            
            if(json.string("active", str) > 0){
                printf("active: %s\n", str);
            }

            if(json.string("time_t", str) > 0){
                printf("time_t: %s\n", str);
            }

        }
        free(ptr);
    }
}

extern "C" void app(void){
char *ptr;

    ESP_LOGI(TAG, "Running Application");

    schedulers = NULL;
    loadSchedules();

    while(1){
        vTaskDelay(1000);
    }
}