
#include <esp_log.h>
#include <esp_system.h>
#include "steper.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"



static xQueueHandle stepper_evt_queue = NULL;
static const char *TAG = "STEP";


static void stepperHandler(void *queue)
{
    uint32_t steps;
    xQueueHandle *stepper_evt_queue = (xQueueHandle*)queue;

    for (;;) {
        if (xQueueReceive(stepper_evt_queue, &steps, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Moving [%d] steps", steps);
        }
    }
}

void STEP_Init(void){
    stepper_evt_queue = xQueueCreate(QUEUE_SIZE, sizeof(uint32_t));
    xTaskCreate(stepperHandler, "stepper_task", 2048, stepper_evt_queue, 10, NULL);
}

void STEP_MoveSteps(uint32_t steps){
    xQueueSend(stepper_evt_queue, &steps,NULL);
}

