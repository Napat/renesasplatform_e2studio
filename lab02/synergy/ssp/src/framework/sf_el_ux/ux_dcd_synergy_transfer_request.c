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
#include "ux_utility.h"
#include "ux_device_stack.h"

/*******************************************************************************************************************//**
 * @addtogroup sf_el_ux
 * @{
 **********************************************************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                                RELEASE       */
/*                                                                        */
/*    ux_dcd_synergy_transfer_request                      PORTABLE C     */
/*                                                            5.6         */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron, Express Logic Inc.                                   */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will initiate a transfer to a specific endpoint.      */
/*    If the endpoint is IN, the endpoint register will be set to accept  */
/*    the request.                                                        */
/*                                                                        */
/*    If the endpoint is IN, the endpoint FIFO will be filled with the    */
/*    buffer and the endpoint register set.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd_synergy                         Pointer to device controller    */
/*    transfer_request                    Pointer to transfer request     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */ 
/*                                                                        */
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    ux_dcd_synergy_register_read             Read register              */
/*    ux_dcd_synergy_register_set              Set register               */
/*    ux_dcd_synergy_register_write            Write register             */
/*    _ux_utility_semaphore_get                Get semaphore              */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    SYNERGY Controller Driver                                           */
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
 * @brief  This function will initiate a transfer to a specific endpoint.
 *         If the endpoint is IN, the endpoint register will be set to accept the request.
 *         If the endpoint is IN, the endpoint FIFO will be filled with the buffer and the endpoint
 *         register set.
 *
 * @param[in,out]  dcd_synergy       : Pointer to a DCD control block
 * @param[in,out]  transfer_request  : Pointer to transfer request
 *
 * @retval UX_SUCCESS                                 Transfer to a specific endpoint is initiated successfully.
 * @retval ux_slave_transfer_request_completion_code  Pointer to structure UX_SLAVE_TRANSFER(transfer request completion code).
 * @retval UX_TRANSFER_ERROR                          Transfer is completed with error.
 *
 * @return               See @ref for other possible return codes or causes.
 *                       This function calls:
 *                       * ux_dcd_synergy_buffer_write()
 **********************************************************************************************************************/
