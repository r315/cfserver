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


static const char *TAG="APP";
static Cfserver server;

extern uri_node_t home_get;
extern uri_node_t feed_post;

extern "C" void app_main()
{    

    ESP_ERROR_CHECK(nvs_flash_init());
    REPO_Init();

    server.tag = TAG;
    server.init();
    //server.add_uri(&ctrl_uri);
    //server.add_uri(&hello_uri);
    server.add_uri(&home_get);
    server.add_uri(&feed_post);
    
    STEP_Init();

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
