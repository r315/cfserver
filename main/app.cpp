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

static const char *TAG="App";
static Cfserver server;

typedef struct _schedule_t{
    uint16_t qnt;              //5 chars
    uint8_t repeat;            //3 chars
    uint64_t time;             //20 chars
}schedule_t;

#define SCHEDULE_T_CHARS    28


void onWifiConnected(void){
    SNTP_Init();
}

void onWifiDisconnect(void){
    
}

extern "C" void server_init(void){    
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

static void loadSchedules(node_t *head){
Json json;
char *jstr;
uint8_t tmp[10];

    if(REPO_GetSchedules(&jstr) > 0){
        ESP_ERROR_CHECK(json.init(jstr));
        
        while(json.nextToken(JSMN_OBJECT)){
            schedule_t *sch = (schedule_t*)malloc(SCHEDULE_T_CHARS);
		    if(sch == NULL){
			    ESP_LOGE(TAG,"Unable to allocate schedule");			    
		    }else{
                if(json.string("qnt", tmp) > 0){
                    sch->qnt = atoi((const char *)tmp);                    
                }

                if(json.string("repeat", tmp) > 0){
                    sch->repeat = atoi((const char *)tmp);                    
                }

                if(json.string("time_t", tmp) > 0){
                    sch->time = atol((const char *)tmp);                    
                }                
                node_t *node = createNode(sch);
                if(node == NULL){
			        ESP_LOGE(TAG,"Unable to allocate node");
                }else{
                    insertTail(head, node);
                }
            }
        }
        free(jstr);
    }
}

extern "C" void app(void){
time_t now;
schedule_t *sch;
node_t list;

    ESP_LOGI(TAG, "Running Application");
    
    loadSchedules(&list);

    while(1){
        now = SNTP_GetTime();
        sch = (schedule_t*)(list.next->value);
        if(sch->time == now){            
                printf("feeding\n");
        }        
        vTaskDelay(1000);
    }
}