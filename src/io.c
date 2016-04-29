#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_audio_codec.h"
#include "organ.h"

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
