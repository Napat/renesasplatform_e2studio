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
#include "ux_dcd_synergy.h"

VOID ux_dcd_synergy_fifo_write_software_copy (UX_DCD_SYNERGY *dcd_synergy,
                                            UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            VOID * p_fifo, ULONG  fifo_sel);

VOID ux_dcd_synergy_fifo_write_last_bytes (UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            VOID * p_fifo,
                                            ULONG  usb_base);

VOID ux_dcd_synergy_write_dma_set (UX_DCD_SYNERGY *dcd_synergy,
                                            UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            ULONG fifo_sel);

static VOID ux_dcd_synergy_write_dma_set_16bit (UX_DCD_SYNERGY *dcd_synergy,
                                            UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            ULONG fifo_sel);
#if defined(R_USBHS_BASE)
static VOID ux_dcd_synergy_write_dma_set_32bit (UX_DCD_SYNERGY *dcd_synergy,
                                            UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            ULONG fifo_sel);
#endif

VOID ux_dcd_synergy_fifo_write_dma_start (UX_DCD_SYNERGY * dcd_synergy,
                                            UCHAR * payload_buffer,
                                            VOID * p_fifo);

/*******************************************************************************************************************//**
 * @addtogroup sf_el_ux
 * @{
 **********************************************************************************************************************/

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    ux_dcd_synergy_fifod_write                          PORTABLE C      */
/*                                                           5.6          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron, Express Logic Inc.                                   */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function writes a buffer to FIFOD0 or FIFOD1                   */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_synergy                 Pointer to synergy controller           */ 
/*    ed                                    Register to the ed            */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                                              */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
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
 * @brief  This function writes a buffer to FIFOD0 or FIFOD1.
 *
 * @param[in,out]  dcd_synergy             : Pointer to a DCD control block
 * @param[in,out]  ed                      : Pointer to a physical Endpoint(ED) control block
 *
 * @retval UX_ERROR                         FIFO is not accessible.
 * @retval UX_SYNERGY_DCD_FIFO_WRITE_END    Write ends of FIFO.
 * @retval UX_SYNERGY_DCD_FIFO_WRITE_SHORT  Write short packet.
 * @retval UX_SYNERGY_DCD_FIFO_WRITING      Continue multiple write to FIFO.
 **********************************************************************************************************************/
