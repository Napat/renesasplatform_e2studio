/* generated thread source file - do not edit */
#include "dac_thread.h"

TX_THREAD dac_thread;
void dac_thread_create(void);
static void dac_thread_func(ULONG thread_input);
/** Alignment requires using pragma for IAR. GCC is done through attribute. */
#if defined(__ICCARM__)
#pragma data_alignment = BSP_STACK_ALIGNMENT
#endif
static uint8_t dac_thread_stack[1024] BSP_PLACE_IN_SECTION(".stack.dac_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void * p_instance, void * p_data);
void tx_startup_common_init(void);
dac_instance_ctrl_t g_dac0_ctrl;
const dac_cfg_t g_dac0_cfg =
{ .channel = 0, .ad_da_synchronized = false, .data_format = DAC_DATA_FORMAT_FLUSH_RIGHT, .output_amplifier_enabled =
          false,
  .p_extend = NULL };
/* Instance structure to use this module. */
const dac_instance_t g_dac0 =
{ .p_ctrl = &g_dac0_ctrl, .p_cfg = &g_dac0_cfg, .p_api = &g_dac_on_dac };
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void dac_thread_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */

    UINT err;
    err = tx_thread_create (&dac_thread, (CHAR *) "DAC_Thread", dac_thread_func, (ULONG) NULL, &dac_thread_stack, 1024,
                            5, 5, 1, TX_AUTO_START);
    if (TX_SUCCESS != err)
    {
        tx_startup_err_callback (&dac_thread, 0);
    }
}

static void dac_thread_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    SSP_PARAMETER_NOT_USED (thread_input);

    /* Initialize common components */
    tx_startup_common_init ();

    /* Initialize each module instance. */

    /* Enter user code for this thread. */
    dac_thread_entry ();
}
