#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <esp_log.h>
#include "list.h"
#include "dal.h"

static const char *TAG = "DAL";

/**
*/
schedule_t *DAL_JsonToSchedule(Json *js){
schedule_t *sch = (schedule_t*)malloc(sizeof(schedule_t));
uint8_t tmp[10];

    if(sch == NULL){
		ESP_LOGE(TAG, "Fail to allocate memory for schedule");
        return NULL;
	}
    if(JSON_string(js, "qnt", tmp) > 0){
        sch->qnt = atoi((const char *)tmp);                    
    }

    if(JSON_string(js, "repeat", tmp) > 0){
        sch->repeat = atoi((const char *)tmp);                    
    }

    if(JSON_string(js, "time", tmp) > 0){
        sch->time = atoll((const char *)tmp);                    
    }                
    return sch;
}

/**
 * */
char *DAL_ScheduleToJson(schedule_t *sch){
char *jstr = (char*)malloc(SCHEDULE_T_CHARS);

	if(jstr == NULL){
		ESP_LOGE(TAG, "Fail to allocate memory for stringify schedule");
        return NULL;
	}
	
	sprintf(jstr, "{\"qnt\":%d,\"repeat\":%d,\"time\":%llu}", 
		sch->qnt,
		sch->repeat,
		sch->time);

		//printf("{\"qnt\":%d,\"repeat\":%d,\"time\":%llu}\n",sch->qnt,sch->repeat,sch->time);
	return jstr;
}

/**
 * convert a list of schedules into a JSON string
 * 
 * \param head  list head
 * \return pointer to JSON string, pointer should be free after use 
 * */
char *DAL_ListToJson(node_t *head){
char *jstr = (char*)malloc(REPO_MAX_SCHEDULES * SCHEDULE_T_CHARS);
char *ret = jstr;

	if(jstr == NULL){
		ESP_LOGE(TAG, "Fail to allocate memory for stringify list");
		return NULL;
	}
	
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

