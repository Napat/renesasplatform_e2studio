/* generated thread header file - do not edit */
#ifndef DAC_THREAD_H_
#define DAC_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus 
extern "C" void dac_thread_entry(void);
#else 
extern void dac_thread_entry(void);
#endif
#include "r_dac.h"
#include "r_dac_api.h"
#ifdef __cplusplus
extern "C"
{
#endif
/** DAC on DAC Instance. */
extern const dac_instance_t g_dac0;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* DAC_THREAD_H_ */
