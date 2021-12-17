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
 *************************************************************************** */

#include "adc.h"
#include "adc_regs.h"
#include "adc_revb.h"
#include "gcr_regs.h"
#include "mxc_device.h"
#include "mxc_errors.h"
#include "mxc_assert.h"
#include "mxc_sys.h"
#include "mcr_regs.h"
#include "mxc_lock.h"
#include "mxc_pins.h"
#include "pwrseq_regs.h"
#include <stdio.h>

#define MXC_F_MCR_ADC_CFG2_CH     0x3

static void initGPIOForChannel(mxc_adc_chsel_t channel)
{
    switch (channel) {
    case MXC_ADC_CH_0:
        MXC_GPIO_Config(&gpio_cfg_adc_ain0);
        break;
        
    case MXC_ADC_CH_1:
        MXC_GPIO_Config(&gpio_cfg_adc_ain1);
        break;
        
    case MXC_ADC_CH_2:
        MXC_GPIO_Config(&gpio_cfg_adc_ain2);
        break;
        
    case MXC_ADC_CH_3:
        MXC_GPIO_Config(&gpio_cfg_adc_ain3);
        break;
        
    case MXC_ADC_CH_4:
        MXC_GPIO_Config(&gpio_cfg_adc_ain4);
        break;
        
    case MXC_ADC_CH_5:
        MXC_GPIO_Config(&gpio_cfg_adc_ain5);
        break;
        
    case MXC_ADC_CH_6:
        MXC_GPIO_Config(&gpio_cfg_adc_ain6);
        break;
        
    case MXC_ADC_CH_7:
        MXC_GPIO_Config(&gpio_cfg_adc_ain7);
        break;

    case MXC_ADC_CH_8:
        MXC_GPIO_Config(&gpio_cfg_adc_ain8);
        break;

    case MXC_ADC_CH_9:
        MXC_GPIO_Config(&gpio_cfg_adc_ain9);
        break;

    case MXC_ADC_CH_10:
        MXC_GPIO_Config(&gpio_cfg_adc_ain10);
        break;

    case MXC_ADC_CH_11:
        MXC_GPIO_Config(&gpio_cfg_adc_ain11);
        break;
        
    default:
        break;
    }
}

static void initGPIOforHWTrig(mxc_adc_trig_sel_t hwTrig) {
    mxc_gpio_cfg_t gpioCfg;
    switch(hwTrig) {
        case MXC_ADC_TRIG_SEL_TMR0:
        case MXC_ADC_TRIG_SEL_TMR1:
        case MXC_ADC_TRIG_SEL_TMR2:
        case MXC_ADC_TRIG_SEL_TMR3:
        case MXC_ADC_TRIG_SEL_TEMP_SENS:
            break;
        case MXC_ADC_TRIG_SEL_P0_29:
            gpioCfg.port = MXC_GPIO0;
            gpioCfg.mask = MXC_GPIO_PIN_29;
            gpioCfg.func = MXC_GPIO_FUNC_ALT4;
            gpioCfg.pad = MXC_GPIO_PAD_NONE;
            gpioCfg.vssel = MXC_GPIO_VSSEL_VDDIO;
            MXC_GPIO_Config(&gpioCfg);
            break;
        case MXC_ADC_TRIG_SEL_P0_22:
            gpioCfg.port = MXC_GPIO0;
            gpioCfg.mask = MXC_GPIO_PIN_22;
            gpioCfg.func = MXC_GPIO_FUNC_ALT2;
            gpioCfg.pad = MXC_GPIO_PAD_NONE;
            gpioCfg.vssel = MXC_GPIO_VSSEL_VDDIO;
            MXC_GPIO_Config(&gpioCfg);
            break;
        case MXC_ADC_TRIG_SEL_P1_4:
            gpioCfg.port = MXC_GPIO1;
            gpioCfg.mask = MXC_GPIO_PIN_4;
            gpioCfg.func = MXC_GPIO_FUNC_ALT4;
            gpioCfg.pad = MXC_GPIO_PAD_NONE;
            gpioCfg.vssel = MXC_GPIO_VSSEL_VDDIO;
            MXC_GPIO_Config(&gpioCfg);
            break;
    }
}

