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

static VOID ux_dcd_synergy_fifo_read_software_copy (UX_DCD_SYNERGY *dcd_synergy,
                                            UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            VOID * p_fifo,
                                            ULONG  fifo_sel);

static VOID ux_dcd_synergy_fifo_read_software_copy_16bit (UX_DCD_SYNERGY *dcd_synergy,
                                            UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            VOID * p_fifo,
                                            ULONG  fifo_sel);

#if defined(R_USBHS_BASE)
static VOID ux_dcd_synergy_fifo_read_software_copy_32bit (UX_DCD_SYNERGY *dcd_synergy,
                                            UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            VOID * p_fifo,
                                            ULONG  fifo_sel);
#endif

static VOID ux_dcd_synergy_fifo_read_last_bytes (UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            VOID * p_fifo);

static VOID ux_dcd_synergy_read_dma_set (UX_DCD_SYNERGY *dcd_synergy,
                                            UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            ULONG fifo_sel);

static VOID ux_dcd_synergy_read_dma_set_16bit (UX_DCD_SYNERGY *dcd_synergy,
                                            UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            ULONG fifo_sel);
#if defined(R_USBHS_BASE)
static VOID ux_dcd_synergy_read_dma_set_32bit (UX_DCD_SYNERGY *dcd_synergy,
                                            UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                            ULONG fifo_sel);
#endif

static VOID ux_dcd_synergy_fifo_read_dma_start (UX_DCD_SYNERGY * dcd_synergy,
                                            UCHAR * payload_buffer,
                                            VOID  * p_fifo);

typedef union local_buffer_u {
    ULONG   input;
    UCHAR   output[4];
} local_buffer_t;

/*******************************************************************************************************************//**
 * @addtogroup sf_el_ux
 * @{
 **********************************************************************************************************************/

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    ux_dcd_synergy_fifo_read                          PORTABLE C        */
/*                                                           5.6          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron, Express Logic Inc.                                   */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function reads a buffer from FIFO C D0 or D1                   */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_synergy                 Pointer to synergy controller           */ 
/*    ed                                 Register to the ed               */ 
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
 * @brief  This function reads from the hardware FIFO C, D0 or D1 and stores in the destination buffer.
 *
 * @param[in,out]  dcd_synergy            : Pointer to a DCD control block
 * @param[in,out]  ed                     : Pointer to a physical Endpoint(ED) control block
 *
 * @retval UX_ERROR                        FIFO is not accessible.
 * @retval UX_SYNERGY_DCD_FIFO_READ_OVER   FIFO read overflow.
 * @retval UX_SYNERGY_DCD_FIFO_READ_SHORT  Short packet is received.
 * @retval UX_SYNERGY_DCD_FIFO_READING     Continue reading FIFO.
 **********************************************************************************************************************/
