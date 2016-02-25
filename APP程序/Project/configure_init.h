#ifndef _CONFIGURE_INIT_H_
#define _CONFIGURE_INIT_H_

#ifdef _cplusplus
  extern "C" {
#endif

/***********includes******************/
#include "stm32f10x.h"
#include "platform_config.h"
#include "Calculate.h"
#include "stm32_eval.h"


/***********Function Declare**********/		
extern void ReportState(void);
extern void ErrorCodeLimit(void);
extern void RepeatCodeLimit(void);
extern void HandleRecData(void);
#ifdef _cplusplus
}
#endif
#endif