/**
 * @file    adc.h
 * @brief   Analog to Digital Converter(ADC) function prototypes and data types.
 */

/* ****************************************************************************
 * Copyright (C) 2019 Maxim Integrated Products, Inc., All Rights Reserved.
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
#ifndef _ADC_H_
#define _ADC_H_

/* **** Includes **** */
#include <stdint.h>
#include "adc_regs.h"
#include "mcr_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup adc ADC
 * @ingroup periphlibs
 * @{
 */

/***************************************************************************************************************
                                    DATA STRUCTURES FOR ADC INITIALIZATION
***************************************************************************************************************/

/**
  * @brief  Enumeration type for the ADC Input Channels
  *
  */
typedef enum {
    MXC_ADC_CH_0,            ///< Select Channel 0
    MXC_ADC_CH_1,            ///< Select Channel 1
    MXC_ADC_CH_2,            ///< Select Channel 2
    MXC_ADC_CH_3,            ///< Select Channel 3
    MXC_ADC_CH_4,            ///< Select Channel 4
    MXC_ADC_CH_5,            ///< Select Channel 5
    MXC_ADC_CH_6,            ///< Select Channel 6
    MXC_ADC_CH_7,            ///< Select Channel 7
} mxc_adc_chsel_t;


// AI87-TODO: Github comment the below enum out - ?
/**
  * @brief  Enumeration type for the number of samples to average
  *
  */
typedef enum {
    MXC_ADC_AVG_1  = MXC_S_ADC_CTRL1_AVG_AVG1,              ///< Select Channel 0
    MXC_ADC_AVG_2  = MXC_S_ADC_CTRL1_AVG_AVG2,              ///< Select Channel 1
    MXC_ADC_AVG_4  = MXC_S_ADC_CTRL1_AVG_AVG4,              ///< Select Channel 2
    MXC_ADC_AVG_8  = MXC_S_ADC_CTRL1_AVG_AVG8,              ///< Select Channel 3
    MXC_ADC_AVG_16 = MXC_S_ADC_CTRL1_AVG_AVG16,             ///< Select Channel 4
    MXC_ADC_AVG_32 = MXC_S_ADC_CTRL1_AVG_AVG32,             ///< Select Channel 5
} mxc_adc_avg_t;

/**
  * Enumeration type for the ADC Compartors
  */
typedef enum {
    MXC_ADC_COMP_0          = 1,
    MXC_ADC_COMP_1          = 2,
    // MXC_ADC_COMP_HYST_0  = 1 << MXC_F_MCR_AINCOMP_AINCOMPHYST_POS,    //Refer to the V HYST specification in the data sheet electrical characteristics
    // // for the values corresponding to this field value.
    // MXC_ADC_COMP_HYST_1     = 2 << MXC_F_MCR_AINCOMP_AINCOMPHYST_POS,   //Refer to the V HYST specification in the data sheet electrical characteristics
    // for the values corresponding to this field value.
} mxc_adc_comp_t;

/**
 * @brief       Enumeration type for ADC clock divider
 */
typedef enum {
    MXC_ADC_CLKDIV_2,        ///< ADC Scale by 1/2
    MXC_ADC_CLKDIV_4,        ///< ADC Scale by 1/4
    MXC_ADC_CLKDIV_8,        ///< ADC Scale by 1/8 
    MXC_ADC_CLKDIV_16,       ///< ADC Scale by 1/16
    MXC_ADC_CLKDIV_1,        ///< ADC Scale by 1x (no scaling)    
} mxc_adc_clkdiv_t;

/**
 * @brief       Clock settings
 */
typedef enum {
    MXC_ADC_CLK_HCLK,        ///< HCLK CLock                               
    MXC_ADC_CLK_EXT,         ///< External Clock
    MXC_ADC_CLK_IBRO,        ///< IBRO Clock
    MXC_ADC_CLK_ERFO,        ///< ERFO Clock                 
} mxc_adc_clock_t;

/**
 * @brief       Calibration settings 
 */
typedef enum {
    MXC_ADC_SKIP_CAL,           ///< Skip calibration on power-up                              
    MXC_ADC_EN_CAL,             ///< Calibrate on power-up             
} mxc_adc_calibration_t;

/**
 * @brief       trigger mode settings 
 */
typedef enum {
    MXC_ADC_TRIG_SOFTWARE,         ///< Software Trigger                               
    MXC_ADC_TRIG_HARDWARE,         ///< Hardware Trigger          
} mxc_adc_trig_mode_t;

/**
 * @brief       Hardware trigger select options
 */
typedef enum {
    MXC_ADC_TRIG_SEL_TMR0,         ///< Timer 0 Out Rising edge                              
    MXC_ADC_TRIG_SEL_TMR1,         ///< Timer 1 Out Rising Edge
    MXC_ADC_TRIG_SEL_TMR2,         ///< Timer 2 Out Rising Edge
    MXC_ADC_TRIG_SEL_TMR3,         ///< Timer 3 Out Rising Edge
    MXC_ADC_TRIG_SEL_P1_12,        ///< GPIO Port 1, Pin 12
    MXC_ADC_TRIG_SEL_P1_13,        ///< GPIO Port 1, Pin 13
    MXC_ADC_TRIG_SEL_P1_14,        ///< GPIO Port 1, Pin 14
    MXC_ADC_TRIG_SEL_TEMP_SENS,    ///< Temperature Sensor Ready          
} mxc_adc_trig_sel_t;

/**
 * @brief       trigger mode settings 
 */
typedef enum {
    MXC_ADC_ATOMIC_CONV,           ///< Software Trigger                               
    MXC_ADC_CONTINUOUS_CONV,       ///< Hardware Trigger          
} mxc_adc_conversion_mode_t;

/**
 * @brief  Analog input divider select.
 */
typedef enum {
    MXC_ADC_DIV1     = MXC_V_MCR_ADCCFG2_CH0_DIV1,        ///< No input divider
    MXC_ADC_DIV2_5K  = MXC_V_MCR_ADCCFG2_CH0_DIV2_5K,     ///< Divide analog input by two with 5K resistor
    MXC_ADC_DIV2_50K = MXC_V_MCR_ADCCFG2_CH0_DIV2_50K,    ///< Divide analog input by two with 50K resistor
} mxc_adc_divsel_t;

/**
 * @brief  Reference voltage select type.
 */
typedef enum {
    MXC_ADC_REF_EXT,               ///< Use external reference voltage source
    MXC_ADC_REF_INT_1V25,          ///< Use internal 1.25V source
    MXC_ADC_REF_INT_2V048,         ///< Use internal 2.048V souce
} mxc_adc_refsel_t;

///< Callback used when a conversion event is complete
typedef void (*mxc_adc_complete_cb_t) (void *req, int error);

typedef struct  {
    mxc_adc_clock_t clock;          ///< clock to use
    mxc_adc_clkdiv_t clkdiv;        ///< clock divider    
    mxc_adc_calibration_t cal;      ///< skip calibration
    mxc_adc_refsel_t ref;           ///< ADC reference voltage
    uint32_t trackCount;            ///< Sample Clock High time
    uint32_t idleCount;             ///< Sample Clock Low time
} mxc_adc_req_t;

typedef struct  {
    mxc_adc_conversion_mode_t mode; ///< conversion mode
    mxc_adc_trig_mode_t trig;       ///< trigger mode
    mxc_adc_avg_t avg_number;       ///< no of samples to average
    mxc_adc_chsel_t channel;        ///< channel select
    mxc_adc_divsel_t div;           ///< Analog input divider
    mxc_adc_trig_sel_t hwTrig;
} mxc_adc_conversion_req_t;

/**
 * @brief   Performs the ADC startup procedure
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_Init (mxc_adc_req_t *req);

/**
 * @brief   Shuts down the ADC
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_Shutdown (void);

/**
 * @brief   Enable specific ADC interrupts
 *
 * @param   flags mask of interrupt flags to enables
 */
void MXC_ADC_EnableInt (uint32_t flags);

/**
 * @brief   Disable specific ADC interrupts
 *
 * @param   flags mask of interrupt flags to enables
 */
void MXC_ADC_DisableInt (uint32_t flags);

/**
 * @brief   Performs the ADC startup procedure
 *
 * @return  active flags
 */
int MXC_ADC_GetFlags (void);

/**
 * @brief   Performs the ADC startup procedure
 *
 * @param   flags mask of flags to clear
 */
void MXC_ADC_ClearFlags (uint32_t flags);

/**
 * @brief   Perform a conversion on a specific channel
 * @note    The channel must be configured separately
 *
 * @param   req \ref mxc_adc_conversion_req_t
 *
 * @return  Raw conversion value, or \ref MXC_Error_Codes for error.
 */
int MXC_ADC_StartConversion (mxc_adc_conversion_req_t *req);

/**
 * @brief   Perform a conversion on a specific channel
 * @note    The channel must be configured separately
 *          The ADC interrupt must be enabled and MXC_ADC_Handler() called in the ISR
 *          places data in the error parameter of the callback function
 *
 * @param   req \ref mxc_adc_conversion_req_t
 * @param   callback the function to call when the conversion is complete
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_StartConversionAsync (mxc_adc_conversion_req_t *req, mxc_adc_complete_cb_t callback);

/**
 * @brief   Perform a conversion on a specific channel using a DMA transfer
 *
 * @param   req \ref mxc_adc_conversion_req_t
 * @param   pointer to the variable to hold the conversion result
 * @param   callback the function to call when the conversion is complete
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_StartConversionDMA(mxc_adc_conversion_req_t *req, uint16_t* data, void (*callback)(int, int));

/**
 * @brief      Call this function from the ADC ISR when using Async API
 *             functions
 *
 * @return     Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_Handler (void);

/**
 * @brief   Enable comparator hysteresis.
 *
 * @param   comp    Selects the comparator to enable hysteresis for.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_ComparatorHysteresisEn (mxc_adc_comp_t comp);

/**
 * @brief   Disable comparator hysteresis.
 *
 * @param   comp    Selects the comparator to disable hysteresis for.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_ComparatorHysteresisDis (mxc_adc_comp_t comp);

/**
 * @brief   Selects the analog input divider.
 *
 * @param   ch     Channel to set divider for
 * @param   div    The analog input divider to select
 * @param   lpEn   If Divide by 2, non-zero values will enable divide by 2 in low-power modes.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_InputDividerSelect(mxc_adc_chsel_t ch, mxc_adc_divsel_t div, uint8_t lpEn);

/**
 * @brief Selects the desired reference voltage for the ADC.
 *
 * @param ref   Desired reference voltage source.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_ReferenceSelect(mxc_adc_refsel_t ref);

/**
 * @brief  Enables the ADC Dynamic Power-Up Mode.
 *
 * @param  Channel to enable dynamic mode for.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_DynamicModeEn(mxc_adc_chsel_t ch);

/**
 * @brief  Disables the ADC Dynamic Power-Up Mode.
 *
 * @param  Channel to disable dynamic mode for.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_DynamicModeDis(mxc_adc_chsel_t ch);

/**
 * @brief Gets the result from the previous ADC conversion
 *
 * @param      outdata Pointer to store the ADC data conversion result 
 *
 * @return     see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_GetData (uint16_t *outdata);

/**@} end of group adc */

#ifdef __cplusplus
}
#endif

#endif /* _ADC_H_ */
