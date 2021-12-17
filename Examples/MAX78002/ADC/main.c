/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
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
******************************************************************************/

/**
 * @file    main.c
 * @brief   ADC demo application
 * @details Continuously monitors the ADC channels
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>

#include "mxc_errors.h"
#include "adc.h"
#include "dma.h"
#include "led.h"
#include "tmr.h"

/***** Definitions *****/

#define POLLING        // Uncomment to perform ADC conversions using blocking/polling method
// #define INTERRUPT    // Uncomment to perform ADC conversions using interrupt driven method
// #define DMA            // Uncomment to perform ADC conversions using DMA driven method.

#define CH  MXC_ADC_CH_0

/***** Globals *****/
#ifdef INTERRUPT
volatile unsigned int adc_done = 0;
#endif

#ifdef DMA
volatile unsigned int dma_done = 0;
#endif

uint32_t adc_val;
int invalid;

/***** Functions *****/

#ifdef INTERRUPT
void adc_complete_cb(void* req, int adcRead)
{
    invalid = (adcRead == E_INVALID ? 1 : 0);
    adc_val = adcRead;
    adc_done = 1;
    return;
}

void ADC_IRQHandler(void)
{
    MXC_ADC->ctrl1 &= ~MXC_F_ADC_CTRL1_START;

    MXC_ADC_Handler();
}
#endif

#ifdef DMA
void ADC_IRQHandler(void)
{
    MXC_ADC->intfl = -1;   // Clear all interurpts
}

void adc_dma_callback(int ch, int err) {
    if(err != E_NO_ERROR) {
        adc_val = err;
    }

    MXC_ADC->ctrl1 &= ~MXC_F_ADC_CTRL1_START;
    dma_done = 1;
}

void DMA0_IRQHandler(void) {
    MXC_DMA_Handler();
}
#endif

int main(void)
{
    printf("********** ADC Example **********\n");
    printf("\nThe voltage applied to analog pin %d is continuously\n", CH);
    printf("measured and the result is printed to the terminal.\n");
    printf("\nThe example can be configured to take the measurements\n");
    printf("by polling, using interrupts, or using DMA.\n\n");
    
    mxc_adc_req_t adc_cfg;
    adc_cfg.clock = MXC_ADC_CLK_HCLK;
    adc_cfg.clkdiv = MXC_ADC_CLKDIV_16;
    adc_cfg.cal = MXC_ADC_SKIP_CAL;
    adc_cfg.trackCount = 4;
    adc_cfg.idleCount = 17;
    adc_cfg.ref = MXC_ADC_REF_INT_1V25;

    mxc_adc_conversion_req_t adc_conv;
    adc_conv.mode = MXC_ADC_ATOMIC_CONV;
    adc_conv.trig = MXC_ADC_TRIG_SOFTWARE;
    adc_conv.avg_number = MXC_ADC_AVG_1;
    adc_conv.channel = CH;
    adc_conv.div = MXC_ADC_DIV1;

    /* Initialize ADC */
    if (MXC_ADC_Init(&adc_cfg) != E_NO_ERROR) {
        printf("Error Bad Parameter\n");
        while (1);
    }

#if defined(INTERRUPT) || defined(DMA)
    NVIC_EnableIRQ(ADC_IRQn);
#endif

#ifdef DMA
    MXC_DMA_Init();
    NVIC_EnableIRQ(DMA0_IRQn);
#endif

    while (1) {
        /* Flash LED when starting ADC cycle */
        LED_On(0);
        MXC_TMR_Delay(MXC_TMR0, MSEC(10));
        LED_Off(0);
        
        /* Convert channel 0 */
#ifdef POLLING
        MXC_ADC_StartConversion(&adc_conv);

        invalid = MXC_ADC_GetData((uint16_t*) &adc_val);
        invalid = (invalid == E_INVALID ? 1 : 0);
#endif

#ifdef INTERRUPT
        adc_done = 0;
        MXC_ADC_StartConversionAsync(&adc_conv, adc_complete_cb);
        
        while (!adc_done) {};
#endif

#ifdef DMA
        dma_done = 0;
        MXC_DMA_ReleaseChannel(0);
        MXC_ADC_StartConversionDMA(&adc_conv, (uint16_t*) &adc_val, adc_dma_callback);

        while(!dma_done) {};
        invalid = ((adc_val & MXC_F_ADC_DATA_INVALID) == MXC_F_ADC_DATA_INVALID ? 1 : 0);
#endif

        /* Display results, display asterisk if invalid */
        printf("%x: 0x%04x%s\n\n", adc_conv.channel, (uint16_t) adc_val, invalid ? "*" : " ");
        printf("                   ");
        printf("\n");

        /* Delay for 1/4 second before next reading */
        MXC_TMR_Delay(MXC_TMR0, MSEC(2000));
    }
}
