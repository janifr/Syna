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

//#include <stdio.h>
//#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "midi.h"
#include "organ.h"

extern uint8_t midibuffer[MIDI_BUFFER_LENGTH];

void Init_midi()
{
    GPIO_InitTypeDef GPIO_InitS;
    USART_InitTypeDef USART_InitS;
    NVIC_InitTypeDef NVIC_InitS;
    DMA_InitTypeDef DMA_InitS;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    GPIO_InitS.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitS.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitS.GPIO_OType = GPIO_OType_PP;
    GPIO_InitS.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitS);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

    USART_InitS.USART_BaudRate = 31250;
    USART_InitS.USART_WordLength = USART_WordLength_8b;
    USART_InitS.USART_StopBits = USART_StopBits_1;
    USART_InitS.USART_Parity = USART_Parity_No;
    USART_InitS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitS.USART_Mode = USART_Mode_Rx;
    USART_Init(USART3, &USART_InitS);

    NVIC_InitS.NVIC_IRQChannel = DMA1_Stream1_IRQn;
    NVIC_InitS.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitS.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitS.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitS);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    USART_Cmd(USART3, ENABLE);

    DMA_DeInit(DMA1_Stream1);

    DMA_InitS.DMA_Channel = DMA_Channel_4;
    DMA_InitS.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitS.DMA_Memory0BaseAddr = (uint32_t)&midibuffer;
    DMA_InitS.DMA_BufferSize = (uint16_t)sizeof(midibuffer);
    DMA_InitS.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
    DMA_InitS.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitS.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitS.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitS.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitS.DMA_Mode = DMA_Mode_Circular;
    DMA_InitS.DMA_Priority = DMA_Priority_High;
    DMA_InitS.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitS.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitS.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitS.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    DMA_Init(DMA1_Stream1, &DMA_InitS);

    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

    DMA_Cmd(DMA1_Stream1, ENABLE);
}


uint8_t New_input()
{
    static uint16_t prev_index=MIDI_BUFFER_LENGTH;
    uint16_t index, n=0;

    index = DMA_GetCurrDataCounter(DMA1_Stream1);

    if (index != prev_index)
    {
	if (index < prev_index)
	{
	    n = prev_index - index;
	}
	else
	{
	    n = prev_index - (index - MIDI_BUFFER_LENGTH);
	}
        prev_index = index;
    }
    return(n);
}


void Decode_midi(uint8_t n)
{
    static uint8_t index = 0;
    static uint8_t status = 0;
    static uint8_t message_index = 0;
    static uint8_t note, velocity;

    uint8_t byte;

    while (n > 0)
    {
	byte = midibuffer[index];
                
        if (byte & 0x80)
        {
            status=byte;
            message_index = 0;
        }
        else
        {
            switch (status & 0xF0)
            {
                case 0x90:
                    if (message_index == 0)
                    {
                        message_index++;
                        note=byte;
                    }
                    else
                    {
                        message_index=0;
                        velocity=byte;
                        if (velocity==0)
                            Organ_noteoff(note);
                        else
                            Organ_noteon(note);
                    }
                    break;
                case 0x80:
                    if (message_index == 0)
                    {
                        message_index++;
                        note=byte;
                    }
                    else
                    {
                        message_index=0;
                        velocity=byte;
                        Organ_noteoff(note);
                    }
                    break;
                default:
                    break;
            }
        }
        index ++;
        if (index == MIDI_BUFFER_LENGTH)
            index = 0;
        n--;
    }
}

