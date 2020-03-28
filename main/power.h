/**
 * 
 */
#ifndef _power_h_
#define _power_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PWR_DCDC_EN_PIN  13 // Shared with stepper enable
#define PWR_DCDC_OFF     0
#define PWR_DCDC_ON      1

void PWR_Init(void);
void PWR_DCDC_EN(uint8_t en);
uint8_t PWR_DCDC_State(void);

#ifdef __cplusplus
}
#endif

#endif