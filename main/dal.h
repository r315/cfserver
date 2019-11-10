#ifndef _dal_h_
#define _dal_h_

#include "json.h"

#define SCHEDULE_T_CHARS    64
#define CONFIG_T_CHARS      256

typedef struct _config_t{
    uint8_t ssid[64];
    uint8_t password[64];
}config_t;

typedef struct _schedule_t{
    uint16_t qnt;              //5 chars
    uint8_t repeat;            //3 chars
    uint64_t time;             //20 chars
}schedule_t;


schedule_t *DAL_JsonToSchedule(Json *js);
char *DAL_ScheduleToJson(schedule_t *sch);
char *DAL_ListToJson(node_t *head, uint32_t maxitems);

#endif