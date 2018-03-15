/**************************************************************************/ 
/*                                                                        */ 
/*            Copyright (c) 1996-2012 by Express Logic Inc.               */ 
/*                                                                        */ 
/*  This software is copyrighted by and is the sole property of Express   */ 
/*  Logic, Inc.  All rights, title, ownership, or other interests         */ 
/*  in the software remain the property of Express Logic, Inc.  This      */ 
/*  software may only be used in accordance with the corresponding        */ 
/*  license agreement.  Any unauthorized use, duplication, transmission,  */ 
/*  distribution, or disclosure of this software is expressly forbidden.  */ 
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */ 
/*  written consent of Express Logic, Inc.                                */ 
/*                                                                        */ 
/*  Express Logic, Inc. reserves the right to modify this software        */ 
/*  without notice.                                                       */ 
/*                                                                        */ 
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               www.expresslogic.com          */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/

/***********************************************************************************************************************
 * Copyright [2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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

/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   SYNERGY Controller Driver                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_dcd_synergy.h"
#include "ux_device_stack.h"

/*******************************************************************************************************************//**
 * @addtogroup sf_el_ux
 * @{
 **********************************************************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                                 RELEASE      */
/*                                                                        */
/*    ux_dcd_synergy_function                               PORTABLE C    */
/*                                                             5.6        */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron, Express Logic Inc.                                   */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function dispatches the DCD function internally to the synergy */
/*    controller.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd                                   Pointer to device controller  */
/*    function                              Function requested            */
/*    parameter                             Pointer to function parameters*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */ 
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    ux_dcd_synergy_endpoint_create           Create endpoint            */
/*    ux_dcd_synergy_endpoint_destroy          Destroy endpoint           */
/*    ux_dcd_synergy_endpoint_reset            Reset endpoint             */
/*    ux_dcd_synergy_endpoint_stall            Stall endpoint             */
/*    ux_dcd_synergy_endpoint_status           Get endpoint status        */
/*    ux_dcd_synergy_frame_number_get          Get frame number           */
/*    ux_dcd_synergy_state_change              Change state               */
/*    ux_dcd_synergy_transfer_request          Request data transfer      */
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    USBX Device Stack                                                   */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  10-10-2012     TCRG                     Initial Version 5.6           */ 
/*                                                                        */ 
/**************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  This function act as interface between upper layer USBX device stack and synergy controller.
 *
 * @param[in,out]  dcd               : Pointer to a USBX control block.
 * @param[in,out]  function          : Function requested to be despatched.
 * @param[in,out]  parameter         : Pointer to requested function parameters.
 *
 * @retval UX_CONTROLLER_UNKNOWN      Desired controller is not specified.
 * @retval UX_FUNCTION_NOT_SUPPORTED  DCD function is not supported by the controller.
 * @retval UX_SUCCESS                 DCD function despatched successfully.

 * @return                See @ref for other possible return codes or causes.
 *                        This function calls:
 *                        * ux_dcd_synergy_frame_number_get()
 *                        * ux_dcd_synergy_transfer_request()
 *                        * ux_dcd_synergy_endpoint_create()
 *                        * ux_dcd_synergy_endpoint_destroy()
 *                        * ux_dcd_synergy_endpoint_reset()
 *                        * ux_dcd_synergy_endpoint_stall()
 *                        * ux_dcd_synergy_endpoint_status()
 **********************************************************************************************************************/
UINT  ux_dcd_synergy_function(UX_SLAVE_DCD *dcd, UINT function, VOID *parameter)
{
    UINT              status;
    UX_DCD_SYNERGY  * dcd_synergy;

    /* Check the status of the controller.  */
    if (dcd -> ux_slave_dcd_status == (UINT)UX_UNUSED)
    {
        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_CONTROLLER_UNKNOWN, 0, 0, 0, UX_TRACE_ERRORS, 0, 0)

        return (UINT)UX_CONTROLLER_UNKNOWN;
    }     
        
    /* Get the pointer to the synergy DCD.  */
    dcd_synergy =  (UX_DCD_SYNERGY *) dcd -> ux_slave_dcd_controller_hardware;

    /* Look at the function and route it.  */
    switch (function)
    {
    case (UINT)UX_DCD_GET_FRAME_NUMBER:

        status =  ux_dcd_synergy_frame_number_get(dcd_synergy, (ULONG *) parameter);
        break;

    case (UINT)UX_DCD_TRANSFER_REQUEST:

        status =  ux_dcd_synergy_transfer_request(dcd_synergy, (UX_SLAVE_TRANSFER *) parameter);
        break;

    case (UINT)UX_DCD_CREATE_ENDPOINT:

        status =  ux_dcd_synergy_endpoint_create(dcd_synergy, parameter);
        break;
        
    case (UINT)UX_DCD_DESTROY_ENDPOINT:

        status =  ux_dcd_synergy_endpoint_destroy(dcd_synergy, parameter);
        break;

    case (UINT)UX_DCD_RESET_ENDPOINT:

        status =  ux_dcd_synergy_endpoint_reset(dcd_synergy, parameter);
        break;

    case (UINT)UX_DCD_STALL_ENDPOINT:

        status =  ux_dcd_synergy_endpoint_stall(dcd_synergy, parameter);
        break;

    case (UINT)UX_DCD_SET_DEVICE_ADDRESS:

        status =  (UINT)UX_SUCCESS;
        break;

    case (UINT)UX_DCD_CHANGE_STATE:

        status =  (UINT)UX_SUCCESS;
        break;

    case (UINT)UX_DCD_ENDPOINT_STATUS:

        status =  ux_dcd_synergy_endpoint_status(dcd_synergy, (ULONG) parameter);
        break;

    default:

        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_FUNCTION_NOT_SUPPORTED, 0, 0, 0, UX_TRACE_ERRORS, 0, 0)

        status =  (UINT)UX_FUNCTION_NOT_SUPPORTED;
        break;
    }        

    /* Return completion status.  */
    return (status);
}
 /*******************************************************************************************************************//**
  * @} (end addtogroup sf_el_ux)
  **********************************************************************************************************************/
