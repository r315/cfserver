
#include <esp_log.h>
#include <esp_system.h>

#include "stepper.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_console.h"

static xQueueHandle stepper_evt_queue = NULL;
static const char *TAG = "STEP";

StpMove default_stp = {
    0,
    STEP_DEFAULT_PULSE,
    STEP_CW
};

static void pulseStep(uint32_t pulse, uint32_t pulses, uint8_t dir){
    gpio_set_level(STEP_DIR_PIN, dir);
    gpio_set_level(STEP_EN_PIN, STEP_ENABLED);
    while(pulses--){
        gpio_set_level(STEP_CLK_PIN, 1);
        ets_delay_us(pulse);
        gpio_set_level(STEP_CLK_PIN, 0);
        ets_delay_us(pulse);
    }
    gpio_set_level(STEP_EN_PIN, STEP_DISABLED);
}

static void stepperHandler(void *queue)
{
    StpMove stp;
    xQueueHandle *stepper_evt_queue = (xQueueHandle*)queue;

    for (;;) {
        if (xQueueReceive(stepper_evt_queue, &stp, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Moving [%d] steps", stp.pulses);            
            pulseStep(stp.time, stp.pulses, STEP_CW);            
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

    stepper_evt_queue = xQueueCreate(QUEUE_SIZE, sizeof(StpMove));
    xTaskCreate(stepperHandler, "stepper_task", 2048, stepper_evt_queue, 10, NULL);
}

void STEP_MoveSteps(uint32_t steps){
    default_stp.pulses = steps;
    xQueueSend(stepper_evt_queue, &default_stp, 0);
}

void STEP_SetPulseWidth(uint32_t time){
    default_stp.time = time;
}

void STEP_Move(uint32_t steps, uint32_t time, uint8_t dir){
    StpMove stp;
    stp.pulses = steps;
    stp.time = time;
    stp.dir = dir;
    xQueueSend(stepper_evt_queue, &stp, 0);
}

