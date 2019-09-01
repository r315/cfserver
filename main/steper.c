
#include <esp_log.h>
#include <esp_system.h>

#include "steper.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


#define STEP_CLK_PIN    14
#define STEP_DIR_PIN    12
#define STEP_EN_PIN     13

#define STEP_SPEED      20
#define STEP_CW         0
#define STEP_CCW        1

#define STEP_DISABLED   0
#define STEP_ENABLED    1

static xQueueHandle stepper_evt_queue = NULL;
static const char *TAG = "STEP";


static void pulseStep(uint32_t time, uint32_t pulses, uint8_t dir){
TickType_t xLastWakeTime = xTaskGetTickCount();

    gpio_set_level(STEP_DIR_PIN, dir);

    while(pulses--){
        gpio_set_level(STEP_CLK_PIN, 1);
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(time/2));
        gpio_set_level(STEP_CLK_PIN, 0);
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(time/2));
    }
}


static void stepperHandler(void *queue)
{
    uint32_t steps;
    xQueueHandle *stepper_evt_queue = (xQueueHandle*)queue;

    for (;;) {
        if (xQueueReceive(stepper_evt_queue, &steps, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Moving [%d] steps", steps);
            gpio_set_level(STEP_EN_PIN, STEP_ENABLED);
            pulseStep(STEP_SPEED, steps, STEP_CW);
            gpio_set_level(STEP_EN_PIN, STEP_DISABLED);
        }
    }
}

void STEP_Init(void){
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = ((1<<STEP_CLK_PIN) | (1<<STEP_DIR_PIN) | (1<<STEP_EN_PIN));
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    gpio_set_level(STEP_EN_PIN, STEP_DISABLED);

    stepper_evt_queue = xQueueCreate(QUEUE_SIZE, sizeof(uint32_t));
    xTaskCreate(stepperHandler, "stepper_task", 2048, stepper_evt_queue, 10, NULL);
}

void STEP_MoveSteps(uint32_t steps){
    xQueueSend(stepper_evt_queue, &steps,NULL);
}

