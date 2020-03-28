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
#include "mdns.h"

static const char *TAG="App";
static const char *MDNS_HOSTNAME = "sissi"; // alias sissi.local
static Cfserver server;

enum {APP_STARTING, APP_DISPENSING, APP_DISPENSING_DONE, APP_IDLE, APP_END};

static void initialise_mdns(void)
{
    ESP_LOGI(TAG, "Initialising mDNS");
    //initialize mDNS
    ESP_ERROR_CHECK( mdns_init() );
    //set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK( mdns_hostname_set(MDNS_HOSTNAME) );
    //set default mDNS instance name
    ESP_ERROR_CHECK( mdns_instance_name_set("sissi cat feeder") );
/*
     mdns_txt_item_t serviceTxtData[3] = {
        {"board","esp32"},
        {"u","user"},
        {"p","password"}
    };

    ESP_ERROR_CHECK( mdns_service_add("catfeed", "_http", "_tcp", 80, serviceTxtData, 3) );
*/
}


void onWifiConnected(void){
    SNTP_Init();
    initialise_mdns();
}

void onWifiDisconnect(void){
    
}

extern "C" void server_init(void){    
config_t *cfg;

    cfg = REPO_ReadConfig();

    if(cfg != NULL){        
        server.set_ssid(cfg->ssid);
        server.set_pass(cfg->password);
        free(cfg);
    }

    server.tag = "CFSERVER";
    server.init();
    server.setUriList(ROUTE_GetUriList());
    server.onConnect = onWifiConnected;
    server.onDisconnect = onWifiDisconnect;
}

extern "C" void app(void){
time_t now;
schedule_t *sch;
uint8_t app_state = APP_STARTING;

    ESP_LOGI(TAG, "Running Application");

    while(1){

        switch(app_state){
        	case APP_STARTING:
        		app_state = APP_IDLE;
        		
            case APP_IDLE:
                now = SNTP_GetTime() / 60; // ignore seconds
                sch = REPO_FirstSchedule();
                if(sch != NULL){
                    if(now == sch->time/60){
                        app_state = APP_DISPENSING;
                    }
                }
                break;

            case APP_DISPENSING:
                printf("\nfeeding\n");
                app_state = APP_DISPENSING_DONE;
                break;

            case APP_DISPENSING_DONE:
                app_state = APP_IDLE;

            default:
                break;

        } 
        vTaskDelay(1000);
    }
}