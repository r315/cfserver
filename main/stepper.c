
#include <esp_log.h>
#include <esp_system.h>

#include "stepper.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_console.h"


#define STEP_CLK_PIN    14
#define STEP_DIR_PIN    12
#define STEP_EN_PIN     13

#define STEP_CW         0
#define STEP_CCW        1

#define STEP_DISABLED   0
#define STEP_ENABLED    1

#define STEP_DEFAULT_PULSE  7000
#define STEP_MIN_PULSE      1000

static xQueueHandle stepper_evt_queue = NULL;
static const char *TAG = "STEP";

static uint32_t pulse_width;

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
    uint32_t steps;
    xQueueHandle *stepper_evt_queue = (xQueueHandle*)queue;

    for (;;) {
        if (xQueueReceive(stepper_evt_queue, &steps, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Moving [%d] steps", steps);            
            pulseStep(pulse_width, steps, STEP_CW);            
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

    pulse_width = STEP_DEFAULT_PULSE;
    stepper_evt_queue = xQueueCreate(QUEUE_SIZE, sizeof(uint32_t));
    xTaskCreate(stepperHandler, "stepper_task", 2048, stepper_evt_queue, 10, NULL);
}

void STEP_MoveSteps(uint32_t steps){
    xQueueSend(stepper_evt_queue, &steps, 0);
}

static int set_pulse(int argc, char **argv){
uint32_t pulse = atoi(argv[1]);

    if(pulse < STEP_MIN_PULSE)
    {
        ESP_LOGI(TAG,"Invalid pulse\n");    
        return 0;
    }

    ESP_LOGI(TAG,"Setting pulse to: %uus\n", pulse);    
    pulse_width = pulse;
    return 0;
}

static int move_steps(int argc, char **argv){
uint32_t steps = atoi(argv[1]);
    ESP_LOGI(TAG,"moving %u steps\n", steps);    
    STEP_MoveSteps(steps);
    return 0;
}


void register_stepper(void){
    const esp_console_cmd_t step_pulse = {
        .command = "step_pulse",
        .help = "Set step pulse width in ms",
        .hint = NULL,
        .func = &set_pulse,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&step_pulse) );

    const esp_console_cmd_t step_move = {
        .command = "step_move",
        .help = "Move in steps",
        .hint = NULL,
        .func = &move_steps,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&step_move) );
}