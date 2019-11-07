

#include <stdint.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include <nvs_flash.h>

#include "cfserver.h"


static wifi_init_config_t cfg = {
    &esp_event_send,
    NULL,
    WIFI_QOS_ENABLED,
    WIFI_AMPDU_RX_ENABLED,
    WIFI_AMPDU_RX_BA_WIN,
    WIFI_AMPDU_RX_AMPDU_BUF_NUM,
    WIFI_AMPDU_RX_AMPDU_BUF_LEN,
    WIFI_RX_MAX_SINGLE_PKT_LEN,
    WIFI_HW_RX_BUFFER_LEN,
    WIFI_AMSDU_RX_ENABLED,
    CONFIG_ESP8266_WIFI_RX_BUFFER_NUM,
    CONFIG_ESP8266_WIFI_RX_PKT_NUM,
    CONFIG_ESP8266_WIFI_LEFT_CONTINUOUS_RX_BUFFER_NUM,
    CONFIG_ESP8266_WIFI_TX_PKT_NUM,
    WIFI_NVS_ENABLED,
     0,
    WIFI_INIT_CONFIG_MAGIC
};

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    Cfserver *server = (Cfserver*)ctx;
    /* For accessing reason codes in case of disconnection */
    system_event_info_t *info = &event->event_info;

    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(server->tag, "SYSTEM_EVENT_STA_START");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(server->tag, "SYSTEM_EVENT_STA_GOT_IP");
        ESP_LOGI(server->tag, "Got IP: '%s'",
                ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        /* Start the web server */        
        server->start();
        /* Initialyze connection dependent services */
        if(server->onConnect != NULL)
            server->onConnect();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(server->tag, "SYSTEM_EVENT_STA_DISCONNECTED");
        ESP_LOGE(server->tag, "Disconnect reason : %d", info->disconnected.reason);
        if (info->disconnected.reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
            /*Switch to 802.11 bgn mode */
            esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
        }
        ESP_ERROR_CHECK(esp_wifi_connect());

        /* Stop the web server */
        server->stop();        
        if(server->onDisconnect != NULL)
            server->onDisconnect();
        break;
    default:
        break;
    }
    return ESP_OK;
}

void Cfserver::init(void)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, this));
    //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_LOGI(tag, "Connecting to \"%s\"...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void Cfserver::start(void)
{    
    if(handle != NULL)
        return;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(tag, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&handle, &config) == ESP_OK) {
        // Set URI handlers
        httpd_uri_t *tmp = this->uri_list;
        while(tmp->uri != NULL){
            ESP_LOGI(tag, "Registering handler for uri \"%s\"",tmp->uri);
            httpd_register_uri_handler(handle, tmp);
            tmp++;
        }          
        return;
    }

    ESP_LOGI(tag, "Error starting server!");
    handle = NULL;
}

void Cfserver::stop(void)
{
    if(handle == NULL)
        return;

    httpd_stop(handle);
    handle = NULL;
}

void Cfserver::setUriList(httpd_uri_t *uri_list){
    this->uri_list = uri_list;
}

void Cfserver::unregister_uri(httpd_uri_t *uri){
    httpd_unregister_uri(handle, uri->uri);
}

void Cfserver::register_uri(httpd_uri_t *uri){
    httpd_register_uri_handler(handle, uri);
}

/** 
 * copy string ended by \n or \0
 * */
static void chrcpy(uint8_t *dst, uint8_t *src){
    while(*src != '\n' && *src != '\0'){
        *(dst++) = *(src++);
    }
    *dst = '\0';
}

void Cfserver::set_ssid(uint8_t *ssid){
    chrcpy(wifi_config.sta.ssid, ssid);
}

void Cfserver::set_pass(uint8_t *pass){
    chrcpy(wifi_config.sta.password, pass);
}

/*
struct uri_node_ {
    struct {
        struct uri_node_ *sle_next;
    } node;
    httpd_uri_t uri;
}

struct uri_list {
    struct uri_node_ *slh_first;
}uri_list_head;
*/
