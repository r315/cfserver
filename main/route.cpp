
#include <esp_log.h>
#include <esp_system.h>
#include "cfserver.h"
#include "stepper.h"
#include "repo.h"
#include "json.h"
#include "route.h"

#define TMP_BUF_LEN 64

static const char *TAG = "ROUTE";

esp_err_t home_get_handler(httpd_req_t *req);
esp_err_t schedule_get_handler(httpd_req_t *req);
esp_err_t feed_post_handler(httpd_req_t *req);
esp_err_t schedule_post_handler(httpd_req_t *req);

httpd_uri_t handlers [] = {
    {
        .uri = "/",
        .method = HTTP_GET,
        .handler = home_get_handler,
        .user_ctx = NULL
    },
    {
        .uri = "/feed",
        .method = HTTP_POST,
        .handler = feed_post_handler,
        .user_ctx = NULL
    },
    {
        .uri = "/schedule",
        .method = HTTP_GET,
        .handler = schedule_get_handler,
        .user_ctx = NULL
    },
    {
        .uri = "/schedule",
        .method = HTTP_POST,
        .handler = schedule_post_handler,
        .user_ctx = NULL
    },
    NULL
};

httpd_uri_t *ROUTE_GetUriList(void){
    return handlers;
}

/**
 * handler for GET /
 */
esp_err_t home_get_handler(httpd_req_t *req){
    
    ESP_LOGI(TAG, "GET for URI: %s", req->uri);

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    //const char* resp_str = "Home, testing testting";
    //httpd_resp_send(req, resp_str, strlen(resp_str));

    char *buf;
    uint32_t size = REPO_HomePage(&buf);

    if(size > 0){
        httpd_resp_send(req, buf, size);
        free(buf);
    }else{
        httpd_resp_send(req, "Fail", 5);
    }    
    return ESP_OK;
}

/**
 * handler for POST /
 */
esp_err_t feed_post_handler(httpd_req_t *req){
    char *buf, tmp[TMP_BUF_LEN];
    Json json;
    int ret = ESP_FAIL, len = req->content_len;

    ESP_LOGI(TAG, "POST for URI \"%s\"", req->uri);

    buf = (char*)malloc(len + 1); // Extra string terminator
    memset(buf, '\0', len + 1);
    memset(tmp,'\0', TMP_BUF_LEN);

    if(buf == NULL){
        httpd_resp_set_status(req, "507");
        ESP_LOGE(TAG, "Failed to allocate memory");
        goto err0;
    }    

    if ((ret = httpd_req_recv(req, buf, len)) <= 0) {
        httpd_resp_set_status(req, "500");
        ESP_LOGE(TAG, "Failed receiving data %u", ret);
        ret = ESP_FAIL;
        goto err1;
    }
    
    ret = json.init(buf);

    if( ret == ESP_OK){
        if(json.string("qnt", (uint8_t*)tmp) > 0){    
            int32_t qnt = atoi(tmp);
            if(qnt > 0){
                ESP_LOGI(TAG, "Dispensing %dg", qnt);
                STEP_MoveSteps(qnt);
                sprintf(tmp, "ok");
            }else{
                ret = ESP_FAIL;
                sprintf(tmp, "Invalid quantity '%d'",qnt);
                httpd_resp_set_status(req, "400");
            }
        }
    }else{
        esp_err_to_name(ret); 
    }

err1:
    free(buf);
err0:
    httpd_resp_send(req, tmp, strlen(tmp));
    return ret;
}

esp_err_t schedule_get_handler(httpd_req_t *req){
esp_err_t ret = ESP_OK;
uint32_t size;
char *buf;

    ESP_LOGI(TAG, "GET for URI: %s", req->uri);

    size = REPO_Schedules(&buf);

    if(size > 0){
        httpd_resp_send(req, buf, size);
        free(buf);
        return ESP_OK;
    }

    httpd_resp_set_status(req, "500");
    httpd_resp_send(req, "Error get schedules", 19);
    return ESP_FAIL;        
}

esp_err_t schedule_post_handler(httpd_req_t *req){
    ESP_LOGI(TAG, "POST for URI: %s", req->uri);
    httpd_resp_set_status(req, "505");
    httpd_resp_send(req, "Not implemented", 19);
    return ESP_OK;
}



#if 0
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

#endif