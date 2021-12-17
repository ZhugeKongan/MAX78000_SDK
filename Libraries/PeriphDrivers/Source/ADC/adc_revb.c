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
#include "dma.h"
#include "adc_revb.h"
#include "adc_regs.h"

#include "mxc_device.h"
#include "mxc_errors.h"
#include "mxc_assert.h"
#include "mxc_delay.h"
#include "mxc_sys.h"
#include "mcr_regs.h"
#include "mxc_lock.h"
#include "mxc_delay.h"
#include <stdio.h>

// Mask for all Interrupt Enable Fields
#define ADC_IE_MASK (MXC_F_ADC_REVB_INTEN_READY       | MXC_F_ADC_REVB_INTEN_ABORT    | MXC_F_ADC_REVB_INTEN_START_DET | \
                     MXC_F_ADC_REVB_INTEN_SEQ_STARTED | MXC_F_ADC_REVB_INTEN_SEQ_DONE | MXC_F_ADC_REVB_INTEN_CONV_DONE | \
                     MXC_F_ADC_REVB_INTEN_CLIPPED     | MXC_F_ADC_REVB_INTEN_FIFO_LVL | MXC_F_ADC_REVB_INTEN_FIFO_UFL  | \
                     MXC_F_ADC_REVB_INTEN_FIFO_OFL )

#define ADC_IF_MASK (MXC_F_ADC_REVB_INTFL_READY       | MXC_F_ADC_REVB_INTFL_ABORT    | MXC_F_ADC_REVB_INTFL_START_DET | \
                     MXC_F_ADC_REVB_INTFL_SEQ_STARTED | MXC_F_ADC_REVB_INTFL_SEQ_DONE | MXC_F_ADC_REVB_INTFL_CONV_DONE | \
                     MXC_F_ADC_REVB_INTFL_CLIPPED     | MXC_F_ADC_REVB_INTFL_FIFO_LVL | MXC_F_ADC_REVB_INTFL_FIFO_UFL  | \
                     MXC_F_ADC_REVB_INTFL_FIFO_OFL )

static mxc_adc_complete_cb_t async_callback;
static mxc_adc_conversion_req_t* async_req;
// static volatile uint8_t flag;      //indicates  to irqhandler where to store data

int MXC_ADC_RevB_Init (mxc_adc_revb_regs_t* adc, mxc_adc_req_t *req)
{
    if(req == NULL) {
        return E_NULL_PTR;
    }

   if((req->trackCount<4) || (req->idleCount<17)) {
           return E_BAD_PARAM;
   }
    //Power up to Sleep State
    adc->clkctrl |= (req->clock << MXC_F_ADC_REVB_CLKCTRL_CLKSEL) & MXC_F_ADC_REVB_CLKCTRL_CLKSEL;

    adc->clkctrl |= (req->clkdiv << MXC_F_ADC_REVB_CLKCTRL_CLKDIV_POS) & MXC_F_ADC_REVB_CLKCTRL_CLKDIV;

    adc->ctrl0 |= MXC_F_ADC_REVB_CTRL0_RESETB;

    //Move to NAP state
    adc->ctrl0 |= MXC_F_ADC_REVB_CTRL0_BIAS_EN;
    MXC_Delay(500);    

    //calibration
    if(req->cal == MXC_ADC_EN_CAL) {
        adc->ctrl0 &= ~MXC_F_ADC_REVB_CTRL0_SKIP_CAL;
    }
    else {
        adc->ctrl0 |= MXC_F_ADC_REVB_CTRL0_SKIP_CAL;
    }

    adc->sampclkctrl |= (req->trackCount << MXC_F_ADC_REVB_SAMPCLKCTRL_TRACK_CNT_POS) & MXC_F_ADC_REVB_SAMPCLKCTRL_TRACK_CNT;
    adc->sampclkctrl |= (req->idleCount << MXC_F_ADC_REVB_SAMPCLKCTRL_IDLE_CNT_POS) & MXC_F_ADC_REVB_SAMPCLKCTRL_IDLE_CNT;

    adc->ctrl0 |= MXC_F_ADC_REVB_CTRL0_ADC_EN;

    //wait for calibration to complete
    while(!(adc->intfl & MXC_F_ADC_REVB_INTFL_READY));

    async_callback = NULL;
    
    async_req = NULL;
    
    return E_NO_ERROR;
}

