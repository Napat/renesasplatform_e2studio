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


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_hcd_synergy.h"

/*******************************************************************************************************************//**
 * @addtogroup sf_el_ux
 * @{
 **********************************************************************************************************************/

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    ux_hcd_synergy_request_isochronous_transfer         PORTABLE C      */
/*                                                           5.6          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron, Express Logic Inc.                                   */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*     This function performs an isochronous transfer request.            */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    hcd_synergy                           Pointer to Synergy controller */
/*    transfer_request                      Pointer to transfer request   */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    Completion Status                                                   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    ux_hcd_synergy_frame_number_get          Get frame number           */
/*    ux_hcd_synergy_isochronous_td_obtain     Obtain isochronous TD      */
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Synergy Controller Driver                                           */
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
 * @brief  This function performs an isochronous transfer request.
 *
 * @param[in]      hcd_synergy         : Pointer to a HCD control block
 * @param[in,out]  transfer_request    : Pointer to transfer request
 *
 * @retval UX_SUCCESS           Isochronous transfer request processed successfully.
 * @retval UX_NO_TD_AVAILABLE   Unavailable new TD.
 **********************************************************************************************************************/
UINT  ux_hcd_synergy_request_isochronous_transfer(UX_HCD_SYNERGY *hcd_synergy, UX_TRANSFER *transfer_request)
{
    UX_ENDPOINT           * endpoint;
    UX_SYNERGY_ISO_TD     * data_td;
    UX_SYNERGY_ISO_TD     * start_data_td;
    UX_SYNERGY_ISO_TD     * next_data_td;
    UX_SYNERGY_ISO_TD     * previous_td;
    UX_SYNERGY_ISO_TD     * tail_td;
    UX_SYNERGY_ED         * ed;
    ULONG                   transfer_request_payload_length;
    ULONG                   isoch_packet_payload_length;
    UCHAR                 * data_pointer;
    ULONG                   current_frame_number;
    
    /* Get the pointer to the Endpoint.  */
    endpoint =  (UX_ENDPOINT *) transfer_request -> ux_transfer_request_endpoint;

    /* Now get the physical ED attached to this endpoint.  */
    ed =  endpoint -> ux_endpoint_ed;

    /* If the transfer_request specifies a max packet length other than the endpoint
       size, we force the transfer request value into the endpoint.  */
    if (transfer_request -> ux_transfer_request_packet_length == 0UL)
    {
        transfer_request -> ux_transfer_request_packet_length = (ULONG) endpoint -> ux_endpoint_descriptor.wMaxPacketSize;
    }

    /* Remember the packet length.  */
    isoch_packet_payload_length =  transfer_request -> ux_transfer_request_packet_length;

    /* Use the TD pointer by ed -> tail for the first TD of this transfer and chain from this one on.  */
    data_td =  (UX_SYNERGY_ISO_TD *) ((void *) ed -> ux_synergy_ed_tail_td);
    previous_td =  data_td;

    /* Reset the first obtained data TD in case there is a TD shortage while building the list of tds.  */
    start_data_td =  UX_NULL;

    /* Calculate the frame number to be used to send this payload. If there are no current transfers, 
       we take the current frame number and add a safety value (2-5) to it. If here is pending transactions,
       we use the frame number stored in the transfer request.  */
    if (ed -> ux_synergy_ed_tail_td == ed -> ux_synergy_ed_head_td)
    {
        ux_hcd_synergy_frame_number_get(hcd_synergy, &current_frame_number);
        ed -> ux_synergy_ed_frame =  current_frame_number + UX_SYNERGY_FRAME_DELAY;
    }
    else
    {
        current_frame_number =  ed -> ux_synergy_ed_frame;
    }

    /* Load the start buffer address and Urb length to split the urb in multiple TD transfer.  */
    transfer_request_payload_length =  transfer_request -> ux_transfer_request_requested_length;
    data_pointer =  transfer_request -> ux_transfer_request_data_pointer;
    
    while (transfer_request_payload_length != 0UL)
    {
        /* Set the direction of the TD.  */
        if ((transfer_request -> ux_transfer_request_type & (UINT)UX_REQUEST_DIRECTION) == (UINT)UX_REQUEST_IN)
        {
            data_td -> ux_synergy_iso_td_direction =  UX_SYNERGY_TD_IN;
        }
        else
        {
            data_td -> ux_synergy_iso_td_direction =  UX_SYNERGY_TD_OUT;
        }

        /* Set the frame number.  */
        ed -> ux_synergy_ed_frame =  current_frame_number;

        /* Set the buffer address.  */
        data_td -> ux_synergy_iso_td_buffer =  data_pointer;

        /* Update the length of the transfer for this TD.  */
        data_td -> ux_synergy_iso_td_length =  isoch_packet_payload_length;

        /* Attach the endpoint and transfer request to the TD.  */
        data_td -> ux_synergy_iso_td_transfer_request =  transfer_request;
        data_td -> ux_synergy_iso_td_ed =  ed;

        /* Adjust the data payload length and the data payload pointer.  */
        transfer_request_payload_length -=  isoch_packet_payload_length;
        data_pointer +=  isoch_packet_payload_length;

        /* Prepare the next frame for the next TD in advance.  */
        current_frame_number++;

        /* Check if there will be another transaction.  */
        if (transfer_request_payload_length != 0UL)
        {
            /* Get a new TD to hook this payload.  */
            data_td =  ux_hcd_synergy_isochronous_td_obtain(hcd_synergy);
            if (data_td == UX_NULL)
            {
                /* If there was already a TD chain in progress, free it.  */
                if (start_data_td != UX_NULL)
                {
                    data_td =  start_data_td;
                    while(data_td)
                    {

                        next_data_td =  data_td -> ux_synergy_iso_td_next_td;
                        data_td -> ux_synergy_iso_td_status = (ULONG)UX_UNUSED;
                        data_td =  next_data_td;
                    }
                }

                return (UINT)UX_NO_TD_AVAILABLE;
            }

            /* the first obtained TD in the chain has to be remembered.  */
            if (start_data_td == UX_NULL)
            {
                start_data_td =  data_td;
            }

            /* Attach this new TD to the previous one.  */                                
            previous_td -> ux_synergy_iso_td_next_td =  data_td;
            previous_td =  data_td;
        }
    }
        
    /* Memorize the next frame number for this ED.  */
    ed -> ux_synergy_ed_frame =  current_frame_number;

    /* At this stage, the Head and Tail in the ED are still the same and the synergy controller 
       will skip this ED until we have hooked the new tail TD.  */
    tail_td =  ux_hcd_synergy_isochronous_td_obtain(hcd_synergy);
    if (tail_td == UX_NULL)
    {

        /* If there was already a TD chain in progress, free it.  */
        if (start_data_td != UX_NULL)
        {
            data_td =  start_data_td;
            while(data_td)
            {
                next_data_td =  data_td -> ux_synergy_iso_td_next_td;
                data_td -> ux_synergy_iso_td_status = (ULONG)UX_UNUSED;
                data_td =  next_data_td;
            }
        }

        return (UINT)UX_NO_TD_AVAILABLE;
    }

    /* Attach the tail TD to the last data TD.  */
    data_td -> ux_synergy_iso_td_next_td =  tail_td;

    /* Adjust the ED tail pointer, the controller can now start this transfer
       at the chosen frame number.  */
    ed -> ux_synergy_ed_tail_td =  (UX_SYNERGY_TD *) ((void *) tail_td);

    /* Return successful completion.  */
    return (UINT)UX_SUCCESS;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup sf_el_ux)
 **********************************************************************************************************************/