UINT  ux_dcd_synergy_fifo_read(UX_DCD_SYNERGY *dcd_synergy, UX_DCD_SYNERGY_ED *ed)
{
    ULONG   fifo_access_status;
    ULONG   max_packet_size;
    UINT    status;
    ULONG   fifo_sel;
    VOID  * fifo_addr;
    ULONG   fifo_ctrl;
    UX_DCD_SYNERGY_PAYLOAD_TRANSFER payload;

    /* We need to select the FIFO registers.  */
    switch (ed -> ux_dcd_synergy_ed_fifo_index)
    {
        case  UX_SYNERGY_DCD_FIFO_C      :

            /* Set fifo_sel and fifo_addr fields to FIFO_C */
            fifo_sel  =  UX_SYNERGY_DCD_CFIFOSEL;
            fifo_addr =  (VOID *)(dcd_synergy->ux_dcd_synergy_base + UX_SYNERGY_DCD_CFIFO);
            fifo_ctrl = UX_SYNERGY_DCD_CFIFOCTR;
            break;

        case  UX_SYNERGY_DCD_FIFO_D0     :

            /* Set fifo_sel and fifo_addr fields to FIFO_D0 */
            fifo_sel  =  UX_SYNERGY_DCD_D0FIFOSEL;
            fifo_addr =  (VOID *)(dcd_synergy->ux_dcd_synergy_base + UX_SYNERGY_DCD_D0FIFO);
            fifo_ctrl =  UX_SYNERGY_DCD_D0FIFOCTR;
            break; 

        case  UX_SYNERGY_DCD_FIFO_D1     :

            /* Set fifo_sel and fifo_addr fields to FIFO_D1 */
            fifo_sel  =  UX_SYNERGY_DCD_D1FIFOSEL;
            fifo_addr = (VOID *)(dcd_synergy->ux_dcd_synergy_base + UX_SYNERGY_DCD_D1FIFO);
            fifo_ctrl =  UX_SYNERGY_DCD_D1FIFOCTR;
            break;

        default :
            /* Set fifo_sel and fifo_addr fields to FIFO_C */
            fifo_sel  =  UX_SYNERGY_DCD_CFIFOSEL;
            fifo_addr =  (VOID *)(dcd_synergy->ux_dcd_synergy_base + UX_SYNERGY_DCD_CFIFO);
            fifo_ctrl = UX_SYNERGY_DCD_CFIFOCTR;
            break;
    }

    /* Get the Fifo access status for the endpoint.  */
    fifo_access_status =  ux_dcd_synergy_fifo_port_change(dcd_synergy, ed, 0);

    /* Check Status.  */
    if (fifo_access_status == (ULONG)UX_ERROR)
    {
        /* We have an error. Abort.  */
        return (UINT)UX_ERROR;
    }

    /* Get the max packet size for this endpoint.  */
    max_packet_size = ed -> ux_dcd_synergy_ed_endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;

    /* Isolate the payload length.  */
    payload.payload_length = fifo_access_status & UX_SYNERGY_DCD_FIFOCTR_DTLN;

    /* Save the payload length in the ED. This is needed to check for end of transfer.  */
    ed -> ux_dcd_synergy_ed_actual_length =  payload.payload_length;

    /* Set NAK.  */
    ux_dcd_synergy_endpoint_nak_set(dcd_synergy, ed);

    /* Check for overwrite.  */
    if (ed -> ux_dcd_synergy_ed_payload_length < payload.payload_length)
    {
        /* Set Status to read overflow.  */
        status = UX_SYNERGY_DCD_FIFO_READ_OVER;

        /* Set the payload length to the size wanted by the caller.  */
        payload.payload_length = ed -> ux_dcd_synergy_ed_payload_length;
    }
    else
    {
        /* Check for short packet.  */
        if ((payload.payload_length == 0UL) || ((UINT)(payload.payload_length % max_packet_size) != 0U))
        {
            /* We have a short packet.  */
            status = UX_SYNERGY_DCD_FIFO_READ_SHORT;
        }
        else
        {
            /* Continue reading.  */
            status = UX_SYNERGY_DCD_FIFO_READING;
        }
    }

    /* Check if received packet is 0 length packet.  */
    if (0UL != payload.payload_length)
    {
        /* Get the payload buffer address.  */
        payload.payload_buffer =  ed->ux_dcd_synergy_ed_payload_address;

        /* Check if Transfer instance is available. */
#if defined(BSP_MCU_GROUP_S124) || defined(BSP_MCU_GROUP_S128)
        if (dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_rx)
#else
        /* Also check if D0 or D1 FIFO is selected. */
        if ((dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_rx != NULL) &&
                ((ed->ux_dcd_synergy_ed_fifo_index == UX_SYNERGY_DCD_FIFO_D0)     ||
                 (ed->ux_dcd_synergy_ed_fifo_index == UX_SYNERGY_DCD_FIFO_D1)))
#endif
        {
            /* Setup DMA transfer. */
            ux_dcd_synergy_read_dma_set (dcd_synergy, &payload, fifo_sel);

            if (0 != payload.transfer_times)
            {
                /* Start DMA transfer by software control. */
                ux_dcd_synergy_fifo_read_dma_start (dcd_synergy, (UCHAR *)(payload.payload_buffer), fifo_addr);

                /* Update buffer address and length for rest of bytes. */
                payload.payload_buffer += (UINT) (payload.transfer_times * (UINT)payload.transfer_width);;
                payload.payload_length -= (ULONG) (payload.transfer_times * (UINT)payload.transfer_width);

                /* Wait till DMA transfer is done. */
                while (1 != dcd_synergy->ux_dcd_synergy_dma_done_rx)
                {
                    /* Poll the DMA completion flag. */
                }

                /* Clear DMA completion flag. */
                dcd_synergy->ux_dcd_synergy_dma_done_rx = 0;
            }

            /* Transfer rest of few bytes in FIFO by software copy. */
            ux_dcd_synergy_fifo_read_last_bytes (&payload, fifo_addr);
        }
        else
        {
            /* Transfer data by software copy. */
            ux_dcd_synergy_fifo_read_software_copy (dcd_synergy, &payload, fifo_addr, fifo_sel);
        }
    }

    /* Clear the FIFO buffer memory. */
    ux_dcd_synergy_register_write(dcd_synergy, fifo_ctrl, UX_SYNERGY_DCD_FIFOCTR_BCLR);

    /* Return status about buffer transfer.  */
    return (status);
}