int MXC_ADC_RevB_Shutdown (mxc_adc_revb_regs_t* adc)
{   
    if (async_callback != NULL) {
        MXC_FreeLock((uint32_t*) &async_callback);
    }
    
    if (async_req != NULL) {
        MXC_FreeLock((uint32_t*) &async_req);
    }
    
    adc->ctrl0 &= ~MXC_F_ADC_REVB_CTRL0_ADC_EN;

    return E_NO_ERROR;
}

void MXC_ADC_RevB_EnableInt (mxc_adc_revb_regs_t* adc, uint32_t flags)
{
    adc->inten |= (flags & ADC_IE_MASK);
}

void MXC_ADC_RevB_DisableInt (mxc_adc_revb_regs_t* adc, uint32_t flags)
{
    adc->inten &= ~(flags & ADC_IE_MASK);
}

int MXC_ADC_RevB_GetFlags (mxc_adc_revb_regs_t* adc)
{
    return (adc->intfl & ADC_IF_MASK);
}

void MXC_ADC_RevB_ClearFlags (mxc_adc_revb_regs_t* adc, uint32_t flags)
{
    // Write 1 to clear flags
    adc->intfl |= (flags & ADC_IF_MASK);
}

void MXC_ADC_RevB_ClockSelect(mxc_adc_revb_regs_t* adc, mxc_adc_clock_t clock)
{
    adc->clkctrl |= (clock << MXC_F_ADC_REVB_CLKCTRL_CLKSEL) & MXC_F_ADC_REVB_CLKCTRL_CLKSEL;
}

int MXC_ADC_RevB_StartConversion (mxc_adc_revb_regs_t* adc, mxc_adc_conversion_req_t *req)
{
// TODO: This should be checked in the chip specific ADC source file
    // if (req->channel > MXC_ADC_CH_8) {
    //     return E_BAD_PARAM;
    // }

    //number of samples to average
    adc->ctrl1 |= (req->avg_number << MXC_F_ADC_REVB_CTRL1_AVG_POS);

    // select between software and hardware trigger
    if(req->trig == MXC_ADC_TRIG_SOFTWARE) {
        adc->ctrl1 &= ~MXC_F_ADC_REVB_CTRL1_TRIG_MODE;
    }
    else {
        adc->ctrl1 |= MXC_F_ADC_REVB_CTRL1_TRIG_MODE;
        MXC_SETFIELD(adc->ctrl1, MXC_F_ADC_REVB_CTRL1_TRIG_SEL, (req->hwTrig << MXC_F_ADC_CTRL1_TRIG_SEL_POS));
    }

    // Select channel for ADC conversion
    MXC_SETFIELD(adc->ctrl1, MXC_F_ADC_REVB_CTRL1_NUM_SLOTS, 0 << MXC_F_ADC_REVB_CTRL1_NUM_SLOTS_POS);
    MXC_SETFIELD(adc->chsel0, MXC_F_ADC_REVB_CHSEL0_SLOT0_ID, req->channel << MXC_F_ADC_REVB_CHSEL0_SLOT0_ID_POS);

    // select between atomic and continuous conversion mode
    switch(req->mode) {
    	case MXC_ADC_ATOMIC_CONV:
    		adc->ctrl1 &= ~MXC_F_ADC_REVB_CTRL1_CNV_MODE;
            break;
    	case MXC_ADC_CONTINUOUS_CONV:
    	default:
    		return E_BAD_PARAM;
    }

    MXC_ADC_ClearFlags(MXC_F_ADC_REVB_INTFL_SEQ_DONE | MXC_F_ADC_REVB_INTFL_START_DET);

    adc->ctrl1 |= MXC_F_ADC_REVB_CTRL1_START;
    while((adc->intfl & MXC_F_ADC_REVB_INTFL_SEQ_DONE) == 0);
    adc->ctrl1 &= ~MXC_F_ADC_REVB_CTRL1_START;
    
    return E_NO_ERROR;    
}

