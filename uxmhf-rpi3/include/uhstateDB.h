#ifndef __UHSTATEDB_H__
#define __UHSTATEDB_H__

#define UAPP_UHSTATEDB_FUNCTION_INIT    0xAA
#define UAPP_UHSTATEDB_FUNCTION_GET     0xAB
#define UAPP_UHSTATEDB_FUNCTION_NEXT    0xAC

#define MAX_STATES 100

#ifndef __ASSEMBLY__

typedef struct {
  uint32_t maxArray[MAX_STATES];
  uint32_t numStates;
  uint32_t vaddr;
  uint32_t deviceID;  
  uint32_t stateVal;
}uhstatedb_param_t;


#endif // __ASSEMBLY__

#endif // __UAGENT_H__
