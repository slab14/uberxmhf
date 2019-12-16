#ifndef __UHFSM_H__
#define __UHFSM_H__

#include <stdint.h>
#include <inttypes.h>

#define UAPP_UHFSM_FUNCTION_SIGN    0x96

#ifndef __ASSEMBLY__

typedef struct {
  char eventString[256];
  uint32_t currentState;
  uint32_t newCurrentState;
  uint32_t samplingRate;
  uint32_t newSamplingRate;
  uint32_t defaultSamplingRate;
  uint32_t vaddr;
}uhfsm_param_t;


#endif // __ASSEMBLY__

#endif // __UHFSM_H__
