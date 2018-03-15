#include "step_select.h"
#include "adc_thread.h"

#if(STEP_SELECT>2)
#define EVENT_FLAG_1    0x00000001  // b 0000-0000-0000-0000-0000-0000-0000-0001
#define EVENT_FLAG_2    0x00000002  // b 0000-0000-0000-0000-0000-0000-0000-0010

#define ALL_FLAGS       (EVENT_FLAG_1 | EVENT_FLAG_2)
                                    // b 0000-0000-0000-0000-0000-0000-0000-0011
uint32_t g_adc_average;

#endif

#if(STEP_SELECT>4)
extern TX_QUEUE g_adc_usb_queue;
#endif


/* ADC Thread entry function */
void adc_thread_entry(void)
{
#if(STEP_SELECT>2)
    /* TODO: add your own code here */
    ssp_err_t ssp_err;
    ioport_level_t toggle = 0;
    uint32_t actual_flags;
    uint8_t loop_counter;
    uint8_t offset = 0;
#endif

#if(STEP_SELECT>4)
    UINT status;
#endif

#if(STEP_SELECT>2)
    /*** ENTER API CALL HERE TO START THE PERIODIC FRAMEWORK ***/
    ssp_err = g_sf_adc_periodic0.p_api->start(g_sf_adc_periodic0.p_ctrl);

    if(SSP_SUCCESS != ssp_err)
    {
        __BKPT(0);
    }

    while (1)
    {
        /* Wait forever until either of the 2 event flags are set, and then clear them (TX_OR_CLEAR)
         * The actual event flag set will be stored in actual_flags.
         */
        /*** ENTER API CALL HERE TO GET AN EVENT FLAG ***/
        tx_event_flags_get(&g_adc_flags, ALL_FLAGS, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);

        /* Toggle the state of LEDs on the S7G2-SK */
        g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00, toggle);
        toggle = 1 - toggle;
        g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_02, toggle);

        /* Calculate an average ADC value of the 32 sampled values */
        g_adc_average = 0;
        /* Read the data from the data buffer, pointed to by p_data_buffer.
           * The buffer is 64 results long, with 32 results being valid at any one time.
           * Calculate the correct index [0] or [32] into the buffer from the event flag set
           * via the callback function and the sample count length.
           * The buffer that is full will have been reported back via the event flag (1 or 2)
           * and is stored in actual_flags.
           * If the event flag = 1, the offset is 0. If the event flag = 2, the offset is 32.
           * Then read 32 bytes of data and create an average.
           */
          if( EVENT_FLAG_1 == (actual_flags & EVENT_FLAG_1))
          {
              offset = 0;
          }
          else
          {
              offset = (uint8_t)g_sf_adc_periodic0.p_cfg->sample_count;
          }

          for(loop_counter=0; loop_counter<g_sf_adc_periodic0.p_cfg->sample_count; loop_counter++)
          {
              g_adc_average += g_sf_adc_periodic0.p_cfg->p_data_buffer[ offset + loop_counter ];
          }
          g_adc_average = g_adc_average / g_sf_adc_periodic0.p_cfg->sample_count;
#endif

#if(STEP_SELECT>4)
        /* Send the adc_average value to the queue g_adc_usb_queue
         * If a USB memory device is inserted, then the USB thread will be
         * pending on data in this queue
         * */
        /*** ENTER API CALL HERE TO SEND DATA TO THE ADC_USB QUEUE WITH A 500ms TIMEOUT ***/
        status = tx_queue_send(&g_adc_usb_queue, &g_adc_average, 50);

        if( TX_QUEUE_FULL == status )
        {
            /* If the USB memory device is not inserted then no data will
             * be read from the queue.
             * Therefore, the previous instruction "tx_queue_send" will
             * wait for 500ms with the status TX_QUEUE_FULL
             * If this is the case, flush the queue.
             */
            tx_queue_flush(&g_adc_usb_queue);
        }
#endif
    }
}

void g_adc_framework_user_callback(sf_adc_periodic_callback_args_t * p_args)
{
    if(p_args->event == SF_ADC_PERIODIC_EVENT_NEW_DATA)
    {
        if(p_args->buffer_index == 0)
        {
            /* Clear the event flag prior to setting, just-in-case it is still set */
            tx_event_flags_set(&g_adc_flags, EVENT_FLAG_1, TX_AND);

            /* Set the event flag */
            tx_event_flags_set(&g_adc_flags, EVENT_FLAG_1, TX_OR);
        }
        else
        {
            /* Clear the event flag prior to setting, just-in-case it is still set */
            tx_event_flags_set(&g_adc_flags, EVENT_FLAG_2, TX_AND);

            /* Set the event flag */
            tx_event_flags_set(&g_adc_flags, EVENT_FLAG_2, TX_OR);
        }
    }
}
