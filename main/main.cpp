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
#include "cfserver.h"
#include "steper.h"
#include "repo.h"
#include "json.h"


static const char *TAG="APP";
static Cfserver server;

extern uri_node_t home_get;
extern uri_node_t feed_post;

extern "C" void app_main()
{    
    char *ptr;
    uint8_t tmp[64];
    Json json;
    
    ESP_LOGI(TAG, "Starting Application");

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(REPO_Init());

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
    server.add_uri(&home_get);
    server.add_uri(&feed_post);
    
    STEP_Init();
}
