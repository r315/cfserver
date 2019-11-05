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

#include "list.h"
#include "cfserver.h"
#include "stepper.h"
#include "repo.h"
#include "json.h"
#include "route.h"
#include "sntp.h"
#include "dal.h"

static const char *TAG="App";
static Cfserver server;

void onWifiConnected(void){
    SNTP_Init();
}

void onWifiDisconnect(void){
    
}

extern "C" void server_init(void){    
char *ptr;
uint8_t tmp[64];
Json js;

    if(REPO_ReadConfig(&ptr) > 0){
        ESP_ERROR_CHECK(JSON_init(&js, ptr));
        if(JSON_string(&js, "ssid", tmp) > 0){
            server.set_ssid(tmp);
        }

        if(JSON_string(&js, "password", tmp) > 0){
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

#if 0
static void schToString(char *dst, schedule_t *sch){
    memcpy(dst, "{}", 2);     
}

static void saveSchedulers(node_t *head){
    uint32_t count = countNodes(head);
    char *jstr = (char*)malloc((SCHEDULE_T_CHARS + 2) * count);
    *(jstr++) = '[';
    for(uint32_t i = 0; i < count; i++){
        schedule_t *sch = (schedule_t*)(head->next->value);
        schToString(jstr, sch);
        jstr +=SCHEDULE_T_CHARS + 2; // plus brackets
    }
}
#endif


extern "C" void app(void){
time_t now;
schedule_t *sch;

    ESP_LOGI(TAG, "Running Application");

    while(1){
        now = SNTP_GetTime();
        sch = REPO_FirstSchedule();
        if(sch->time == now){            
                printf("feeding\n");
        }        
        vTaskDelay(1000);
    }
}