UINT  ux_dcd_synergy_transfer_request(UX_DCD_SYNERGY *dcd_synergy, UX_SLAVE_TRANSFER *transfer_request)
{
    UX_DCD_SYNERGY_ED * ed;
    UCHAR             * fifo_buffer;
    ULONG               fifo_length;
    UX_SLAVE_ENDPOINT * endpoint;
    UINT                status;
    ULONG               pipectr_reg;

    /* Get the pointer to the logical endpoint from the transfer request.  */
    endpoint =  transfer_request -> ux_slave_transfer_request_endpoint;

    /* Get the physical endpoint from the logical endpoint.  */
    ed =  (UX_DCD_SYNERGY_ED *) endpoint -> ux_slave_endpoint_ed;
    
    /* Set PID to NAK. Isolate Control endpoint and others.  */
    if (ed -> ux_dcd_synergy_ed_index == 0UL)
    {
        /* Set PID to NAK for Control Endpoint.  */
        ux_dcd_synergy_register_clear(dcd_synergy, UX_SYNERGY_DCD_DCPCTR, UX_SYNERGY_DCD_PIPECTR_PID_MASK);
    }
    else
    {
        pipectr_reg = ux_dcd_synergy_register_read(dcd_synergy,
                                           UX_SYNERGY_DCD_PIPE1CTR + ((ed -> ux_dcd_synergy_ed_index - 1UL) * 2UL));

        while (pipectr_reg & UX_SYNERGY_DCD_DCPCTR_INBUFM )
        {
            pipectr_reg = ux_dcd_synergy_register_read(dcd_synergy,
                                           UX_SYNERGY_DCD_PIPE1CTR + ((ed -> ux_dcd_synergy_ed_index - 1UL) * 2UL));
        }

        /* Set PID to NAK for non Control Endpoints.  */
        ux_dcd_synergy_register_clear(dcd_synergy, UX_SYNERGY_DCD_PIPE1CTR +
                                    ((ed -> ux_dcd_synergy_ed_index - 1UL) * 2UL), UX_SYNERGY_DCD_PIPECTR_PID_MASK);
        
    }

    /* Clear the BRDY and BEMP status for this pipe.  */
    ux_dcd_synergy_register_clear(dcd_synergy, UX_SYNERGY_DCD_BEMPSTS, (USHORT)(1UL << ed -> ux_dcd_synergy_ed_index));
    ux_dcd_synergy_register_clear(dcd_synergy, UX_SYNERGY_DCD_BRDYSTS, (USHORT)(1UL << ed -> ux_dcd_synergy_ed_index));

    /* Check for transfer direction.  */
    if (transfer_request -> ux_slave_transfer_request_phase == (ULONG)UX_TRANSFER_PHASE_DATA_OUT)
    {
        /* Data Out. So we expect a IN transaction from the host.  */
        /* Get the size of the transfer, used for a IN transaction only.  */
        fifo_length =  transfer_request -> ux_slave_transfer_request_requested_length;

        /* Keep the FIFO length in the endpoint.  */
        ed -> ux_dcd_synergy_ed_payload_length =  fifo_length;

        /* Check if the endpoint size is bigger that data requested. */
        if(fifo_length > endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize)
        {
            /* Adjust the transfer size.  */
            fifo_length =  endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
        }

        /* Point the FIFO buffer to the current transfer request buffer address.  */
        fifo_buffer =  transfer_request -> ux_slave_transfer_request_data_pointer;
        
        /* Keep the FIFO length in the endpoint.  */
        ed -> ux_dcd_synergy_ed_payload_address =  fifo_buffer;

        /* Set the current fifo port.  */
        ux_dcd_synergy_current_endpoint_change(dcd_synergy, ed, UX_SYNERGY_DCD_CFIFOSEL_ISEL);

        /* Clear the FIFO buffer memory. */
        ux_dcd_synergy_register_write(dcd_synergy, UX_SYNERGY_DCD_CFIFOCTR, UX_SYNERGY_DCD_FIFOCTR_BCLR);

#if !defined(BSP_MCU_GROUP_S124) && !defined(BSP_MCU_GROUP_S128)
        ux_dcd_synergy_register_write(dcd_synergy, UX_SYNERGY_DCD_D0FIFOCTR, UX_SYNERGY_DCD_FIFOCTR_BCLR);
#endif

        /* Point the FIFO buffer to the current transfer request buffer address.  */
        fifo_buffer =  transfer_request -> ux_slave_transfer_request_data_pointer;
        
        /* Keep the FIFO length in the endpoint.  */
        ed -> ux_dcd_synergy_ed_payload_address =  fifo_buffer;
        
        /* Write the buffer to the pipe.  */
        status =  ux_dcd_synergy_buffer_write(dcd_synergy, ed);
        
        /* Check status.  */
        if (status != (UINT)UX_ERROR)
        {        
            /* Adjust the FIFO length in the endpoint.  */
            ed -> ux_dcd_synergy_ed_payload_length -=  fifo_length;

            /* Adjust the data pointer.  */
            transfer_request -> ux_slave_transfer_request_current_data_pointer += fifo_length;
            
            /* Update the length of the data transmitted.  */
            transfer_request -> ux_slave_transfer_request_actual_length += fifo_length;
              
            /* Adjust the transfer length remaining.  */
            transfer_request -> ux_slave_transfer_request_in_transfer_length -= fifo_length;

            /* Set PID to BUF. Isolate Control endpoint and others.  */
            if (ed -> ux_dcd_synergy_ed_index == 0UL)
            {
                /* Set PID to BUF for Control Endpoint.  */
                ux_dcd_synergy_register_set(dcd_synergy, UX_SYNERGY_DCD_DCPCTR, UX_SYNERGY_DCD_PIPECTR_PIDBUF);
            }
            else
            {
                /* Set PID to BUF for non Control Endpoints.  */
                ux_dcd_synergy_register_set(dcd_synergy, UX_SYNERGY_DCD_PIPE1CTR + ((ed -> ux_dcd_synergy_ed_index - 1) * 2), UX_SYNERGY_DCD_PIPECTR_PIDBUF);
            }
        }
                
        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if (ed -> ux_dcd_synergy_ed_index != 0UL)
        {
            /* We should wait for the semaphore to wake us up.  */
            status =  _ux_utility_semaphore_get(&transfer_request -> ux_slave_transfer_request_semaphore, UX_WAIT_FOREVER);
    
            /* Check the completion code. */
            if (status != (UINT)UX_SUCCESS)
            {
                return (status);
            }
            
            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (transfer_request -> ux_slave_transfer_request_completion_code != (ULONG)UX_SUCCESS)
            {
                return (UINT)(transfer_request -> ux_slave_transfer_request_completion_code);
            }
        }
    }
    else
    {
        /* We have a request for a OUT transaction from the host.  */
        /* Set the current endpoint fifo.  */
        ux_dcd_synergy_current_endpoint_change(dcd_synergy, ed, 0);
    
        /* Clear the FIFO buffer memory. */
        ux_dcd_synergy_register_write(dcd_synergy, UX_SYNERGY_DCD_CFIFOCTR, UX_SYNERGY_DCD_FIFOCTR_BCLR);

#if !defined(BSP_MCU_GROUP_S124) && !defined(BSP_MCU_GROUP_S128)
        ux_dcd_synergy_register_write(dcd_synergy, UX_SYNERGY_DCD_D0FIFOCTR, UX_SYNERGY_DCD_FIFOCTR_BCLR);
#endif
        /* Enable the BRDY interrupt.  */
        ux_dcd_synergy_buffer_ready_interrupt(dcd_synergy, ed, UX_DCD_SYNERGY_ENABLE);
           
        /* Set PID to BUF. Isolate Control endpoint and others.  */
        if (ed -> ux_dcd_synergy_ed_index == 0UL)
        {
            /* Set PID to BUF for Control Endpoint.  */
            ux_dcd_synergy_register_set(dcd_synergy, UX_SYNERGY_DCD_DCPCTR, UX_SYNERGY_DCD_PIPECTR_PIDBUF);
        }
        else
        {
            /* Set PID to BUF for non Control Endpoints.  */
            ux_dcd_synergy_register_set(dcd_synergy, UX_SYNERGY_DCD_PIPE1CTR + ((ed -> ux_dcd_synergy_ed_index - 1) * 2), UX_SYNERGY_DCD_PIPECTR_PIDBUF);
        }

        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if (ed -> ux_dcd_synergy_ed_index != 0UL)
        {
            /* We should wait for the semaphore to wake us up.  */
            status =  _ux_utility_semaphore_get(&transfer_request -> ux_slave_transfer_request_semaphore, UX_WAIT_FOREVER);
                
            /* Check the completion code. */
            if (status != (UINT)UX_SUCCESS)
            {
                return (status);
            }
            
            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (transfer_request -> ux_slave_transfer_request_completion_code != (ULONG)UX_SUCCESS)
            {
                return (UINT)(transfer_request -> ux_slave_transfer_request_completion_code);
            }

            /* Check the transfer request status. The transfer is completed with error. This may happen when requested
               length is than actual reception bytes(length) from the USB host which is nothing but user buffer overflowed */
            if (transfer_request -> ux_slave_transfer_request_status == (ULONG)UX_TRANSFER_ERROR)
            {
                return (UINT)(transfer_request -> ux_slave_transfer_request_status);
            }
        }
    }

    /* Return to caller with success.  */
    return (UINT)UX_SUCCESS;
}
 /*******************************************************************************************************************//**
  * @} (end addtogroup sf_el_ux)
  **********************************************************************************************************************/
