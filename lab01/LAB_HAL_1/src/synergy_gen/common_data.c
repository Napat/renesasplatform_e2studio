/* generated common source file - do not edit */
#include "common_data.h"
crypto_ctrl_t g_sce_ctrl;
crypto_cfg_t g_sce_cfg =
{ .p_sce_long_plg_start_callback = NULL, .p_sce_long_plg_end_callback = NULL, .endian_flag = CRYPTO_WORD_ENDIAN_BIG };
const crypto_instance_t g_sce =
{ .p_ctrl = &g_sce_ctrl, .p_cfg = &g_sce_cfg, .p_api = &g_sce_crypto_api };
trng_ctrl_t g_sce_trng_ctrl;
trng_cfg_t g_sce_trng_cfg =
{ .p_crypto_api = &g_sce_crypto_api, .nattempts = 2 };
const trng_instance_t g_sce_trng =
{ .p_ctrl = &g_sce_trng_ctrl, .p_cfg = &g_sce_trng_cfg, .p_api = &g_trng_on_sce };
/* Instance structure to use this module. */
const fmi_instance_t g_fmi =
{ .p_api = &g_fmi_on_fmi };
const ioport_instance_t g_ioport =
{ .p_api = &g_ioport_on_ioport, .p_cfg = NULL };
const elc_instance_t g_elc =
{ .p_api = &g_elc_on_elc, .p_cfg = NULL };
const cgc_instance_t g_cgc =
{ .p_api = &g_cgc_on_cgc, .p_cfg = NULL };
void g_common_init(void)
{

}
