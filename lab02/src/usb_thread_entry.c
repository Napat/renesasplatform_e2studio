#include "step_select.h"
#include "usb_thread.h"

#if(STEP_SELECT>3)
#define EVENT_USB_PLUG_IN           (1UL << 0)
#define EVENT_USB_PLUG_OUT          (1UL << 1)
#define UX_STORAGE_BUFFER_SIZE      (64*1024)
#define ASCII_OFFSET                (48)

#define LED_PIN_LEVEL_ON            0
#define LED_PIN_LEVEL_OFF           1

static FX_FILE  my_file;
static char local_buffer[UX_STORAGE_BUFFER_SIZE];


/* Constant data used for calculating the weekday and day of year */
static const uint8_t month_table[12]    = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
static const uint8_t ly_month_table[12] = {6, 2, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
static const uint8_t days_in_month[12]  = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static uint32_t weekday_get(rtc_time_t * p_time);
static uint32_t yearday_get(rtc_time_t * p_time);


void fx_media_init_function0(void);
#endif


/* USB Thread entry function */
void usb_thread_entry(void)
{
#if(STEP_SELECT>3)
    /* TODO: add your own code here */
    UINT status;
    FX_MEDIA * p_media;
    ULONG actual_flags;

    ssp_err_t ssp_err;
    volatile ioport_level_t ioport_0_6_level;
    rtc_time_t rtc_time;
    uint32_t rec_data = 0;


    uint8_t led_blink_count;

    /*
     * Example of __TIME__ string: "14:15:16"
     *                             [01234567]
     * Calculations based upon MACRO definitions on Stack Overflow
     * https://stackoverflow.com/questions/11697820/how-to-use-date-and-time-predefined-macros-in-as-two-integers-then-stri
     */
    rtc_time.tm_hour = ((__TIME__[0] - ASCII_OFFSET) * 10 + __TIME__[1] - ASCII_OFFSET);
    rtc_time.tm_min =  ((__TIME__[3] - ASCII_OFFSET) * 10 + __TIME__[4] - ASCII_OFFSET);
    rtc_time.tm_sec =  ((__TIME__[6] - ASCII_OFFSET) * 10 + __TIME__[7] - ASCII_OFFSET);

    /*
     * Example of __DATE__ string: "Jul  2 2017"    If day < 10, day is padded as shown
     * Example of __DATE__ string: "Jul 20 2017"
     *                             [01234567890]
     * Calculations based upon MACRO definitions on Stack Overflow
     * https://stackoverflow.com/questions/11697820/how-to-use-date-and-time-predefined-macros-in-as-two-integers-then-stri
     */
    rtc_time.tm_year = (__DATE__[ 7] - ASCII_OFFSET) * 1000 +  \
                       (__DATE__[ 8] - ASCII_OFFSET) * 100 + \
                       (__DATE__[ 9] - ASCII_OFFSET) * 10 + \
                       (__DATE__[10] - ASCII_OFFSET);

    rtc_time.tm_mday = ((__DATE__[4] >= ASCII_OFFSET) ? (__DATE__[4] - ASCII_OFFSET) * 10 : 0) + \
                        (__DATE__[5] - ASCII_OFFSET);

    rtc_time.tm_mon =  ((__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')) ?  0 : \
                       ((__DATE__[0] == 'F'))                                             ?  1 : \
                       ((__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')) ?  2 : \
                       ((__DATE__[0] == 'A' && __DATE__[1] == 'p'))                       ?  3 : \
                       ((__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')) ?  4 : \
                       ((__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')) ?  5 : \
                       ((__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')) ?  6 : \
                       ((__DATE__[0] == 'A' && __DATE__[1] == 'u'))                       ?  7 : \
                       ((__DATE__[0] == 'S'))                                             ?  8 : \
                       ((__DATE__[0] == 'O'))                                             ?  9 : \
                       ((__DATE__[0] == 'N'))                                             ? 10 : \
                       ((__DATE__[0] == 'D'))                                             ? 11 : 255; /* 255 == error */

    rtc_time.tm_wday = (int)weekday_get( &rtc_time );    /* Calculate the weekday (0 - 6) */
    rtc_time.tm_yday = (int)yearday_get( &rtc_time );    /* Calculate the year-day */
    rtc_time.tm_year = rtc_time.tm_year - 1900;



    ssp_err = g_rtc0.p_api->open(g_rtc0.p_ctrl, g_rtc0.p_cfg);
    if(SSP_SUCCESS != ssp_err)
    {
        __BKPT(0);   /* If open is unsuccessful, hit breakpoint */
    }


    ssp_err = g_rtc0.p_api->calendarTimeSet(g_rtc0.p_ctrl, &rtc_time, true);
    if(SSP_SUCCESS != ssp_err)
    {
        __BKPT(0);   /* If time set is unsuccessful, hit breakpoint */
    }
#endif


    while (1)
    {
        #if(STEP_SELECT>3)
        /* Wait until device inserted. */
        tx_event_flags_get (&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);

        /* Get the pointer to FileX Media Control Block for a USB flash device */
        p_media = g_fx_media0_ptr;

        if (p_media != NULL)
        {
            /* Turn on the Amber LED */
            g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_02, LED_PIN_LEVEL_ON);

            /* Set the default directory to the root.
             * If a value of FX_NULL is supplied, the
             * default directory is set to the media’s root directory
             * */
            status = fx_directory_default_set(p_media, FX_NULL);
            if (FX_SUCCESS != status)
            {
                __BKPT(0);
            }

            /* Now create a new directory from the root
             * Create a sub-directory called "adc_results" in the current default directory.
             * */
            status = fx_directory_create(p_media, "adc_results");
            if ((FX_SUCCESS != status) && (FX_ALREADY_CREATED != status))
            {
                __BKPT(0);
            }

            /* Now set the default directory as "adc_results */
            status = fx_directory_default_set(p_media, "\\adc_results");
            if (FX_SUCCESS != status)
            {
                __BKPT(0);
            }

            /* Try to open the file, "results.txt" */
            status = fx_file_open(p_media, &my_file, "results.txt", FX_OPEN_FOR_READ | FX_OPEN_FOR_WRITE);
            if (status != FX_SUCCESS)
            {
                /* The "results.txt" file is not found, so create a new file */
                status = fx_file_create(p_media, "results.txt");
                if (status != FX_SUCCESS)
                {
                    __BKPT(0);
                }

                /* Open that file */
                status = fx_file_open(p_media, &my_file, "results.txt", FX_OPEN_FOR_READ | FX_OPEN_FOR_WRITE);
                if (status != FX_SUCCESS)
                {
                    __BKPT(0);
                }
            }

            /* Now that the file is open, write our ADC results to it
             * Do this until S4 on the SK is pressed to unmount the USB drive
             * */
            do{
#if(STEP_SELECT==4)
                tx_thread_sleep(100);   /* Here we will do this every second.
                                         * In the real app we will pend on data in a queue
                                         */
#endif
#if(STEP_SELECT==5)
                /*** ENTER API CALL HERE TO RECEIVE DATA FROM THE ADC_USB QUEUE WITH INFINITE TIMEOUT ***/
                tx_queue_receive(&g_adc_usb_queue, &rec_data, TX_WAIT_FOREVER);
#endif

                /* Time stamp the results.
                 * Read the RTC value
                 */
                ssp_err = g_rtc0.p_api->calendarTimeGet(g_rtc0.p_ctrl, &rtc_time);
                if(SSP_SUCCESS != ssp_err)
                {
                    __BKPT(0);
                }

                /* Create a text string comprising of the adc value and time */
                sprintf(local_buffer, "%04d - %02d %02d %02d\n\r", (int)rec_data, rtc_time.tm_hour, rtc_time.tm_min, rtc_time.tm_sec);

                /* Write the text string to the file */
                status = fx_file_write(&my_file, local_buffer, strlen(local_buffer) );
                if (status != FX_SUCCESS)
                {
                    __BKPT(0);
                }

                /* Read the level of S4 on the S7G2-SK.
                 * If the S4 button is being pressed we will drop out of this loop,
                 * stop writing data to the file and close the file.
                 */
                g_ioport.p_api->pinRead(IOPORT_PORT_00_PIN_06, (ioport_level_t*)&ioport_0_6_level );

            } while(IOPORT_LEVEL_HIGH == ioport_0_6_level);

            /* Close the previously opened file */
            status = fx_file_close(&my_file);
            if (status != FX_SUCCESS)
            {
                __BKPT(0);
            }

            /* flush the media */
            status = fx_media_flush(p_media);
            if (status != FX_SUCCESS)
            {
                __BKPT(0);
            }

            /* close the media */
            status = fx_media_close(p_media);
            if (status != FX_SUCCESS)
            {
                __BKPT(0);
            }

            /* Indicate to the user that they can release S4 by toggling the LEDs */
            for(led_blink_count=0; led_blink_count<5; led_blink_count++)
            {
                g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_01, LED_PIN_LEVEL_ON);
                tx_thread_sleep(10);
                g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_01, LED_PIN_LEVEL_OFF);
                tx_thread_sleep(10);
            }
        }

        /* Now that the file has been closed, assume that the USB device is going to be removed
         * Wait for this to happen.
         * When removed the thread will wait for it to be inserted and then repeat the
         * file writing process
         */
        tx_event_flags_get (&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);

        /* Turn off Amber LED */
        g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_02, LED_PIN_LEVEL_OFF);
        #endif
    }
}


