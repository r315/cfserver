

/**
 * 
 */
#ifndef _steper_h_
#define _steper_h_

#ifdef __cplusplus
extern "C" {
#endif


#define QUEUE_SIZE 5

void STEP_Init(void);
void STEP_MoveSteps(uint32_t steps);

#ifdef __cplusplus
}
#endif

#endif