int MXC_ADC_Init(mxc_adc_req_t *req)
{
    switch(req->clock) {
        case MXC_ADC_CLK_HCLK:
            break;
        case MXC_ADC_CLK_EXT:
            MXC_GPIO_Config(&gpio_cfg_hfextclk);
            MXC_SYS_ClockSourceEnable(MXC_SYS_CLOCK_EXTCLK);
            break;
        case MXC_ADC_CLK_IBRO:
            MXC_SYS_ClockSourceEnable(MXC_SYS_CLOCK_IBRO);
            break;
        case MXC_ADC_CLK_ERFO:
            MXC_SYS_ClockSourceEnable(MXC_SYS_CLOCK_ERFO);
            break;
        default:
            return E_BAD_PARAM;
    }

    MXC_SYS_Reset_Periph(MXC_SYS_RESET0_ADC);
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_ADC);
    
    MXC_ADC_ReferenceSelect(req->ref);

    return MXC_ADC_RevB_Init((mxc_adc_revb_regs_t*) MXC_ADC, req);
}

int MXC_ADC_Shutdown(void)
{
    MXC_ADC_RevB_Shutdown((mxc_adc_revb_regs_t*) MXC_ADC);
    
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_ADC);
    
    return E_NO_ERROR;
}

void MXC_ADC_EnableInt(uint32_t flags)
{
    MXC_ADC_RevB_EnableInt((mxc_adc_revb_regs_t*) MXC_ADC, flags);
}

void MXC_ADC_DisableInt(uint32_t flags)
{
    MXC_ADC_RevB_DisableInt((mxc_adc_revb_regs_t*) MXC_ADC, flags);
}

int MXC_ADC_GetFlags(void)
{
    return MXC_ADC_RevB_GetFlags((mxc_adc_revb_regs_t*) MXC_ADC);
}

void MXC_ADC_ClearFlags(uint32_t flags)
{
    MXC_ADC_RevB_ClearFlags((mxc_adc_revb_regs_t*) MXC_ADC, flags);
}

void MXC_ADC_ClockSelect(mxc_adc_clock_t clock)
{
    MXC_ADC_RevB_ClockSelect((mxc_adc_revb_regs_t*) MXC_ADC, clock);
}

int MXC_ADC_StartConversion(mxc_adc_conversion_req_t *req)
{
    initGPIOForChannel(req->channel);
    MXC_ADC_InputDividerSelect(req->channel, req->div, 0);

    if(req->trig == MXC_ADC_TRIG_HARDWARE) {
        initGPIOforHWTrig(req->hwTrig);
    }
    
    return MXC_ADC_RevB_StartConversion((mxc_adc_revb_regs_t*) MXC_ADC, req);
}

int MXC_ADC_StartConversionAsync(mxc_adc_conversion_req_t *req, mxc_adc_complete_cb_t callback)
{
    initGPIOForChannel(req->channel);
    MXC_ADC_InputDividerSelect(req->channel, req->div, 0);

    if(req->trig == MXC_ADC_TRIG_HARDWARE) {
        initGPIOforHWTrig(req->hwTrig);
    }
    
    return MXC_ADC_RevB_StartConversionAsync((mxc_adc_revb_regs_t*) MXC_ADC, req, callback);
}

int MXC_ADC_StartConversionDMA(mxc_adc_conversion_req_t *req, uint16_t* data, void (*callback)(int, int))
{
    initGPIOForChannel(req->channel);
    MXC_ADC_InputDividerSelect(req->channel, req->div, 0);

    if(req->trig == MXC_ADC_TRIG_HARDWARE) {
        initGPIOforHWTrig(req->hwTrig);
    }
    
    return MXC_ADC_RevB_StartConversionDMA((mxc_adc_revb_regs_t*) MXC_ADC, req, data, callback);
}

int MXC_ADC_Handler(void)
{
    return MXC_ADC_RevB_Handler((mxc_adc_revb_regs_t*) MXC_ADC);
}

int MXC_ADC_GetData(uint16_t* outdata)
{
    return MXC_ADC_RevB_GetData((mxc_adc_revb_regs_t*) MXC_ADC, outdata);
}