/*******************************************************************************************************************//**
 * @brief  USBX DCD DMA read setup function. Call a subroutine for selected USB controller hardware.
 *
 * @param[in]      dcd_synergy  Pointer to the DCD control block
 * @param[in,out]  p_payload    Pointer to a payload transfer structure
 * @param[in]      fifo_sel     FIFO select register
 **********************************************************************************************************************/
static VOID ux_dcd_synergy_read_dma_set (UX_DCD_SYNERGY *dcd_synergy,
                                                   UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                                   ULONG fifo_sel)
{
#if defined(R_USBHS_BASE)
    if (dcd_synergy -> ux_dcd_synergy_base == R_USBHS_BASE)
    {
        /* Set DMA transfer for 32-bit USB hardwares. */
        ux_dcd_synergy_read_dma_set_32bit (dcd_synergy, p_payload, fifo_sel);
    }
    else
#endif
    {
        /* Set DMA transfer for 16-bit USB hardwares. */
        ux_dcd_synergy_read_dma_set_16bit (dcd_synergy, p_payload, fifo_sel);
    }
}

/*******************************************************************************************************************//**
 * @brief  USBX DCD DMA read setup function for USB hardwares with 16-bit FIFO
 *
 * @param[in]      dcd_synergy  Pointer to the DCD control block
 * @param[in,out]  p_payload    Pointer to a payload transfer structure
 * @param[in]      fifo_sel     FIFO select register
 **********************************************************************************************************************/
static VOID ux_dcd_synergy_read_dma_set_16bit (UX_DCD_SYNERGY *dcd_synergy,
                                                   UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                                   ULONG fifo_sel)
{
    if(0 == ((UINT)p_payload->payload_buffer % 2U))
    {
        /* Transfer data in 2bytes unit if buffer is aligned to 16-bit address. */
        p_payload -> transfer_width = 2;
        p_payload -> transfer_times = p_payload->payload_length / 2;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_rx.p_info->size   = TRANSFER_SIZE_2_BYTE;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_rx.p_info->length = (uint16_t)p_payload -> transfer_times;

        /* Set FIFO access width to 16 bits. */
        ux_dcd_synergy_register_set(dcd_synergy, fifo_sel, (USHORT)UX_SYNERGY_DCD_DFIFOSEL_MBW_16);
    }
    else
    {
        /* Transfer data in 1byte unit if buffer is aligned to 8-bit address. */
        p_payload -> transfer_width = 1;
        p_payload -> transfer_times = p_payload -> payload_length;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_rx.p_info->size   = TRANSFER_SIZE_1_BYTE;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_rx.p_info->length = (uint16_t) p_payload -> transfer_times;

        /* Set FIFO access width to 8 bits. */
        ux_dcd_synergy_register_set(dcd_synergy, fifo_sel, (USHORT)UX_SYNERGY_DCD_DFIFOSEL_MBW_8);
    }
}