int MXC_ADC_RevB_StartConversionAsync (mxc_adc_revb_regs_t* adc, mxc_adc_conversion_req_t *req, mxc_adc_complete_cb_t callback)
{
// TODO: This should be checked in the chip specific ADC source file
    // if (req->channel > MXC_ADC_CH_8) {
    //     return E_BAD_PARAM;
    // }

    if(callback == NULL) {
        return E_BAD_PARAM;
    }

    while (MXC_GetLock((uint32_t*) &async_callback, (uint32_t) callback) != E_NO_ERROR);

    //number of samples to average
    adc->ctrl1 |= (req->avg_number << MXC_F_ADC_REVB_CTRL1_AVG_POS);

    // select between software and hardware trigger
    if(req->trig == MXC_ADC_TRIG_SOFTWARE) {
        adc->ctrl1 &= ~MXC_F_ADC_REVB_CTRL1_TRIG_MODE;
    }
    else {
        adc->ctrl1 |= MXC_F_ADC_REVB_CTRL1_TRIG_MODE;
        MXC_SETFIELD(adc->ctrl1, MXC_F_ADC_REVB_CTRL1_TRIG_SEL, (req->hwTrig << MXC_F_ADC_CTRL1_TRIG_SEL_POS));
    }

    // Clear interrupt flags
    MXC_ADC_RevB_ClearFlags(adc, ADC_IF_MASK);

    // Select channel for ADC Conversion
    MXC_SETFIELD(adc->ctrl1, MXC_F_ADC_REVB_CTRL1_NUM_SLOTS, 0 << MXC_F_ADC_REVB_CTRL1_NUM_SLOTS_POS);
    MXC_SETFIELD(adc->chsel0, MXC_F_ADC_REVB_CHSEL0_SLOT0_ID, req->channel << MXC_F_ADC_REVB_CHSEL0_SLOT0_ID_POS);

    // select between atomic and continuous conversion mode
    switch(req->mode) {
        case MXC_ADC_ATOMIC_CONV:
            adc->ctrl1 &= ~MXC_F_ADC_REVB_CTRL1_CNV_MODE;
            break; 
        case MXC_ADC_CONTINUOUS_CONV:
        default:
            return E_BAD_PARAM;
    }

    MXC_ADC_RevB_EnableInt(adc, MXC_F_ADC_REVB_INTEN_SEQ_DONE);

    adc->ctrl1 |= MXC_F_ADC_REVB_CTRL1_START;
    
    return E_NO_ERROR;  
}

