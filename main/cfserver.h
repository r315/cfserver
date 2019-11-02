
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

class Cfserver
{
private:
    httpd_handle_t handle = NULL;
    wifi_config_t wifi_config;
    httpd_uri_t *uri_list;
public:
    const char *tag;
    void init(void);
    void stop(void);
    void start(void);
    void setUriList(httpd_uri_t *uri_list);
    void unregister_uri(httpd_uri_t *uri);
    void register_uri(httpd_uri_t *uri);
    void set_ssid(uint8_t *ssid);
    void set_pass(uint8_t *pass);
    void (*onConnect)(void);
    void (*onDisconnect)(void);
};



#ifdef __cplusplus
}
#endif

#endif