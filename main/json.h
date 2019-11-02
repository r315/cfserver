#ifndef _json_h_
#define _json_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <esp_system.h>
#include "jsmn.h"

#define JSON_MAX_TOKENS 30

class Json{
private:
    jsmn_parser jp;
    jsmntok_t jtokens[JSON_MAX_TOKENS];
    int32_t jelements, curjelement;
    uint8_t *jstr;
public:
    esp_err_t init(char *str);
    uint32_t string(const char *name, uint8_t *dst);
    uint32_t nextToken(jsmntype_t tok);
};

#ifdef __cplusplus
}
#endif

#endif // !_json_h_