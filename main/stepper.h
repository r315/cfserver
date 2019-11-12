

/**
 * 
 */
#ifndef _stepper_h_
#define _stepper_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define QUEUE_SIZE 5
#define STEP_CLK_PIN    14
#define STEP_DIR_PIN    12
#define STEP_EN_PIN     13

#define STEP_CW         0
#define STEP_CCW        1

#define STEP_DISABLED   0
#define STEP_ENABLED    1

#define STEP_DEFAULT_PULSE  7000
#define STEP_MIN_PULSE      1000


typedef struct _StpMove{
    uint32_t pulses;
    uint32_t time;
    uint8_t dir;
}StpMove;

void STEP_Init(void);
void STEP_MoveSteps(uint32_t steps);
void STEP_SetPulseWidth(uint32_t time);
void STEP_Move(uint32_t steps, uint32_t time, uint8_t dir);

#ifdef __cplusplus
}
#endif

#endif