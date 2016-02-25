#ifndef _STM32_EVAL_WAVE_H_
#define _STM32_EVAL_WAVE_H_

#ifdef _cplusplus
	extern "C"{
#endif
#include "stm32f10x.h"
#include "platform_config.h"		
#include "Calculate.h"
#include "stm32_eval_spi.h"
#include "stm32f10x_it.h"
#include "stm32_eval.h"
		
extern void WaveExport(void);		
#ifdef _cplusplus
}
#endif
#endif