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
 * File Name    : sf_crypto_err.h
 * Description  : AMS Crypto error codes header file
 ***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *           AMS Crypto Error codes 1.00    Initial Release.
 **********************************************************************************************************************/

#ifndef SF_CRYPTO_ERR_H_
#define SF_CRYPTO_ERR_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
/* Include driver API and ThreadX API */

#include <stdint.h>

/*******************************************************************************************************************//**
 * @file: sf_crypto_err.h
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup Crypto_Error_Codes
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define SSP_ERR_CRYPTO_START (0x10000)
/** SF crypto module error codes */
typedef enum e_sf_crypto_err
{
    SF_CRYPTO_SUCCESS  =   0,                  /*!< function completed successfully  */
    SF_CRYPTO_CONTINUE = SSP_ERR_CRYPTO_START, /*!< continue executing function */
    SSP_ERR_CRYPTO_SCE_RESOURCE_CONFLICT,      /*!< hardwrae resource busy */
    SSP_ERR_CRYPTO_SCE_FAIL,                   /*!< internal I/O buffer is not empty */
    SSP_ERR_CRYPTO_SCE_HRK_INVALID_INDEX,      /*!< invalid index */
    SSP_ERR_CRYPTO_SCE_RETRY,                  /*!< */
    SSP_ERR_CRYPTO_SCE_VERIFY_FAIL,            /*!< */
    SSP_ERR_CRYPTO_SCE_ALREADY_OPEN,           /*!< */
    SSP_ERR_CRYPTO_NOT_OPEN,                   //!< hardware module is not initialized

    SSP_ERR_CRYPTO_UNKNOWN,                    /*!< some unknown error occured */
    SSP_ERR_CRYPTO_NULL_POINTER,               /*!< null pointer input as a parameter */
    SSP_ERR_CRYPTO_NOT_IMPLEMENTED,            /*!< algorith/size not implemented */

    SSP_ERR_CRYPTO_RNG_INVALID_PARAM,          /*!< an invalid parameter is specified */
    SSP_ERR_CRYPTO_RNG_FATAL_ERROR,            /*!< a fatal error occurred */
    SSP_ERR_CRYPTO_INVALID_SIZE,               /*!< size specified is invalid */
    SSP_ERR_CRYPTO_INVALID_STATE,              /*!< Function used in an valid state */
    SSP_ERR_CRYPTO_ALREADY_OPEN,               /*!< control block is already opened */
    SSP_ERR_CRYPTO_INSTALL_KEY_FAILED,         /*!< Specified input key is invalid. */
    SSP_ERR_CRYPTO_AUTHENTICATION_FAILED,      /*!< Authentication failed */
} sf_crypto_err_t;

/** @} (end ingroup Crypto_Error_Codes) */
#endif /* SF_CRYPTO_ERR_H_ */
