#include <string.h>
#include <esp_log.h>
#include <esp_system.h>
#include "json.h"
#include "jsmn.h"

static const char *TAG = "JSON";

static const char *jsmnParseErr(int32_t err){
    switch(err){
        case JSMN_ERROR_NOMEM: return "JSMN_ERROR_NOMEM";
        case JSMN_ERROR_INVAL: return "JSMN_ERROR_INVAL";
        case JSMN_ERROR_PART: return "JSMN_ERROR_PART";
    }
    return ESP_OK;
}

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

esp_err_t JSON_init(Json *js, char *jstr){
    
    jsmn_init(&js->jp);

    js->jelements = jsmn_parse(&js->jp, jstr, strlen(jstr), js->jtokens, sizeof(js->jtokens) / sizeof(jsmntok_t));
    if (js->jelements < 0) {
        ESP_LOGE(TAG, "Failed to parse JSON: %s", jsmnParseErr(js->jelements));
        ESP_LOGE(TAG, "JSON String : \n%s", jstr);
        return ESP_ERR_NOT_FOUND;
    }
    else{
        ESP_LOGD(TAG, "%u elements parsed", js->jelements);
    }

    /* Top-level element must be an object or array */
    if (js->jelements < 1 || (js->jtokens[0].type != JSMN_OBJECT && js->jtokens[0].type != JSMN_ARRAY) ) {
        ESP_LOGE(TAG, "Object or array expected");
        return ESP_ERR_NOT_FOUND;
    }

    js->jstr = (uint8_t*)jstr;

    js->curjelement = 0;

    return ESP_OK;
}

/**
 * */
uint32_t JSON_string(Json *js, const char *name, uint8_t *dst){
    uint32_t size = 0;

    for(uint32_t i = js->curjelement; i < js->jelements; i++){
        if (jsoneq(js->jstr, &js->jtokens[i], name) == 0) {
            size = js->jtokens[i + 1].end - js->jtokens[i + 1].start;
            jcpy(dst, js->jstr + js->jtokens[i + 1].start, size);
            //ESP_LOGW(TAG, "%s : %s", name, dst);
            break;
        }
    }
    return size;
}

/**
 * Move to next parsed token, the next strings will be parsed 
 * from this point on
 * */
uint32_t JSON_nextToken(Json *js, jsmntype_t tok){
	for(int i = js->curjelement + 1; i < js->jelements; i++){
		if(js->jtokens[i].type == tok){
			js->curjelement = i;
			return js->curjelement;
		}
	}
	return 0;
}