UINT cb_host_event(ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    SSP_PARAMETER_NOT_USED(instance);
    SSP_PARAMETER_NOT_USED(host_class);
#if(STEP_SELECT>3)
    UX_HOST_CLASS_STORAGE_MEDIA * p_ux_host_class_storage_media;

    /* Check if host_class is for Mass Storage class. */
    if (UX_SUCCESS == _ux_utility_memory_compare ( _ux_system_host_class_storage_name, host_class, _ux_utility_string_length_get (_ux_system_host_class_storage_name)))
    {
        /* Get the pointer to the media */
        ux_system_host_storage_fx_media_get (instance, &p_ux_host_class_storage_media, &g_fx_media0_ptr);

        switch (event)
        {
            case EVENT_USB_PLUG_IN:
                /* Notify the insertion of a USB Mass Storage device.
                 * clear the event flags
                 * */
                tx_event_flags_set (&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_AND);
                tx_event_flags_set (&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_AND);

                /* Set the event flag */
                tx_event_flags_set (&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_OR);

                /* Turn Green LED on */
                g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00, LED_PIN_LEVEL_ON);
                break;

            case EVENT_USB_PLUG_OUT:
                /* Notify the removal of a USB Mass Storage device.
                 * clear the event flags
                 * */
                tx_event_flags_set (&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_AND);
                tx_event_flags_set (&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_AND);

                /* Set the event flag */
                tx_event_flags_set (&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_OR);

                /* Turn Green LED off */
                g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00, LED_PIN_LEVEL_OFF);

                break;

            default:
                /* ignore this unsupported event */
                break;
        }
    }
    #endif
    return UX_SUCCESS;
}


