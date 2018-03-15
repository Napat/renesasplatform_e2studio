/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "fx_api.h"
#include "ux_api.h"
#include "ux_host_class_storage.h"
#include "r_crypto_api.h"
#include "r_trng_api.h"
#include "r_elc.h"
#include "r_elc_api.h"
#include "r_ioport.h"
#include "r_ioport_api.h"
#include "r_fmi.h"
#include "r_fmi_api.h"
#include "r_cgc.h"
#include "r_cgc_api.h"
#ifdef __cplusplus
extern "C"
{
#endif
void fx_common_init0(void);
#include "ux_api.h"

/* USBX Host Stack initialization error callback function. User can override the function if needed. */
void ux_v2_err_callback(void * p_instance, void * p_data);

#if !defined(cb_host_event)
/* User Callback for Host Event Notification (Only valid for USB Host). */
extern UINT cb_host_event(ULONG event, UX_HOST_CLASS * host_class, VOID * instance);
#endif

#ifdef UX_HOST_CLASS_STORAGE_H
/* Utility function to get the pointer to a FileX Media Control Block for a USB Mass Storage device. */
UINT ux_system_host_storage_fx_media_get(UX_HOST_CLASS_STORAGE * instance, UX_HOST_CLASS_STORAGE_MEDIA ** p_storage_media, FX_MEDIA ** p_fx_media);
#endif
void ux_common_init0(void);
#include "ux_api.h"
#include "ux_hcd_synergy.h"
#include "sf_el_ux_hcd_hs_cfg.h"
void g_ux_host_0_err_callback(void * p_instance, void * p_data);
void ux_host_init0(void);
#include "ux_api.h"
#include "ux_host_class_storage.h"

/* Pointer to a USBX Host Mass Storage Class Instance */
extern UX_HOST_CLASS_STORAGE * g_ux_host_class_storage0;
/* A pointer to FileX Media Control Block for a USB flash device */
extern FX_MEDIA * g_fx_media0_ptr;

/* Pointer to a USBX Host Mass Storage Class Media */
extern UX_HOST_CLASS_STORAGE_MEDIA * g_ux_host_class_storage0_media;

/* Macro to dereference a pointer to FileX Media Control Block for a USB flash device */
#define  g_fx_media0 (*(FX_MEDIA*)g_fx_media0_ptr)

/* The function prototype of getting g_fx_media0 for a USB Mass Storage device. */
void fx_media_init_function0(void);

/* FileX media initialization error callback function for USBX Host Mass Storage. User can override the function if needed. */
void g_fx_media0_err_callback_failed_to_get_fx_media(void * p_instance, void * p_data);
extern const crypto_instance_t g_sce;
extern const trng_instance_t g_sce_trng;
/** ELC Instance */
extern const elc_instance_t g_elc;
/** IOPORT Instance */
extern const ioport_instance_t g_ioport;
/** FMI on FMI Instance. */
extern const fmi_instance_t g_fmi;
/** CGC Instance */
extern const cgc_instance_t g_cgc;
void g_common_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* COMMON_DATA_H_ */
