#include "power.h"
#include "driver/gpio.h"

static uint8_t dcdc_state;

uint8_t PWR_DCDC_State(void){
    return dcdc_state;
}

void PWR_Init(void){
    gpio_config_t io_conf;
    
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1<<PWR_DCDC_EN_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(PWR_DCDC_EN_PIN, PWR_DCDC_OFF);
    dcdc_state = PWR_DCDC_OFF;
}

void PWR_DCDC_EN(uint8_t en){
    if(en > PWR_DCDC_ON){
        return;
    }
    dcdc_state = en;
    gpio_set_level(PWR_DCDC_EN_PIN, en);
}