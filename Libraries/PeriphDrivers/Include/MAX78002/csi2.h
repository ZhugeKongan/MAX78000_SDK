/**
 * @file    csi2.h
 * @brief   Camera Serial Interface 2 (CSI-2) function prototypes and data types.
 */

/* ****************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 * $Date: 2018-08-28 17:03:02 -0500 (Tue, 28 Aug 2018) $
 * $Revision: 37424 $
 *
 *************************************************************************** */

/* Define to prevent redundant inclusion */
#ifndef _CSI2_H_
#define _CSI2_H_

/* **** Includes **** */
#include "csi2_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup csi2 Camera Serial Interface 2 (CSI-2)
 * @ingroup periphlibs
 * @{
 */

/* **** Definitions **** */

/**
 * @brief      Type alias for a GPIO callback function with prototype:
 * @code
    void callback_fn(void *cbdata);
 * @endcode
 * @param      cbdata  A void pointer to the data type as registered when
 *                     MXC_GPIO_RegisterCallback() was called.
 */
typedef void (*mxc_csi_callback_fn) (void *cbdata);

/**
 * @brief   Enumeration type for the CSI-2
 */
typedef enum {

} mxc_csi2_func_t;

/* **** Function Prototypes **** */

/**
 * @brief      Initialize CSI.
 * @param      portMask     Mask for the port to be initialized
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_Init ();


// CCI functions will have to be in a separate file within the Board files location.
int MXC_CSI2_CCI_Init ();

int MXC_CSI2_CCI_Start();

int MXC_CSI2_CCI_Stop();

/**
 * @brief      Shutdown GPIO.
 * @param      portMask     Mask for the port to be initialized
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_Shutdown (uint32_t portMask);

/**
 * @brief      Reset GPIO.
 * @param      portMask     Mask for the port to be initialized
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_Reset (uint32_t portMask);

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_Config (const mxc_csi2_cfg_t *cfg);

int MXC_CSI2_Config (const mxc_csi2_cfg_t *cfg);

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_EnableLanes(int lanes);

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_DisableLanes(int lanes);

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_SetVirtualChannel(int VC);

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_GetVirtualChannel();

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_SetDatatype();

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_GetDatatype();

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_VFIFO_EnableInt();

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_VFIFO_DisableInt();

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_CTR_EnableInt();

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_CTR_DisableInt();

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_DPHY_EnableInt(uint32_t flags);

/**
 * @brief      Configure GPIO pin(s).
 * @param      flags    Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_DPHY_DisableInt(uint32_t flags);

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_VFIFO_Config (const mxc_csi2_vfifo_cfg_t *cfg);

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_ULPS_Config()

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_PHY_Config()

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_VFIFO_Start();

/**
 * @brief      Configure GPIO pin(s).q
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSI2_VFIFO_Stop();

/**
 * @brief      Configure GPIO pin(s).
 * @param      cfg   Pointer to configuration structure describing the pin.
 * @return     #E_NO_ERROR if everything is successful.
 */
int MXC_CSIS_VFIFO_Status();




/**@} end of group csi2 */

#ifdef __cplusplus
}
#endif

#endif /* _CSI2_H_ */
