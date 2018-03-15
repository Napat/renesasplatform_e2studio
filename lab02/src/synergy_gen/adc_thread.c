/* generated thread source file - do not edit */
#include "adc_thread.h"

TX_THREAD adc_thread;
void adc_thread_create(void);
static void adc_thread_func(ULONG thread_input);
/** Alignment requires using pragma for IAR. GCC is done through attribute. */
#if defined(__ICCARM__)
#pragma data_alignment = BSP_STACK_ALIGNMENT
#endif
static uint8_t adc_thread_stack[1024] BSP_PLACE_IN_SECTION(".stack.adc_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void * p_instance, void * p_data);
void tx_startup_common_init(void);
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer0) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_ELC_SOFTWARE_EVENT_0)
#define DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_0);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0
#endif
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_1) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_1);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1
#endif
#endif
#endif

dtc_instance_ctrl_t g_transfer0_ctrl;
transfer_info_t g_transfer0_info =
{ .dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED, .repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED, .size =
          TRANSFER_SIZE_2_BYTE,
  .mode = TRANSFER_MODE_BLOCK, .p_dest = (void *) NULL, .p_src = (void const *) NULL, .num_blocks = 1, .length = 1, };
const transfer_cfg_t g_transfer0_cfg =
{ .p_info = &g_transfer0_info, .activation_source = ELC_EVENT_ELC_SOFTWARE_EVENT_0, .auto_enable = false, .p_callback =
          NULL,
  .p_context = &g_transfer0, .irq_ipl = (BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 =
{ .p_ctrl = &g_transfer0_ctrl, .p_cfg = &g_transfer0_cfg, .p_api = &g_transfer_on_dtc };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_gpt1) && !defined(SSP_SUPPRESS_ISR_GPT1)
SSP_VECTOR_DEFINE_CHAN(gpt_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 1);
#endif
#endif
static gpt_instance_ctrl_t g_gpt1_ctrl;
static const timer_on_gpt_cfg_t g_gpt1_extend =
{ .gtioca =
{ .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
  .gtiocb =
  { .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW } };
static const timer_cfg_t g_gpt1_cfg =
{ .mode = TIMER_MODE_PERIODIC, .period = 31250, .unit = TIMER_UNIT_PERIOD_USEC, .duty_cycle = 50, .duty_cycle_unit =
          TIMER_PWM_UNIT_RAW_COUNTS,
  .channel = 1, .autostart = false, .p_callback = NULL, .p_context = &g_gpt1, .p_extend = &g_gpt1_extend, .irq_ipl =
          (BSP_IRQ_DISABLED), };
/* Instance structure to use this module. */
const timer_instance_t g_gpt1 =
{ .p_ctrl = &g_gpt1_ctrl, .p_cfg = &g_gpt1_cfg, .p_api = &g_timer_on_gpt };
#if (6) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc0) && !defined(SSP_SUPPRESS_ISR_ADC0)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_isr, ADC, SCAN_END, 0);
#endif
#endif
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc0) && !defined(SSP_SUPPRESS_ISR_ADC0)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_b_isr, ADC, SCAN_END_B, 0);
#endif
#endif
adc_instance_ctrl_t g_adc0_ctrl;
const adc_cfg_t g_adc0_cfg =
{ .unit = 0, .mode = ADC_MODE_SINGLE_SCAN, .resolution = ADC_RESOLUTION_12_BIT, .alignment = ADC_ALIGNMENT_RIGHT,
  .add_average_count = ADC_ADD_OFF, .clearing = ADC_CLEAR_AFTER_READ_ON, .trigger = ADC_TRIGGER_SYNC_ELC,
  .trigger_group_b = ADC_TRIGGER_SYNC_ELC, .p_callback = NULL, .p_context = &g_adc0, .scan_end_ipl = (6),
  .scan_end_b_ipl = (BSP_IRQ_DISABLED), };
