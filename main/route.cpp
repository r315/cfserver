
#include <esp_log.h>
#include <esp_system.h>
#include "cfserver.h"

static const char *TAG="ROUTE";

esp_err_t repoReadFile(char *file, char *dst, uint32_t len);

/**
 * handler for GET /
 */
esp_err_t home_get_handler(httpd_req_t *req){
    
    ESP_LOGI(TAG, "Request URI: %s", req->uri);


    /* Send response with custom headers and body set as the
     * string passed in user context*/
    //const char* resp_str = "Home, testing testting";
    //httpd_resp_send(req, resp_str, strlen(resp_str));

    char resp_str[64];
    repoReadFile((char*)"/spiffs/foo.txt", resp_str, sizeof(resp_str));
    httpd_resp_send(req, resp_str, strlen(resp_str));

    return ESP_OK;
}

uri_node_t home_uri = {
    .node = {NULL},
    .uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = home_get_handler,
        .user_ctx = NULL
    }
};

/**
 * Handler for GET /ctrl
 * 
 * An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 */
esp_err_t ctrl_put_handler(httpd_req_t *req)
{
    char buf;
    int ret;
    Cfserver *server = (Cfserver*)(req->user_ctx);

    if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    if (buf == '0') {        
        ESP_LOGI(server->tag, "Unregistering /hello and /echo URIs");
        //server->unregister_uri(&hello_uri);
        //server->unregister_uri(&echo_uri);
    }
    else {
        ESP_LOGI(server->tag, "Registering /hello and /echo URIs");
        //server->register_uri(&hello_uri);
        //server->unregister_uri(&echo_uri);
    }

    /* Respond with empty body */
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}
#if 0
httpd_uri_t echo = {
    .uri       = "/echo",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = NULL
};

/* An HTTP POST handler */
esp_err_t echo_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(tag, "=========== RECEIVED DATA ==========");
        ESP_LOGI(tag, "%.*s", ret, buf);
        ESP_LOGI(tag, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
#endif
uri_node_t ctrl_uri = {
    .node = {NULL},
    .uri = {
        .uri = "/ctrl",
        .method = HTTP_GET,
        .handler = ctrl_put_handler,
        .user_ctx = NULL
    }
};

/**
 * 
 *
 * An HTTP GET handler 
 * 
 * */
esp_err_t hello_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;
    Cfserver *server = (Cfserver*)(req->user_ctx);

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(server->tag, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) {
            ESP_LOGI(server->tag, "Found header => Test-Header-2: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK) {
            ESP_LOGI(server->tag, "Found header => Test-Header-1: %s", buf);
        }
        free(buf);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(server->tag, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(server->tag, "Found URL query parameter => query1=%s", param);
            }
            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(server->tag, "Found URL query parameter => query3=%s", param);
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(server->tag, "Found URL query parameter => query2=%s", param);
            }
        }
        free(buf);
    }

    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = "Hello, testing testting";
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(server->tag, "Request headers lost");
    }
    return ESP_OK;
}

uri_node_t hello_uri = {
    .node = {NULL},
    .uri = {
        .uri = "/hello",
        .method = HTTP_GET,
        .handler = hello_get_handler,
        .user_ctx = NULL
    }
};