#if(STEP_SELECT>3)
/*
 * Helper functions for creating the time and date
 *
 * weekday_get will return the day of week (0 - 6: Sunday - Saturday)
 * from the passed in data of year and month
 * */
static uint32_t weekday_get(rtc_time_t * p_time)
{
    uint32_t day = (uint32_t)p_time->tm_mday;
    bool leap = false;


    if (0 == p_time->tm_year % 4)
    {
        leap = true;
        if (0 == p_time->tm_year % 100)
        {
            leap = false;
            if (0 != p_time->tm_year % 400)
            {
                leap = true;
            }
        }
    }

    uint32_t month_offset;
    if (leap)
    {
        month_offset = ly_month_table[ p_time->tm_mon ];
    }
    else
    {
        month_offset = month_table[ p_time->tm_mon ];
    }

    uint32_t year_offset    = (uint32_t)(p_time->tm_year % 100);
    uint32_t century_offset = (uint32_t)((3 - ((p_time->tm_year / 100) % 4)) * 2);
    uint32_t offset         = day + month_offset + year_offset + (year_offset / 4) + century_offset;
    uint32_t index          = offset % 7;

    return index;   /* Return 0 to 6, where:
                     * Sun = 0, Mon = 1, Tue = 2, Wed = 3, Thu = 4, Fri = 5, Saturday = 6
                     */
}

/*
 * Helper functions for creating the time and date
 *
 * yearday_get will return the day of the year
 * from the passed in data of year and day of month
 * */
static uint32_t yearday_get(rtc_time_t * p_time)
{
    uint8_t month_count;
    uint32_t year_day = 0;

    /* Add the number of elapsed months days together */
    /* For example, if the month is June, add Jan(31) + Feb(28) + Mar(31) + April(30) + May(31) */
    for( month_count=0; month_count<(p_time->tm_mon); month_count++ )
    {
        year_day += days_in_month[month_count];
    }

    /* For example, if the day is June 26
     * Add 26 to this total */
    year_day += (uint32_t) p_time->tm_mday;

    /* If we are in a leap year, and the month has passed Feb, add an extra day to the total */
    bool leap = false;

    if (0 == p_time->tm_year % 4)
    {
        leap = true;
        if (0 == p_time->tm_year % 100)
        {
            leap = false;
            if (0 != p_time->tm_year % 400)
            {
                leap = true;
            }
        }
    }

    if ((leap) && (p_time->tm_mon > 1))
    {
        year_day++;
    }

    return (year_day);
}
#endif
