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


/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 * The examples use simple WiFi configuration that you can set via
 * 'make menuconfig'.
 * If you'd rather not, just change the below entries to strings
 * with the config you want -
 * ie. #define EXAMPLE_WIFI_SSID "mywifissid"
*/

static const char *TAG="APP";

static Cfserver server;

extern uri_node_t hello_uri;
extern uri_node_t ctrl_uri;
extern uri_node_t home_uri;
extern uri_node_t post_uri;

extern "C" esp_err_t repoInit(void);

extern "C" void app_main()
{
    

    ESP_ERROR_CHECK(nvs_flash_init());

    server.tag = TAG;
    server.init();
    server.add_uri(&ctrl_uri);
    server.add_uri(&hello_uri);
    server.add_uri(&home_uri);
    repoInit();    
}
