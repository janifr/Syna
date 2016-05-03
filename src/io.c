/* Yet unnamed clonewheel organ for STM32F4 Discovery, working name "Syna"

Copyright (C) 2016 Jani Frilander

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

//#define USE_STDPERIPH_DRIVER
//#include "stm32f4xx.h"
//#include <stdio.h>
//#include "stm32f4_discovery.h"
//#include "stm32f4xx_conf.h"
//#include "stm32f4xx_dma.h"
//#include "stm32f4xx_gpio.h"
//#include "stm32f4xx_rcc.h"
//#include "stm32f4xx_adc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_audio_codec.h"
#include "organ.h"


#define ADC1_DR_ADDRESS ((uint32_t)0x4001204C)

/*__IO*/// uint16_t ADCConvertedValue = 0;

//uint8_t buf[18];

uint8_t Drawchars[]=
{ 0x1f,0,0,0,0,0,0,0,
  0x1f,0x1f,0,0,0,0,0,0,
  0x1f,0x1f,0x1f,0,0,0,0,0,
  0x1f,0x1f,0x1f,0x1f,0,0,0,0,
  0x1f,0x1f,0x1f,0x1f,0x1f,0,0,0,
  0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0,0,
  0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0,
  0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f};

uint8_t Barchartcoarse[]=
{ 32,32,32,32,
  3,32,32,32,
  7,32,32,32,
  7,3,32,32,
  7,7,32,32,
  7,7,3,32,
  7,7,7,32,
  7,7,7,3,
  7,7,7,7};

uint8_t Barchartfine[]=
{ 32,32,32,32,
  0,32,32,32,
  1,32,32,32,
  2,32,32,32,
  3,32,32,32,
  4,32,32,32,
  5,32,32,32,
  6,32,32,32,
  7,32,32,32,
  7,0,32,32,
  7,1,32,32,
  7,2,32,32,
  7,3,32,32,
  7,4,32,32,
  7,5,32,32,
  7,6,32,32,
  7,7,32,32,
  7,7,0,32,
  7,7,1,32,
  7,7,2,32,
  7,7,3,32,
  7,7,4,32,
  7,7,5,32,
  7,7,6,32,
  7,7,7,32,
  7,7,7,0,
  7,7,7,1,
  7,7,7,2,
  7,7,7,3,
  7,7,7,4,
  7,7,7,5,
  7,7,7,6,
  7,7,7,7};
/*
void Init_adc()
{
    ADC_InitTypeDef ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_144Cycles);

    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);
}

void assert_failed(uint8_t* file, uint32_t line)
{
    while(1)
    {
    }
}*/
//Quick hack, approximately 1ms delay
void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=20000;
      while (x-- > 0)
         __asm("nop");
   }
}

void Display_command_clock(uint8_t chr)
{
    GPIOD->ODR &= ~2;
    GPIOD->ODR |= 8;
    GPIOE->ODR = (chr<<7);
    ms_delay(1);
    GPIOD->ODR &= ~8;
    ms_delay(1);
}

void Display_character_clock(uint8_t chr)
{
    GPIOD->ODR |= 2;
    GPIOD->ODR |= 8;
    GPIOE->ODR = (chr<<7);
    ms_delay(1);
    GPIOD->ODR &= ~8;
    ms_delay(1);
}

void Display_command_no_clock(uint8_t chr)
{
    GPIOD->ODR &= ~2;
    GPIOD->ODR |= 8;
    GPIOE->ODR = (chr<<7);
}

void Display_character_no_clock(uint8_t chr)
{
    GPIOD->ODR |= 2;
    GPIOD->ODR |= 8;
    GPIOE->ODR = (chr<<7);
}

void Display_data_no_clock(uint16_t data)
{
/*    if (0x2 & data)
        GPIOD->ODR |= 0x2;
    else
        GPIOD->ODR &= ~2;*/
    GPIOD->ODR &= ~2;
    GPIOD->ODR |= (0x2 & data);
    GPIOD->ODR |= 8;
    GPIOE->ODR = (data & 0x7f80);
}

void Display_clock()
{
    GPIOD->ODR &= ~8;
}

void Init_customchars()
{
    int32_t i;
    Display_command_clock(0x40);
    for (i=0;i<64;i++)
        Display_character_clock(Drawchars[i]);
    Display_command_clock(0x80);  
}


void Init_hid(void)
{
    //int16_t x,y;

    //init_adc();
        
    //ADC_SoftwareStartConv(ADC1);
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN;
    __asm("dsb");
    GPIOD->MODER = 0x04000044;
    GPIOE->MODER = 0x15554000;

    ms_delay(500); 
    Display_command_clock(0x38);
    ms_delay(4);
    Display_command_clock(0x38);
    Display_command_clock(0x38);
    Display_command_clock(0x8);
    Display_command_clock(0x1);
    Display_command_clock(0x6);
    Display_command_clock(0xc);
    Init_customchars();
}
/*
    for (;;) {
        disp_cmd(0x80);
        x = ((((ADCConvertedValue>>8)+4)>>3));
        y = ((((ADCConvertedValue>>8)+16)>>5));
       snprintf(buf,18,"%x %x",x,ADCConvertedValue);
       disp_cmd(0x82);
        disp_char(buf[0]);
        disp_char(buf[1]);
        disp_char(buf[2]);
        disp_char(buf[3]);
        disp_char(buf[4]);
        disp_char(buf[5]);
        disp_char(buf[6]);
        disp_char(buf[7]);
        disp_char(buf[8]);
        disp_char(buf[9]);

        disp_cmd(0x80);
        disp_char(barchartfine[x<<2]);
        disp_cmd(0xc0);
        disp_char(barchartfine[(x<<2)+1]);
        disp_cmd(0x94);
        disp_char(barchartfine[(x<<2)+2]);
        disp_cmd(0xd4);
        disp_char(barchartfine[(x<<2)+3]);
        disp_cmd(0x81);
        disp_char(barchartcoarse[y<<2]);
        disp_cmd(0xc1);
        disp_char(barchartcoarse[(y<<2)+1]);
        disp_cmd(0x95);
        disp_char(barchartcoarse[(y<<2)+2]);
        disp_cmd(0xd5);
        disp_char(barchartcoarse[(y<<2)+3]);
 
       ms_delay(20);
       GPIOD->ODR ^= (1 << 13);            
    }
}*/

void Init_misc()
{
    GPIO_InitTypeDef  	GPIO_InitS;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitS.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
    GPIO_InitS.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitS.GPIO_OType = GPIO_OType_PP;
    GPIO_InitS.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitS.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitS);

    GPIO_InitS.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitS.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitS.GPIO_OType = GPIO_OType_PP;
    GPIO_InitS.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitS.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitS);
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{
    GPIO_SetBits(GPIOD, GPIO_Pin_12);
    Generate_buffer(AUDIO_BUFFER_LENGTH_HALF);
}

void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_12);
	Generate_buffer(0);
}

uint16_t EVAL_AUDIO_GetSampleCallBack(void)
{
    return 0;
}

uint32_t Codec_TIMEOUT_UserCallback(void)
{
    return (0);
}

void USART3_IRQHandler(void)
{
}

void DMA1_Stream1_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1))
    {
	DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
    }
}
