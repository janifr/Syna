//#define USE_STDPERIPH_DRIVER
#include "stm32f4xx.h"
#include <stdio.h>
#include "stm32f4_discovery.h"
//#include "stm32f4xx_conf.h"
//#include "stm32f4xx_dma.h"
//#include "stm32f4xx_gpio.h"
//#include "stm32f4xx_rcc.h"
//#include "stm32f4xx_adc.h"

#define ADC1_DR_ADDRESS ((uint32_t)0x4001204C)

/*__IO*/ uint16_t ADCConvertedValue = 0;

uint8_t buf[18];

uint8_t drawchars[]=
{ 0x1f,0,0,0,0,0,0,0,
  0x1f,0x1f,0,0,0,0,0,0,
  0x1f,0x1f,0x1f,0,0,0,0,0,
  0x1f,0x1f,0x1f,0x1f,0,0,0,0,
  0x1f,0x1f,0x1f,0x1f,0x1f,0,0,0,
  0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0,0,
  0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0,
  0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f};

uint8_t barchartcoarse[]=
{ 32,32,32,32,
  3,32,32,32,
  7,32,32,32,
  7,3,32,32,
  7,7,32,32,
  7,7,3,32,
  7,7,7,32,
  7,7,7,3,
  7,7,7,7};

uint8_t barchartfine[]=
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

void init_adc()
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
}
//Quick hack, approximately 1ms delay
void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=5971;
      while (x-- > 0)
         __asm("nop");
   }
}

void disp_cmd(uint8_t chr)
{
    GPIOD->ODR &= ~2;
    GPIOD->ODR |= 8;
    GPIOE->ODR = (chr<<7);
    ms_delay(1);
    GPIOD->ODR &= ~8;
    ms_delay(1);
}

void disp_char(uint8_t chr)
{
    GPIOD->ODR |= 2;
    GPIOD->ODR |= 8;
    GPIOE->ODR = (chr<<7);
    ms_delay(1);
    GPIOD->ODR &= ~8;
    ms_delay(1);
}

void init_customchars()
{
    int32_t i;
    disp_cmd(0x40);
    for (i=0;i<64;i++)
        disp_char(drawchars[i]);
    disp_cmd(0x80);  
}


//Flash orange LED at about 1hz
int main(void)
{
    int16_t x,y;
   // uint8_t chr='A';
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN;  // enable the clock to GPIOD
    __asm("dsb");                         // stall instruction pipeline, until instruction completes, as
                                          //    per Errata 2.1.13, "Delay after an RCC peripheral clock enabling"
    GPIOD->MODER = 0x04000044; //(1 << 26)|(1<<2)|(1<<6)
    GPIOE->MODER = 0x15554000;

    buf[0]=32;
    buf[1]=32;
    buf[2]=32;
    buf[3]=32;
 
    init_adc();
        
    ADC_SoftwareStartConv(ADC1);

    ms_delay(500); 
    disp_cmd(0x38);
    ms_delay(4);
    disp_cmd(0x38);
    disp_cmd(0x38);
    disp_cmd(0x8);
    disp_cmd(0x1);
    disp_cmd(0x6);
    disp_cmd(0xc);
      init_customchars(); 
    for (;;) {
        disp_cmd(0x80);
        //x = ADC_GetConversionValue(ADC1);
        //chr = (uint8_t) ((x>>4) & 0xff);
        x = ((((ADCConvertedValue>>8)+4)>>3));
        y = ((((ADCConvertedValue>>8)+16)>>5));
        //x = (ADCConvertedValue>>11);
        //x=ADCConvertedValue;
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
//        chr = chr<<2;
//        x &= 0x3f;

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
 
        /*for(x=0;x<9;x++)
        {
            if (chr>x)
                disp_char(2);
            else
                disp_char(0);
        }*/
        

       ms_delay(20);
       GPIOD->ODR ^= (1 << 13);           // Toggle the pin 
//        chr++;
//        if(chr>'F')
//            chr='A';
    }
}
