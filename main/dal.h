#ifndef _dal_h_
#define _dal_h_

#include "json.h"
#include "repo.h"

schedule_t *DAL_JsonToSchedule(Json *js);
char *DAL_ScheduleToJson(schedule_t *sch);
char *DAL_ListToJson(node_t *head);

#endif