const adc_channel_cfg_t g_adc0_channel_cfg =
{ .scan_mask = (uint32_t) (
        ((uint64_t) ADC_MASK_CHANNEL_0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | (0)),
  /** Group B channel mask is right shifted by 32 at the end to form the proper mask */
  .scan_mask_group_b = (uint32_t) (
          (((uint64_t) ADC_MASK_CHANNEL_0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | (0)) >> 32),
  .priority_group_a = ADC_GROUP_A_PRIORITY_OFF, .add_mask = (uint32_t) (
          (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)
                  | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)),
  .sample_hold_mask = (uint32_t) ((0) | (0) | (0)), .sample_hold_states = 24, };
/* Instance structure to use this module. */
const adc_instance_t g_adc0 =
{ .p_ctrl = &g_adc0_ctrl, .p_cfg = &g_adc0_cfg, .p_channel_cfg = &g_adc0_channel_cfg, .p_api = &g_adc_on_adc };
#if defined(__ICCARM__)
#define g_sf_adc_periodic0_err_callback_WEAK_ATTRIBUTE
#pragma weak g_sf_adc_periodic0_err_callback  = g_sf_adc_periodic0_err_callback_internal
#elif defined(__GNUC__)
#define g_sf_adc_periodic0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_adc_periodic0_err_callback_internal")))
#endif
void g_sf_adc_periodic0_err_callback(void * p_instance, void * p_data)
g_sf_adc_periodic0_err_callback_WEAK_ATTRIBUTE;
#if ADC_PERIODIC_ON_ADC_PERIODIC_CALLBACK_USED_g_sf_adc_periodic0
void g_adc_framework_user_callback(sf_adc_periodic_callback_args_t * p_args);
#endif
uint16_t g_user_buffer[64];
sf_adc_periodic_instance_ctrl_t g_sf_adc_periodic0_ctrl;
sf_adc_periodic_cfg_t g_sf_adc_periodic0_cfg =
{ .p_lower_lvl_adc = &g_adc0, .p_lower_lvl_timer = &g_gpt1, .p_lower_lvl_transfer = &g_transfer0, .p_data_buffer =
          g_user_buffer,
  .data_buffer_length = 64, .sample_count = 32, .p_callback = g_adc_framework_user_callback, .p_extend = NULL, };

sf_adc_periodic_instance_t g_sf_adc_periodic0 =
        { .p_ctrl = &g_sf_adc_periodic0_ctrl, .p_cfg = &g_sf_adc_periodic0_cfg, .p_api =
                  &g_sf_adc_periodic_on_sf_adc_periodic, };
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_sf_adc_periodic0_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_adc_periodic0_err_callback_internal(void * p_instance, void * p_data);
void g_sf_adc_periodic0_err_callback_internal(void * p_instance, void * p_data)
{
    /** Suppress compiler warning for not using parameters. */
    SSP_PARAMETER_NOT_USED (p_instance);
    SSP_PARAMETER_NOT_USED (p_data);

    /** An error has occurred. Please check function arguments for more information. */
    BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
}

/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.

 *            - void sf_adc_periodic_init0(void)
 **********************************************************************************************************************/
void sf_adc_periodic_init0(void)
{
    ssp_err_t ssp_err_g_sf_adc_periodic0;
    ssp_err_g_sf_adc_periodic0 = g_sf_adc_periodic0.p_api->open (g_sf_adc_periodic0.p_ctrl, g_sf_adc_periodic0.p_cfg);
    if (SSP_SUCCESS != ssp_err_g_sf_adc_periodic0)
    {
        g_sf_adc_periodic0_err_callback ((void *) &g_sf_adc_periodic0, &ssp_err_g_sf_adc_periodic0);
    }
}
TX_EVENT_FLAGS_GROUP g_adc_flags;
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void adc_thread_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */
    UINT err_g_adc_flags;
    err_g_adc_flags = tx_event_flags_create (&g_adc_flags, (CHAR *) "New Event Flags");
    if (TX_SUCCESS != err_g_adc_flags)
    {
        tx_startup_err_callback (&g_adc_flags, 0);
    }

    UINT err;
    err = tx_thread_create (&adc_thread, (CHAR *) "ADC_Thread", adc_thread_func, (ULONG) NULL, &adc_thread_stack, 1024,
                            3, 3, 1, TX_AUTO_START);
    if (TX_SUCCESS != err)
    {
        tx_startup_err_callback (&adc_thread, 0);
    }
}

static void adc_thread_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    SSP_PARAMETER_NOT_USED (thread_input);

    /* Initialize common components */
    tx_startup_common_init ();

    /* Initialize each module instance. */
    /** Call initialization function if user has selected to do so. */
#if (1)
    sf_adc_periodic_init0 ();
#endif

    /* Enter user code for this thread. */
    adc_thread_entry ();
}