#if defined(R_USBHS_BASE)
/******************************************************************************************************************//**
 * @brief  USBX DCD DMA read setup function for USB hardwares with 32-bit FIFO
 *
 * @param[in]      dcd_synergy  Pointer to the DCD control block
 * @param[in,out]  p_payload    Pointer to a payload transfer structure
 * @param[in]      fifo_sel     FIFO select register
 **********************************************************************************************************************/
static VOID ux_dcd_synergy_read_dma_set_32bit (UX_DCD_SYNERGY * dcd_synergy,
                                                    UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                                    ULONG fifo_sel)
{
    if(0 == ((UINT)p_payload->payload_buffer % 4U))
    {
        /* Transfer data in 4bytes unit if buffer is aligned to 32-bit address. */
        p_payload -> transfer_width = 4;
        p_payload -> transfer_times = p_payload->payload_length / 4;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_rx.p_info->size   = TRANSFER_SIZE_4_BYTE;
        dcd_synergy->ux_dcd_synergy_transfer_cfg_rx.p_info->length = (uint16_t)p_payload -> transfer_times;

        /* Set FIFO access width to 32 bits. */
        ux_dcd_synergy_register_set(dcd_synergy, fifo_sel, (USHORT)UX_SYNERGY_DCD_DFIFOSEL_MBW_32);
    }
    else
    {
        ux_dcd_synergy_read_dma_set_16bit (dcd_synergy, p_payload, fifo_sel);
    }
}
#endif

/******************************************************************************************************************//**
 * @brief  USBX DCD FIFO read - DMA start function
 *
 * @param[in]      dcd_synergy      Pointer to the DCD control block
 * @param[in,out]  p_payload_buffer Pointer to a payload buffer
 * @param[in]      p_fifo           FIFO register address
 **********************************************************************************************************************/
