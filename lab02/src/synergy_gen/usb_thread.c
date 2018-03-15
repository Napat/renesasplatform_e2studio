/* generated thread source file - do not edit */
#include "usb_thread.h"

TX_THREAD usb_thread;
void usb_thread_create(void);
static void usb_thread_func(ULONG thread_input);
/** Alignment requires using pragma for IAR. GCC is done through attribute. */
#if defined(__ICCARM__)
#pragma data_alignment = BSP_STACK_ALIGNMENT
#endif
static uint8_t usb_thread_stack[2048] BSP_PLACE_IN_SECTION(".stack.usb_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void * p_instance, void * p_data);
void tx_startup_common_init(void);
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_rtc0) && !defined(SSP_SUPPRESS_ISR_RTC)
SSP_VECTOR_DEFINE(rtc_alarm_isr, RTC, ALARM);
#endif
#endif
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_rtc0) && !defined(SSP_SUPPRESS_ISR_RTC)
SSP_VECTOR_DEFINE(rtc_period_isr, RTC, PERIOD);
#endif
#endif
#if (8) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_rtc0) && !defined(SSP_SUPPRESS_ISR_RTC)
SSP_VECTOR_DEFINE( rtc_carry_isr, RTC, CARRY);
#endif
#endif
rtc_instance_ctrl_t g_rtc0_ctrl;
const rtc_cfg_t g_rtc0_cfg =
{ .clock_source = RTC_CLOCK_SOURCE_LOCO, .error_adjustment_value = 0,
  .error_adjustment_type = RTC_ERROR_ADJUSTMENT_NONE, .p_callback = NULL, .p_context = &g_rtc0, .alarm_ipl =
          (BSP_IRQ_DISABLED),
  .periodic_ipl = (BSP_IRQ_DISABLED), .carry_ipl = (8), };
/* Instance structure to use this module. */
const rtc_instance_t g_rtc0 =
{ .p_ctrl = &g_rtc0_ctrl, .p_cfg = &g_rtc0_cfg, .p_api = &g_rtc_on_rtc };
TX_EVENT_FLAGS_GROUP g_usb_plug_events;
TX_QUEUE g_adc_usb_queue;
static uint8_t queue_memory_g_adc_usb_queue[8];
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void usb_thread_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */
    UINT err_g_usb_plug_events;
    err_g_usb_plug_events = tx_event_flags_create (&g_usb_plug_events, (CHAR *) "USB_Plug_Events");
    if (TX_SUCCESS != err_g_usb_plug_events)
    {
        tx_startup_err_callback (&g_usb_plug_events, 0);
    }
    UINT err_g_adc_usb_queue;
    err_g_adc_usb_queue = tx_queue_create (&g_adc_usb_queue, (CHAR *) "ADC_USB_Queue", 1, &queue_memory_g_adc_usb_queue,
                                           sizeof(queue_memory_g_adc_usb_queue));
    if (TX_SUCCESS != err_g_adc_usb_queue)
    {
        tx_startup_err_callback (&g_adc_usb_queue, 0);
    }

    UINT err;
    err = tx_thread_create (&usb_thread, (CHAR *) "USB_Thread", usb_thread_func, (ULONG) NULL, &usb_thread_stack, 2048,
                            4, 4, 1, TX_AUTO_START);
    if (TX_SUCCESS != err)
    {
        tx_startup_err_callback (&usb_thread, 0);
    }
}

static void usb_thread_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    SSP_PARAMETER_NOT_USED (thread_input);

    /* Initialize common components */
    tx_startup_common_init ();

    /* Initialize each module instance. */

    /* Enter user code for this thread. */
    usb_thread_entry ();
}
