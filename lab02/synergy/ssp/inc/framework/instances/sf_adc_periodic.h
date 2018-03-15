/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 * 
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : sf_adc_periodic.h
 * Description  : RTOS integrated adc_periodic framework.
 **********************************************************************************************************************/

#ifndef SF_ADC_PERIODIC_H
#define SF_ADC_PERIODIC_H

/*******************************************************************************************************************//**
 * @ingroup SF_Library
 * @defgroup SF_ADC_PERIODIC ADC periodic Framework
 * @brief RTOS-integrated ADC Framework.
 *
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "sf_adc_periodic_api.h"

/** Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** Version of code that implements the API defined in this file */
#define SF_ADC_PERIODIC_CODE_VERSION_MAJOR (1U)
#define SF_ADC_PERIODIC_CODE_VERSION_MINOR (4U)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Channel instance control block. DO NOT INITIALIZE.  Initialization occurs when ::SF_ADC_PERIODIC_Open is called */
typedef struct st_sf_adc_periodic_instance_ctrl
{
    uint32_t               open;        ///< Used by driver to check if pointer to control block is valid
    TX_MUTEX               mutex;       ///< Mutex used to protect access to lower level driver hardware registers
    adc_instance_t const *   p_lower_lvl_adc;   ///< Pointer to the ADC instance
    timer_instance_t const * p_lower_lvl_timer; ///< Pointer to the Timer instance
    transfer_instance_t const * p_lower_lvl_transfer; ///< Pointer to the Transfer instance
    void const * volatile  p_src_transfer;  ///< Source pointer for the low level transfer method
    adc_data_size_t      * p_data_buffer;      ///< Pointer to the buffer that will store the samples
    uint32_t               data_buffer_length; ///< Length of the data buffer that will store the samples
    uint32_t               data_buffer_index;  ///< Index of the data buffer where data is to be written to next
    uint32_t               sample_count;       ///< Samples per channel to be buffered before notifying the app
    uint32_t               dtc_transfer_length;///< Total Length of DTC transfer for requested number of samples
    void                (* p_callback)(sf_adc_periodic_callback_args_t * p_args); ///< Callback function
    void const           * p_context;    ///< Placeholder for user data
} sf_adc_periodic_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const sf_adc_periodic_api_t g_sf_adc_periodic_on_sf_adc_periodic;
/** @endcond */

/*******************************************************************************************************************//**
 * @} (end defgroup SF_ADC_PERIODIC)
 **********************************************************************************************************************/

/** Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif /* SF_ADC_PERIODIC_H */
