/* generated thread header file - do not edit */
#ifndef USB_THREAD_H_
#define USB_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus 
extern "C" void usb_thread_entry(void);
#else 
extern void usb_thread_entry(void);
#endif
#include "r_rtc.h"
#include "r_rtc_api.h"
#ifdef __cplusplus
extern "C"
{
#endif
/** RTC on RTC Instance. */
extern const rtc_instance_t g_rtc0;
#ifndef NULL
void NULL(rtc_callback_args_t * p_args);
#endif
extern TX_EVENT_FLAGS_GROUP g_usb_plug_events;
extern TX_QUEUE g_adc_usb_queue;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* USB_THREAD_H_ */
