#ifndef _json_h_
#define _json_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <esp_system.h>
#include "jsmn.h"

#define JSON_MAX_TOKENS 100

esp_err_t JSON_init(char *str);
uint32_t JSON_string(const char *name, uint8_t *dst);
uint32_t JSON_nextToken(jsmntype_t tok);

#ifdef __cplusplus
}
#endif

#endif // !_json_h_