UINT  ux_dcd_synergy_fifod_write(UX_DCD_SYNERGY *dcd_synergy, UX_DCD_SYNERGY_ED *ed)
{
    ULONG    fifo_access_status;
    ULONG    max_packet_size;
    ULONG    data_buffer_size;
    UINT     status;
    VOID   * fifo_addr;
    ULONG    fifo_ctrl;
    ULONG    fifo_sel;
    ULONG    synergy_register;
    UX_DCD_SYNERGY_PAYLOAD_TRANSFER payload;

    /* Get the Fifo access status.  */
    fifo_access_status =  ux_dcd_synergy_fifo_port_change(dcd_synergy, ed, 0);

    /* Check Status.  */
    if (fifo_access_status == (ULONG)UX_ERROR)
    {
        /* We have an error. Abort.  */
        return (UINT)UX_ERROR;
    }

    /* Get the data buffer size.  */
    data_buffer_size = ux_dcd_synergy_data_buffer_size(dcd_synergy, ed);

    /* Get the max packet size for this endpoint.  */
    max_packet_size = ed -> ux_dcd_synergy_ed_endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;

    /* Check if this transfer takes more than one packet.  */
    if (ed -> ux_dcd_synergy_ed_payload_length <= max_packet_size) 
    {
        /* Set the payload to the TD payload length.  */
        payload.payload_length =  ed -> ux_dcd_synergy_ed_payload_length;

        /* Set Status to write ends.  */
        status = UX_SYNERGY_DCD_FIFO_WRITE_END;

        /* Check for 0 packet.  */
        if ((payload.payload_length == 0UL) || (((UINT)payload.payload_length % (UINT)max_packet_size ) != 0U))
        {
            /* Set Status to write short.  */
            status = (UINT)UX_SYNERGY_DCD_FIFO_WRITE_SHORT;
        }
    }
    else
    {
        /* We are doing a multi write.  */
        status = (UINT)UX_SYNERGY_DCD_FIFO_WRITING;

        /* Payload length is the buffer size.  */
        payload.payload_length = data_buffer_size;
    }      

    /* We need to select the FIFO registers.  */
    if (ed -> ux_dcd_synergy_ed_fifo_index == UX_SYNERGY_DCD_FIFO_D0)
    {
        fifo_addr =  (VOID *) (dcd_synergy->ux_dcd_synergy_base + UX_SYNERGY_DCD_D0FIFO);
        fifo_ctrl =  UX_SYNERGY_DCD_D0FIFOCTR;
        fifo_sel  =  UX_SYNERGY_DCD_D0FIFOSEL;
    }
    else
    {
        fifo_addr =  (VOID *) (dcd_synergy->ux_dcd_synergy_base + UX_SYNERGY_DCD_D1FIFO);
        fifo_ctrl =  UX_SYNERGY_DCD_D1FIFOCTR;
        fifo_sel  =  UX_SYNERGY_DCD_D1FIFOSEL;
    }

    /* Get the payload buffer address.  */
    payload.payload_buffer =  ed -> ux_dcd_synergy_ed_payload_address;

    /* Memorize packet length.  */
    ed -> ux_dcd_synergy_ed_packet_length = payload.payload_length;

    if (dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_tx != NULL)
    {
        /* Setup DMA transfer. */
        ux_dcd_synergy_write_dma_set (dcd_synergy, &payload, fifo_sel);

        if (0U != payload.transfer_times)
        {
            /* Start DMA transfer by software control. */
            ux_dcd_synergy_fifo_write_dma_start (dcd_synergy, (UCHAR *)(payload.payload_buffer), fifo_addr);

            /* Update buffer address and length for rest of bytes. */
            payload.payload_buffer += payload.transfer_times * (INT)payload.transfer_width;
            payload.payload_length -= (ULONG) (payload.transfer_times * (UINT)payload.transfer_width);

            /* Wait till DMA transfer is done. */
            while (true != dcd_synergy->ux_dcd_synergy_dma_done_tx)
            {
                /* Poll the DMA completion flag. */
            }

            /* Clear DMA completion flag. */
            dcd_synergy->ux_dcd_synergy_dma_done_tx = false;
        }

        /* Transfer rest of few bytes into FIFO by software copy. */
        ux_dcd_synergy_fifo_write_last_bytes (&payload, fifo_addr, dcd_synergy -> ux_dcd_synergy_base);
    }
    else
    {
        /* Transfer data by software copy. */
        ux_dcd_synergy_fifo_write_software_copy (dcd_synergy, &payload, fifo_addr, fifo_sel);
    }

    /* Check status. If we have a short packet, we need to set
     * the BVAL flag in the FIFO controller.  */
    if (status != UX_SYNERGY_DCD_FIFO_WRITING)
    {
        /* Read the current FIFO control register.  */
        synergy_register = ux_dcd_synergy_register_read(dcd_synergy, fifo_ctrl);

        /* Check if the BVAL bit is already set.  */
        if ((synergy_register & UX_SYNERGY_DCD_FIFOCTR_BVAL) == 0)
        {
            /* No so set it.  This will enable the Buffer Memory Valid flag.
             * This bit is set to 1 when data has been written to the FIFO and
             * this is a short packet or zero packet or a full packet but not the
             * end of the transmission.  */
            ux_dcd_synergy_register_set(dcd_synergy, fifo_ctrl, UX_SYNERGY_DCD_FIFOCTR_BVAL);
        }
    }

    /* Return status about buffer transfer.  */
    return (status);
}


/******************************************************************************************************************//**
 * @brief  USBX DCD DMA write setup function. Call a subroutine for selected USB controller hardware.
 *
 * @param[in]      dcd_synergy  Pointer to the DCD control block
 * @param[in,out]  p_payload    Pointer to a payload transfer structure
 * @param[in]      fifo_sel     FIFO select register
 **********************************************************************************************************************/
VOID ux_dcd_synergy_write_dma_set (UX_DCD_SYNERGY *dcd_synergy,
                                                   UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                                   ULONG fifo_sel)
{
#if defined(R_USBHS_BASE)
    if (dcd_synergy -> ux_dcd_synergy_base == R_USBHS_BASE)
    {
        /* Set DMA transfer for 32-bit USB hardwares. */
        ux_dcd_synergy_write_dma_set_32bit (dcd_synergy, p_payload, fifo_sel);
    }
    else
#endif
    {
        /* Set DMA transfer for 16-bit USB hardwares. */
        ux_dcd_synergy_write_dma_set_16bit (dcd_synergy, p_payload, fifo_sel);
    }
}

/******************************************************************************************************************//**
 * @brief  USBX DCD DMA write setup function for USB hardwares with 16-bit FIFO
 *
 * @param[in]      dcd_synergy  Pointer to the DCD control block
 * @param[in,out]  p_payload    Pointer to a payload transfer structure
 * @param[in]      fifo_sel     FIFO select register
 **********************************************************************************************************************/
