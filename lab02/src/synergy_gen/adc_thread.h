/* generated thread header file - do not edit */
#ifndef ADC_THREAD_H_
#define ADC_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus 
extern "C" void adc_thread_entry(void);
#else 
extern void adc_thread_entry(void);
#endif
#include "r_dtc.h"
#include "r_transfer_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_adc.h"
#include "r_adc_api.h"
#include "sf_adc_periodic.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer0;
#ifndef NULL
void NULL(transfer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_gpt1;
#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** ADC on ADC Instance. */
extern const adc_instance_t g_adc0;
#ifndef NULL
void NULL(adc_callback_args_t * p_args);
#endif
/** ADC Periodic on ADC Periodic instance */
extern sf_adc_periodic_instance_t g_sf_adc_periodic0;
/** Buffer where the sampled data will be stored for application usage */
extern uint16_t g_user_buffer[64];
#ifndef g_adc_framework_user_callback
void g_adc_framework_user_callback(sf_adc_periodic_callback_args_t * p_args);
#endif
void g_sf_adc_periodic0_err_callback(void * p_instance, void * p_data);
void sf_adc_periodic_init0(void);
extern TX_EVENT_FLAGS_GROUP g_adc_flags;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ADC_THREAD_H_ */
