#include <string.h>
#include <esp_log.h>
#include <esp_system.h>
#include "json.h"

static const char *TAG = "JSON";

static int jsoneq(uint8_t *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp((char *)json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

static void jcpy(uint8_t *dst, uint8_t *src, uint32_t len){
    while(len--){
        *(dst++) = *(src++);
    }
    *(dst) = '\0';
}

esp_err_t Json::init(char *json){
    
    jsmn_init(&jp);

    jelements = jsmn_parse(&jp, json, strlen(json), jtokens, sizeof(jtokens) / sizeof(jtokens[0]));
    if (jelements < 0) {
        ESP_LOGE(TAG, "Failed to parse JSON: %d", jelements);
        return ESP_ERR_NOT_FOUND;
    }
    else{
        ESP_LOGI(TAG, "%u elements parsed", jelements);
    }

    /* Assume the top-level element is an object */
    if (jelements < 1 || jtokens[0].type != JSMN_OBJECT) {
        ESP_LOGE(TAG, "Object expected");
        return ESP_ERR_NOT_FOUND;
    }

    jstr = (uint8_t*)json;

    return ESP_OK;
}

uint32_t Json::string(const char *name, uint8_t *dst){
    uint32_t size = 0;

    for(uint32_t i = 1; i < jelements; i++){
        if (jsoneq(jstr, &jtokens[i], name) == 0) {
            size = jtokens[i + 1].end - jtokens[i + 1].start;
            jcpy(dst, jstr + jtokens[i + 1].start, size);
            //ESP_LOGW(TAG, "%s : %s", name, dst);
            break;
        }
    }
    return size;
}