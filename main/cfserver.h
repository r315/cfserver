
/**
 * Cat Feeder Server
 */
#ifndef _cfserver_h_
#define _cfserver_h_

#include <esp_http_server.h>
#include <esp_wifi.h>

#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

#define FEED_200G   1000

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/queue.h>

typedef struct uri_node_{
    SLIST_ENTRY(uri_node_) node;
    httpd_uri_t uri;
}uri_node_t;

class Cfserver
{
private:
    httpd_handle_t handle = NULL;
    wifi_config_t wifi_config;
    SLIST_HEAD(uri_list, uri_node_) uri_list_head;
public:
    const char *tag;
    void init(void);
    void stop(void);
    void start(void);
    void add_uri(uri_node_ *uri);
    void remove_uri(uri_node_ *uri);
    void unregister_uri(uri_node_t *uri);
    void register_uri(uri_node_t *uri_node);
    void set_ssid(uint8_t *ssid);
    void set_pass(uint8_t *pass);
};

#ifdef __cplusplus
}
#endif

#endif