int MXC_ADC_RevB_StartConversionDMA (mxc_adc_revb_regs_t* adc, mxc_adc_conversion_req_t *req, uint16_t *data, void (*callback) (int, int))
{
// TODO: This should be checked in the chip specific ADC source file
    // if (req->channel > MXC_ADC_CH_8) {
    //     return E_BAD_PARAM;
    // }

    if(callback == NULL) {
        return E_BAD_PARAM;
    }

    if (data == NULL) {
        return E_NULL_PTR;
    }

    uint8_t channel;
    mxc_dma_config_t config;    
    mxc_dma_srcdst_t srcdst;

    //number of samples to average
    adc->ctrl1 |= (req->avg_number << MXC_F_ADC_REVB_CTRL1_AVG_POS);

    // select between software and hardware trigger
    if(req->trig == MXC_ADC_TRIG_SOFTWARE) {
        adc->ctrl1 &= ~MXC_F_ADC_REVB_CTRL1_TRIG_MODE;
    }
    else {
        adc->ctrl1 |= MXC_F_ADC_REVB_CTRL1_TRIG_MODE;
        MXC_SETFIELD(adc->ctrl1, MXC_F_ADC_REVB_CTRL1_TRIG_SEL, (req->hwTrig << MXC_F_ADC_REVB_CTRL1_TRIG_SEL_POS));
    }

    // Clear interrupt flags
    MXC_ADC_RevB_ClearFlags(adc, ADC_IF_MASK);

    //Set channel selection
    MXC_SETFIELD(adc->ctrl1, MXC_F_ADC_REVB_CTRL1_NUM_SLOTS, 0 << MXC_F_ADC_REVB_CTRL1_NUM_SLOTS_POS);
    MXC_SETFIELD(adc->chsel0, MXC_F_ADC_REVB_CHSEL0_SLOT0_ID, req->channel << MXC_F_ADC_REVB_CHSEL0_SLOT0_ID_POS);

    //Select conversion mode
    switch(req->mode) {
        case MXC_ADC_ATOMIC_CONV:
            adc->ctrl1 &= ~MXC_F_ADC_REVB_CTRL1_CNV_MODE;
            break; 
        case MXC_ADC_CONTINUOUS_CONV:
        default:
            return E_BAD_PARAM;
    }

    adc->fifodmactrl |= MXC_F_ADC_REVB_FIFODMACTRL_FLUSH;                   //Flush data FIFO
    adc->fifodmactrl |= MXC_S_ADC_REVB_FIFODMACTRL_DATA_FORMAT_DATA_STATUS; //Transfer data and status bits
    adc->fifodmactrl |= (0 << MXC_F_ADC_REVB_FIFODMACTRL_THRESH_POS);       //Start DMA transfer when 1 ADC conversion has finished
    adc->fifodmactrl |= MXC_F_ADC_REVB_FIFODMACTRL_DMA_EN;                  //Enable ADC DMA
        
    channel = MXC_DMA_AcquireChannel();
    
    config.reqsel = MXC_S_DMA_CTRL_REQUEST_ADC;
    config.ch = channel;
    
    config.srcwd = MXC_DMA_WIDTH_WORD;
    config.dstwd = MXC_DMA_WIDTH_WORD;
    
    config.srcinc_en = 0;
    config.dstinc_en = 1;
    
    srcdst.ch      = channel;
    srcdst.source  = NULL;
    srcdst.dest    = data;
    srcdst.len     = 4;
    
    MXC_DMA_ConfigChannel(config, srcdst);
    
    MXC_DMA_SetCallback(channel, callback);

    MXC_DMA->ch[channel].ctrl |= 4 << MXC_F_DMA_CTRL_BURST_SIZE_POS;
      
    MXC_DMA_EnableInt(channel);

    MXC_DMA_Start(channel);
    MXC_DMA_SetChannelInterruptEn(channel, 0, 1);

    adc->ctrl1 |= MXC_F_ADC_REVB_CTRL1_START;

    return E_NO_ERROR;
}

int MXC_ADC_RevB_Handler (mxc_adc_revb_regs_t* adc)
{
    uint32_t flags;
    uint16_t data;
    int error;
    
    error = MXC_ADC_RevB_GetData(adc, &data);

    flags = MXC_ADC_RevB_GetFlags(adc);

    if(flags & (MXC_F_ADC_REVB_INTEN_SEQ_DONE | MXC_F_ADC_REVB_INTEN_CONV_DONE)) {
        mxc_adc_complete_cb_t cb = async_callback;

        if (flags & MXC_F_ADC_REVB_INTEN_SEQ_DONE) {
            MXC_ADC_RevB_ClearFlags(adc, flags);
            MXC_ADC_RevB_DisableInt(adc, (MXC_F_ADC_REVB_INTFL_SEQ_DONE | MXC_F_ADC_REVB_INTFL_CONV_DONE));
            MXC_FreeLock((uint32_t*) &async_callback);
        }
        
        if (flags & MXC_F_ADC_REVB_INTEN_CONV_DONE) {
            MXC_ADC_RevB_ClearFlags(adc, MXC_F_ADC_REVB_INTFL_CONV_DONE);
        }  

        if(error != E_NO_ERROR) {
            (cb)(NULL, error);
        }
        else {
            (cb)(NULL, data);
        }
    }

    return E_NO_ERROR;  
}

// ************************************* Function to Read ADC Data *******************************************
int MXC_ADC_RevB_GetData (mxc_adc_revb_regs_t* adc, uint16_t *outdata)
{
	uint32_t dataReg = adc->data;

    if(dataReg & MXC_F_ADC_REVB_DATA_INVALID) {
        return E_INVALID;
    }

    *outdata = (dataReg & MXC_F_ADC_REVB_DATA_DATA);

    return E_NO_ERROR;
}