int MXC_ADC_EnableComparator(mxc_adc_comp_t comp, mxc_adc_chsel_t negCh, mxc_adc_chsel_t posCh)
{
    if(negCh > MXC_ADC_CH_3) {
        return E_BAD_PARAM;
    }

    if(posCh < MXC_ADC_CH_4 || posCh > MXC_ADC_CH_7) {
        return E_BAD_PARAM;
    }

    initGPIOForChannel(negCh);
    initGPIOForChannel(posCh);

    switch(comp) {
        case MXC_ADC_COMP_0:
        	MXC_SETFIELD(MXC_MCR->aincomp, MXC_F_MCR_AINCOMP_NSEL_COMP0, (1 << negCh) << MXC_F_MCR_AINCOMP_NSEL_COMP0_POS);
        	MXC_SETFIELD(MXC_MCR->aincomp, MXC_F_MCR_AINCOMP_PSEL_COMP0, (1 << posCh % 4) << MXC_F_MCR_AINCOMP_PSEL_COMP0_POS);
            break;
        case MXC_ADC_COMP_1:
            MXC_SETFIELD(MXC_MCR->aincomp, MXC_F_MCR_AINCOMP_NSEL_COMP1, (1 << negCh) << MXC_F_MCR_AINCOMP_NSEL_COMP1_POS);
            MXC_SETFIELD(MXC_MCR->aincomp, MXC_F_MCR_AINCOMP_PSEL_COMP1, (1 << posCh % 4) << MXC_F_MCR_AINCOMP_PSEL_COMP1_POS);
            break;
        default:
            return E_BAD_PARAM;
    }

    MXC_MCR->aincomp &= ~(comp << MXC_F_MCR_AINCOMP_PD_POS);

    return E_NO_ERROR;
}

int MXC_ADC_DisableComparator(mxc_adc_comp_t comp) 
{
    switch(comp) {
        case MXC_ADC_COMP_0:
            MXC_MCR->aincomp &= ~(MXC_F_MCR_AINCOMP_NSEL_COMP0 | MXC_F_MCR_AINCOMP_PSEL_COMP0);
            break;
        case MXC_ADC_COMP_1:
            MXC_MCR->aincomp &= ~(MXC_F_MCR_AINCOMP_NSEL_COMP1 | MXC_F_MCR_AINCOMP_PSEL_COMP1);
            break;
        default:
            return E_BAD_PARAM;
    }

    MXC_MCR->aincomp |= (comp << MXC_F_MCR_AINCOMP_PD_POS);

    return E_NO_ERROR;
}

int MXC_ADC_InputDividerSelect(mxc_adc_chsel_t ch, mxc_adc_divsel_t div, uint8_t lpEn) 
{
    uint32_t bitOffset;

    if(ch > MXC_ADC_CH_11 || div > MXC_ADC_DIV2_50K) {
        return E_BAD_PARAM;
    }

    bitOffset = ch << 1;
    MXC_MCR->adc_cfg2 &= ~(MXC_F_MCR_ADC_CFG2_CH << bitOffset);
    MXC_MCR->adc_cfg2 |= (div << bitOffset);

    if((div == MXC_ADC_DIV2_5K) && lpEn) {
        MXC_MCR->adc_cfg0 &= ~MXC_F_MCR_ADC_CFG0_LP_5K_DIS;
    }
    else if((div == MXC_ADC_DIV2_50K) && lpEn) {
        MXC_MCR->adc_cfg0 &= ~MXC_F_MCR_ADC_CFG0_LP_50K_DIS; 
    }

    return E_NO_ERROR;
}

int MXC_ADC_ReferenceSelect(mxc_adc_refsel_t ref)
{
    switch(ref) {
        case MXC_ADC_REF_EXT:
            MXC_MCR->adc_cfg0 |= MXC_F_MCR_ADC_CFG0_EXT_REF;
            break;
        case MXC_ADC_REF_INT_1V25:
            MXC_MCR->adc_cfg0 &= ~(MXC_F_MCR_ADC_CFG0_EXT_REF | MXC_F_MCR_ADC_CFG0_REF_SEL);
            break;
        case MXC_ADC_REF_INT_2V048:
            MXC_MCR->adc_cfg0 &= ~MXC_F_MCR_ADC_CFG0_EXT_REF;
            MXC_MCR->adc_cfg0 |= MXC_F_MCR_ADC_CFG0_REF_SEL;
            break;
        default:
            return E_BAD_PARAM;
    }

    return E_NO_ERROR;
}

int MXC_ADC_DynamicModeEn(mxc_adc_chsel_t ch)
{
    if(ch > MXC_ADC_CH_11) {
        return E_BAD_PARAM;
    }

    MXC_MCR->adc_cfg1 |= (1 << ch);

    return E_NO_ERROR;
}

int MXC_ADC_DynamicModeDis(mxc_adc_chsel_t ch)
{
    if(ch > MXC_ADC_CH_11) {
        return E_BAD_PARAM;
    }

    MXC_MCR->adc_cfg1 &= ~(1 << ch);

    return E_NO_ERROR;
}
