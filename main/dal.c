#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <esp_log.h>
#include "list.h"
#include "repo.h"

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
        sch->time = atol((const char *)tmp);                    
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
	
	sprintf(jstr, "{\"qnt\":%d,\"repeat\":%d,\"time\":%lu}", 
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
char *DAL_ListToJson(node_t *head, uint32_t maxitems){
char *jstr = (char*)malloc(maxitems * SCHEDULE_T_CHARS);
char *aux;

	if(jstr == NULL){
		ESP_LOGE(TAG, "Fail to allocate memory for stringify list");
		return NULL;
	}
	
	if(head->next == NULL){
		strcpy(jstr, "[]");
		return jstr;
	}
	
	aux = jstr;
	*jstr++ = '[';

	while(1){
		head = head->next;
		char *jsch = DAL_ScheduleToJson((schedule_t*)head->value);
		if(jsch != NULL){
			//printf("%s\n", jsch);
			uint32_t jsize = strlen(jsch);
			memcpy(jstr, jsch, jsize);
			free(jsch);
			jstr += jsize;
			if(head->next == NULL){			
				break;
			}
			*jstr++ = ',';
		}
	}

	*jstr++ = ']';
	*jstr = '\0';
    return aux;
}

/**
 * Convert Json string to configuration structure
 * 
 * \param jstr	pointer to json string
 * \return pointer to config_t structure, NULL on error
 * */
config_t *DAL_JsonToConfig(char *jstr){
config_t *cfg = (config_t*)malloc(sizeof(config_t));
Json js;

	if(jstr == NULL){
		ESP_LOGE(TAG, "Fail to allocate memory for config structure");
		return NULL;
	}

	if(JSON_init(&js, jstr) == ESP_OK){
        if(JSON_string(&js, "ssid", cfg->ssid) == 0){
            ESP_LOGE(TAG,"Unable to read wificfg ssid");
        }

        if(JSON_string(&js, "password", cfg->password) == 0){
            ESP_LOGE(TAG,"Unable to read wificfg password");
        }
	}
	return cfg;
}

/**
 * 
 * */
char *DAL_ConfigToJson(config_t *cfg){
	char *jstr = (char*)malloc(CONFIG_T_CHARS);

	if(jstr == NULL){
		ESP_LOGE(TAG, "Fail to allocate memory for stringify configuration");
        return NULL;
	}
	
	sprintf(jstr, "{\"ssid\":\"%s\",\"password\":\"%s\"}", 
		cfg->ssid,
		cfg->password);
	
	return jstr;
}