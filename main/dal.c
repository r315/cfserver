#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "list.h"
#include "dal.h"

/**
*/
schedule_t *DAL_JsonToSchedule(Json *js){
schedule_t *sch = (schedule_t*)malloc(sizeof(schedule_t));
uint8_t tmp[10];

    if(sch == NULL)
        return NULL;
    if(JSON_string(js, "qnt", tmp) > 0){
        sch->qnt = atoi((const char *)tmp);                    
    }

    if(JSON_string(js, "repeat", tmp) > 0){
        sch->repeat = atoi((const char *)tmp);                    
    }

    if(JSON_string(js, "time_t", tmp) > 0){
        sch->time = atol((const char *)tmp);                    
    }                
    return sch;
}

/**
 * */
char *DAL_ScheduleToJson(schedule_t *sch){
char *jstr = (char*)malloc(SCHEDULE_T_CHARS);

	 if(jstr == NULL)
        return NULL;
	
	sprintf(jstr, "{\"qnt\":%d,\"repeat\":%d,\"time\":%lld}", 
		sch->qnt,
		sch->repeat,
		sch->time);
	return jstr;
}

/**
 * */
char *DAL_ListToJson(node_t *head){
char *jstr = (char*)malloc(REPO_MAX_SCHEDULES * SCHEDULE_T_CHARS);
char *ret = jstr;

	if(jstr == NULL)
		return NULL;
	
	*jstr = '[';

	for(uint32_t i = 0; head->next != NULL; i++, *jstr = ','){	
		head = head->next;
		char *jsch = DAL_ScheduleToJson((schedule_t*)head->value);
		if(jsch != NULL){
			//printf("%s\n", jsch);
			uint32_t jsize = strlen(jsch);
			jstr++;
			memcpy(jstr, jsch, jsize);
			jstr += jsize;
			free(jsch);
		}
	}
	
	*jstr++ = ']';
	*jstr = '\0';
    return ret;
}