static VOID ux_dcd_synergy_write_dma_set_16bit (UX_DCD_SYNERGY *dcd_synergy,
                                                   UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                                   ULONG fifo_sel)
{
    if(0 == ((UINT)p_payload->payload_buffer % 2U))
    {
        /* Transfer data in 2bytes unit if buffer is aligned to 16-bit address. */
        p_payload -> transfer_width = 2;
        p_payload -> transfer_times = p_payload->payload_length / 2;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_tx.p_info->size   = TRANSFER_SIZE_2_BYTE;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_tx.p_info->length = (uint16_t)p_payload -> transfer_times;

        /* Set FIFO access width to 16 bits. */
        ux_dcd_synergy_register_set(dcd_synergy, fifo_sel, (USHORT)UX_SYNERGY_DCD_DFIFOSEL_MBW_16);
    }
    else
    {
        /* Transfer data in 1byte unit if buffer is aligned to 8-bit address. */
        p_payload -> transfer_width = 1;
        p_payload -> transfer_times = p_payload -> payload_length;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_tx.p_info->size   = TRANSFER_SIZE_1_BYTE;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_tx.p_info->length = (uint16_t) p_payload -> transfer_times;

        /* Set FIFO access width to 8 bits. */
        ux_dcd_synergy_register_set(dcd_synergy, fifo_sel, (USHORT)UX_SYNERGY_DCD_DFIFOSEL_MBW_8);
    }
}

#if defined(R_USBHS_BASE)
/******************************************************************************************************************//**
 * @brief  USBX DCD DMA write setup function for USB hardwares with 32-bit FIFO
 *
 * @param[in]      dcd_synergy  Pointer to the DCD control block
 * @param[in,out]  p_payload    Pointer to a payload transfer structure
 * @param[in]      fifo_sel     FIFO select register
 **********************************************************************************************************************/
static VOID ux_dcd_synergy_write_dma_set_32bit (UX_DCD_SYNERGY * dcd_synergy,
                                                    UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                                    ULONG fifo_sel)
{
    if(0 == ((UINT)p_payload->payload_buffer % 4U))
    {
        /* Transfer data in 4bytes unit if buffer is aligned to 32-bit address. */
        p_payload -> transfer_width = 4;
        p_payload -> transfer_times = p_payload->payload_length / 4;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_tx.p_info->size   = TRANSFER_SIZE_4_BYTE;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_tx.p_info->length = (uint16_t)p_payload -> transfer_times;

        /* Set FIFO access width to 32 bits. */
        ux_dcd_synergy_register_set(dcd_synergy, fifo_sel, (USHORT)UX_SYNERGY_DCD_DFIFOSEL_MBW_32);
    }
    else
    {
        ux_dcd_synergy_write_dma_set_16bit (dcd_synergy, p_payload, fifo_sel);
    }
}
#endif

/******************************************************************************************************************//**
 * @brief  USBX DCD DMA FIFO write - DMA start function
 *
 * @param[in]      dcd_synergy      Pointer to the DCD control block
 * @param[in,out]  p_payload_buffer Pointer to a payload buffer
 * @param[in]      p_fifo           FIFO register address
 **********************************************************************************************************************/
VOID ux_dcd_synergy_fifo_write_dma_start (UX_DCD_SYNERGY * dcd_synergy,
                                                    UCHAR * p_payload_buffer,
                                                    VOID  * p_fifo)
{
#if defined(R_USBHS_BASE)
    if (dcd_synergy -> ux_dcd_synergy_base == R_USBHS_BASE)
    {
        if (TRANSFER_SIZE_4_BYTE == dcd_synergy->ux_dcd_synergy_transfer_cfg_tx.p_info->size)
        {
            /* 32-bit FIFO access does not need address offset. */
        }
        else if (TRANSFER_SIZE_2_BYTE == dcd_synergy->ux_dcd_synergy_transfer_cfg_tx.p_info->size)
        {
            /* USBHS controller needs address offset (+2) for 16-bit access to the FIFO. */
            p_fifo =  (VOID *)((ULONG)p_fifo + 2UL);
        }
        else
        {
            /* USBHS controller needs address offset (+3) for 8-bit access to the FIFO. */
            p_fifo =  (VOID *)((ULONG)p_fifo + 3UL);
        }
    }
#endif

    /* Setup DMA block transfer. */
    dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_tx->p_api->blockReset(
            dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_tx->p_ctrl,
            p_payload_buffer,
            p_fifo,
            dcd_synergy->ux_dcd_synergy_transfer_cfg_tx.p_info->length,
            dcd_synergy->ux_dcd_synergy_transfer_cfg_tx.p_info->size,
            1);

    /* Trigger DMA by software control. */
    dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_tx->p_api->start(
            dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_tx->p_ctrl,
            TRANSFER_START_MODE_SINGLE);
}
/*******************************************************************************************************************//**
 * @} (end addtogroup sf_el_ux)
 **********************************************************************************************************************/
