
#include <esp_wifi.h>

#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include <nvs_flash.h>

#include "cfserver.h"

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
    case SYSTEM_EVENT_STA_GOT_IP:{
        ESP_LOGI(server->tag, "SYSTEM_EVENT_STA_GOT_IP");
        ESP_LOGI(server->tag, "Got IP: '%s'",
                ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        /* Start the web server */        
        server->start();   }     
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
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    memcpy(&wifi_config.sta.ssid, EXAMPLE_WIFI_SSID, sizeof(EXAMPLE_WIFI_SSID));
    memcpy(&wifi_config.sta.password, EXAMPLE_WIFI_PASS, sizeof(EXAMPLE_WIFI_PASS));
    ESP_LOGI(tag, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    /* Note this must be called before adding items */
    uri_list_head = SLIST_HEAD_INITIALIZER(uri_list_head);
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
        uri_node_t *tmp;
        SLIST_FOREACH(tmp, &this->uri_list_head, node){
            ESP_LOGI(tag, "Registering handler for uri \"%s\"",tmp->uri.uri);
            httpd_register_uri_handler(handle, &tmp->uri);
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

void Cfserver::add_uri(uri_node_t *uri_node){
    uri_node->uri.user_ctx = this;
    SLIST_INSERT_HEAD(&uri_list_head, uri_node, node);
}

void Cfserver::remove_uri(uri_node_t *uri_node){
    SLIST_REMOVE(&uri_list_head, uri_node, uri_node_, node);
}

void Cfserver::unregister_uri(uri_node_t *uri_node){
    /* Handler can be unregistered using the uri string */
    httpd_unregister_uri(handle, uri_node->uri.uri);
    // TODO: fix crash if elemnt does not exists on list
    remove_uri(uri_node);
}

void Cfserver::register_uri(uri_node_t *uri_node){
    add_uri(uri_node);
    httpd_register_uri_handler(handle, &uri_node->uri);
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