static VOID ux_dcd_synergy_fifo_read_dma_start (UX_DCD_SYNERGY * dcd_synergy,
                                                    UCHAR * p_payload_buffer,
                                                    VOID  * p_fifo)
{
#if defined(R_USBHS_BASE)
    if (dcd_synergy -> ux_dcd_synergy_base == R_USBHS_BASE)
    {
        if (TRANSFER_SIZE_4_BYTE == dcd_synergy->ux_dcd_synergy_transfer_cfg_rx.p_info->size)
        {
            /* 32-bit FIFO access does not need address offset. */
        }
        else if (TRANSFER_SIZE_2_BYTE == dcd_synergy->ux_dcd_synergy_transfer_cfg_rx.p_info->size)
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
    dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_rx->p_api->blockReset(
            dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_rx->p_ctrl,
            p_fifo,
            p_payload_buffer,
            dcd_synergy->ux_dcd_synergy_transfer_cfg_rx.p_info->length,
            dcd_synergy->ux_dcd_synergy_transfer_cfg_rx.p_info->size,
            1);

    /* Trigger DMA by software control. */
    dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_rx->p_api->start(
            dcd_synergy->ux_dcd_synergy_transfer.ux_synergy_transfer_rx->p_ctrl,
            TRANSFER_START_MODE_SINGLE);
}

/******************************************************************************************************************//**
 * @brief  USBX DCD FIFO read by software copy.  Call a subroutine for selected USB controller hardware.
 *
 * @param[in]      dcd_synergy      Pointer to the DCD control block
 * @param[in,out]  p_payload        Pointer to a payload transfer structure
 * @param[in]      p_fifo           FIFO register address
 * @param[in]      fifo_sel         FIFO select register
 **********************************************************************************************************************/
static VOID ux_dcd_synergy_fifo_read_software_copy (UX_DCD_SYNERGY *dcd_synergy,
                                                    UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                                    VOID * p_fifo,
                                                    ULONG  fifo_sel)
{
#if defined(R_USBHS_BASE)
    if (dcd_synergy -> ux_dcd_synergy_base == R_USBHS_BASE)
    {
        /* Transfer data by software copy. */
        ux_dcd_synergy_fifo_read_software_copy_32bit (dcd_synergy, p_payload, p_fifo, fifo_sel);
    }
    else
#endif
    {
        /* Transfer data by software copy. */
        ux_dcd_synergy_fifo_read_software_copy_16bit (dcd_synergy, p_payload, p_fifo, fifo_sel);
    }
}

#if defined(R_USBHS_BASE)
/******************************************************************************************************************//**
 * @brief  USBX DCD FIFO read - Software copy for USB hardwares with 32-bit FIFO
 *
 * @param[in]      dcd_synergy      Pointer to the DCD control block
 * @param[in,out]  p_payload        Pointer to a payload transfer structure
 * @param[in]      p_fifo           FIFO register address
 * @param[in]      fifo_sel         FIFO select register
 **********************************************************************************************************************/
static VOID ux_dcd_synergy_fifo_read_software_copy_32bit (UX_DCD_SYNERGY *dcd_synergy,
                                                    UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                                    VOID * p_fifo,
                                                    ULONG  fifo_sel)
{
    if (dcd_synergy -> ux_dcd_synergy_base == R_USBHS_BASE)
    {
        if(0 == ((UINT)p_payload -> payload_buffer % 4U))
        {
            /* Calculate FIFO access times. */
            p_payload -> transfer_times = p_payload -> payload_length / 4;
            p_payload -> transfer_width = 4;
            p_payload -> payload_length = p_payload -> payload_length - (p_payload -> transfer_times * 4);

            /* Set the width to 32-bit. */
            ux_dcd_synergy_register_set(dcd_synergy, fifo_sel, (USHORT)UX_SYNERGY_DCD_DFIFOSEL_MBW_32);

            while (0 != p_payload -> transfer_times)
            {
                /* Read 32-bit value from FIFO. */
                *(ULONG *)(p_payload -> payload_buffer) =  *(volatile ULONG *) p_fifo;

                /* Update the payload buffer address. */
                p_payload -> payload_buffer = p_payload -> payload_buffer + (INT)sizeof(ULONG);

                --p_payload -> transfer_times;
            }

            ux_dcd_synergy_fifo_read_last_bytes (p_payload, p_fifo);
        }
        else
        {
            ux_dcd_synergy_fifo_read_software_copy_16bit (dcd_synergy, p_payload, p_fifo, fifo_sel);
        }
    }
}
#endif

/******************************************************************************************************************//**
 * @brief  USBX DCD FIFO read - Software copy for USB hardwares with 16-bit FIFO
 *
 * @param[in]      dcd_synergy      Pointer to the DCD control block
 * @param[in,out]  p_payload        Pointer to a payload transfer structure
 * @param[in]      p_fifo           FIFO register address
 * @param[in]      fifo_sel         FIFO select register
 **********************************************************************************************************************/
static VOID ux_dcd_synergy_fifo_read_software_copy_16bit (UX_DCD_SYNERGY *dcd_synergy,
                                                    UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                                    VOID * p_fifo,
                                                    ULONG  fifo_sel)
{
    if(0 == ((UINT)p_payload -> payload_buffer % 2U))
    {
        /* Calculate FIFO access times. */
        p_payload -> transfer_times = p_payload -> payload_length / 2;
        p_payload -> transfer_width = 2;
        p_payload -> payload_length = p_payload -> payload_length - (p_payload -> transfer_times * 2);

        /* Set the width to 16-bit. */
        ux_dcd_synergy_register_set(dcd_synergy, fifo_sel, (USHORT)UX_SYNERGY_DCD_DFIFOSEL_MBW_16);

#if defined(R_USBHS_BASE)
        /* USBHS controller needs address offset (+2) for 16-bit access to the FIFO. */
        if (dcd_synergy -> ux_dcd_synergy_base == R_USBHS_BASE)
        {
            p_fifo =  (VOID *)((ULONG)p_fifo + 2UL);
        }
#endif

        while (0 != p_payload -> transfer_times)
        {
            /* Read 16-bit value from FIFO. */
            *(USHORT *)(p_payload -> payload_buffer) =  *(volatile USHORT *) p_fifo;

            /* Update the payload buffer address. */
            p_payload -> payload_buffer = p_payload -> payload_buffer + (INT)sizeof(USHORT);

            --p_payload -> transfer_times;
        }

        ux_dcd_synergy_fifo_read_last_bytes (p_payload, p_fifo);
    }
    else
    {
        ux_dcd_synergy_register_set(dcd_synergy, fifo_sel, (USHORT)UX_SYNERGY_DCD_DFIFOSEL_MBW_8);

#if defined(R_USBHS_BASE)
        /* USBHS controller needs address offset (+3) for 8-bit access to the FIFO. */
        if (dcd_synergy -> ux_dcd_synergy_base == R_USBHS_BASE)
        {
            p_fifo =  (VOID *)((ULONG)p_fifo + 3UL);
        }
#endif

        while (0 != p_payload -> payload_length)
        {
            /* Read 8-bit value from FIFO. */
            *(UCHAR *)(p_payload -> payload_buffer) = *(volatile UCHAR *) p_fifo;

            /* Update the payload buffer address  */
            p_payload -> payload_buffer = p_payload -> payload_buffer + (INT)sizeof(UCHAR);

            /* And the remaining length  */
            p_payload -> payload_length = p_payload -> payload_length - (INT)sizeof(UCHAR);
        }
    }
}

/******************************************************************************************************************//**
 * @brief  USBX DCD FIFO read - Copy last bytes from FIFO by software if the rest bytes are less than FIFO access width
 *
 * @param[in,out]  p_payload        Pointer to a payload transfer structure
 * @param[in]      p_fifo           FIFO register address
 **********************************************************************************************************************/
static VOID ux_dcd_synergy_fifo_read_last_bytes (UX_DCD_SYNERGY_PAYLOAD_TRANSFER * p_payload,
                                                     VOID * p_fifo)
{
    local_buffer_t  local_buffer;
    INT             byte_count = 0;

    if (0 != p_payload -> payload_length)
    {
        /* Send remaining bytes by software copy. */
        if (4 == p_payload -> transfer_width)
        {
            /* Read data at once from FIFO. */
            local_buffer.input = *((volatile ULONG *)p_fifo);

            while ((INT)p_payload -> payload_length > byte_count)
            {
                /* Copy data to the payload buffer 1 byte each. */
                *(UCHAR *)(p_payload -> payload_buffer) = local_buffer.output[byte_count++];

                /* Update the payload buffer address  */
                p_payload -> payload_buffer = p_payload -> payload_buffer + (INT)sizeof(UCHAR);
            }
        }
        else
        {
            /* Read data at once from FIFO. */
            local_buffer.input = *(volatile USHORT *)p_fifo;

            while ((INT)p_payload -> payload_length > byte_count)
            {
                /* Copy data to the payload buffer 1 byte each. */
                *(UCHAR *)(p_payload -> payload_buffer) = local_buffer.output[byte_count++];

                /* Update the payload buffer address  */
                p_payload -> payload_buffer = p_payload -> payload_buffer + (INT)sizeof(UCHAR);
            }
        }
    }
}
/*******************************************************************************************************************//**
 * @} (end addtogroup sf_el_ux)
 **********************************************************************************************************************/
