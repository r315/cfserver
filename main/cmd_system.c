/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_console.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "esp_spi_flash.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "rom/uart.h"
#include "cmd_system.h"
#include "sdkconfig.h"
#include "sntp.h"
#include "repo.h"
#include "stepper.h"

static const char *TAG = "cmd_system";

typedef struct _opt{
    const char *opt;
    char *optv;
}opt_t;

static void parseOptions(int argc, char **argv, int optc, opt_t *options){
    while(argc--){
        for(uint32_t i = 0; i < optc; i++){
            if(memcmp(argv[argc], options[i].opt, strlen(argv[argc])) == 0){
                options[i].optv = argv[argc + 1];
            }
        }
    }
}


/* 'version' command */
static int get_version(int argc, char **argv)
{
    esp_chip_info_t info;
    esp_chip_info(&info);
    printf("IDF Version:%s\r\n", esp_get_idf_version());
    printf("Chip info:\r\n");
    printf("\tmodel:%s\r\n", info.model == CHIP_ESP8266 ? "ESP8266" : "Unknow");
    printf("\tcores:%d\r\n", info.cores);
    printf("\tfeature:%s%s%s%s%d%s\r\n",
           info.features & CHIP_FEATURE_WIFI_BGN ? "/802.11bgn" : "",
           info.features & CHIP_FEATURE_BLE ? "/BLE" : "",
           info.features & CHIP_FEATURE_BT ? "/BT" : "",
           info.features & CHIP_FEATURE_EMB_FLASH ? "/Embedded-Flash:" : "/External-Flash:",
           spi_flash_get_chip_size() / (1024 * 1024), " MB");
    printf("\trevision number:%d\r\n", info.revision);
    return 0;
}

static void register_version()
{
    const esp_console_cmd_t cmd = {
        .command = "version",
        .help = "Get version of chip and SDK",
        .hint = NULL,
        .func = &get_version,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** 'restart' command restarts the program */

static int restart(int argc, char **argv)
{
    ESP_LOGI(TAG, "Restarting");
    esp_restart();
}

static void register_restart()
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "Software reset of the chip",
        .hint = NULL,
        .func = &restart,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** 'free' command prints available heap memory */
static int free_mem(int argc, char **argv)
{
    printf("%d\n", esp_get_free_heap_size());
    return 0;
}

static void register_free()
{
    const esp_console_cmd_t cmd = {
        .command = "free",
        .help = "Get the current size of free heap memory",
        .hint = NULL,
        .func = &free_mem,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/* 'heap' command prints minumum heap size */
static int heap_size(int argc, char **argv)
{
    uint32_t heap_size = heap_caps_get_minimum_free_size(MALLOC_CAP_32BIT);
    ESP_LOGI(TAG, "min heap size: %u", heap_size);
    return 0;
}

static void register_heap()
{
    const esp_console_cmd_t heap_cmd = {
        .command = "heap",
        .help = "Get minimum size of free heap memory that was available during program execution",
        .hint = NULL,
        .func = &heap_size,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&heap_cmd) );

}

/* 'wifi' command manages wifi connection */
static int system_wifi(int argc, char **argv){
config_t *cfg;
opt_t options[] = {
{"-p",NULL},
{"-s", NULL}
};

    cfg = REPO_ReadConfig();

    if (argc == 1)
    {
        if (cfg != NULL)
        {
            printf("SSID: %s\nPASS: %s\n", cfg->ssid, cfg->password);
            goto end1;            
        }
    }

    parseOptions(argc, argv, sizeof(options)/sizeof(opt_t), options);

    if(options[0].optv != NULL){
        strcpy((char*)cfg->password, options[0].optv);
    }

    if(options[1].optv != NULL){
        strcpy((char*)cfg->ssid, options[1].optv);
    }

    if(REPO_SaveConfig(cfg) == 0){
        ESP_LOGE(TAG,"Fail to save to config file");
    }

end1:
    free(cfg);
    return ESP_OK;
}

static void register_wifi()
{
    const esp_console_cmd_t cmd = {
        .command = "wifi",
        .help = "Get/Set wifi configuration, no options prints current configuration\n"
                "-p <password>\n"
                "-s <ssid>",
        .hint = NULL,
        .func = &system_wifi,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}


/* 'time'  */
static int system_time(int argc, char **argv)
{
    SNTP_PrintTime();
    return 0;
}

static void register_time()
{
    const esp_console_cmd_t cmd = {
        .command = "time",
        .help = "Prints current system time",
        .hint = NULL,
        .func = &system_time,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}


/* Schedules  */
static int system_schedules(int argc, char **argv){
char *jstr = REPO_JsonSchedules();
    printf("\n%s\n", jstr);
    free(jstr);
    return ESP_OK;
}

static void register_schedules(){
    const esp_console_cmd_t cmd = {
        .command = "schedules",
        .help = "Prints feed schedules",
        .hint = NULL,
        .func = &system_schedules,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );    
}

/**
 * Stepper console command
 * */
static int move_steps(int argc, char **argv){
uint32_t steps, time;
opt_t options[] = {
    {"-n",NULL},
    {"-t", NULL}
};

    parseOptions(argc, argv, sizeof(options)/sizeof(opt_t), options);

    if(options[0].optv == NULL || options[1].optv == NULL){
        ESP_LOGW(TAG,"missing parameters");
        return 0;
    }
    
    steps = atoi(options[0].optv);
    time = atoi(options[1].optv);

    if(time < STEP_MIN_PULSE)
    {
        ESP_LOGI(TAG,"Invalid pulse time\n");    
        return 0;
    }    

    ESP_LOGI(TAG,"moving %u steps\n", steps);
    STEP_Move(steps, time, STEP_CW);

    return 0;
}

void register_stepper(void){    
    const esp_console_cmd_t step_move = {
        .command = "step",
        .help = "Move stepper motor\n"
                "-n <number of pulses>\n"
                "-t <time between pulses in us (>1000)>\n",
        .hint = NULL,
        .func = &move_steps,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&step_move) );
}

void register_commands()
{
    register_free();
    register_heap();
    register_version();
    register_restart();
    register_time();
    register_wifi();
    register_schedules();
    register_stepper();
}
