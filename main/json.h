#ifndef _json_h_
#define _json_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <esp_system.h>
#include "jsmn.h"

#define JSON_MAX_TOKENS 100

typedef struct _Json{
    jsmn_parser jp;
    jsmntok_t jtokens[JSON_MAX_TOKENS];
    int32_t jelements, curjelement;
    uint8_t *jstr;
}Json;

esp_err_t JSON_init(Json *, char *str);
uint32_t JSON_string(Json *, const char *name, uint8_t *dst);
uint32_t JSON_nextToken(Json *, jsmntype_t tok);

#ifdef __cplusplus
}
#endif

#endif // !_json_h_